#include "Parser.h"
#include "AST.h"
#include "Error.h"
#include "Token.h"
#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <memory>
#include <ostream>
#include <set>

namespace {
const auto builtin_types =
    std::set<TokenType>{TokenType::FLOAT, TokenType::STRING};
}

Parser::Parser(std::string_view filename, const std::vector<Token> &tokens)
    : filename_{filename}, tokens_{tokens} {}

auto Parser::parse() -> ProgramNode & {
  // -1 is there for the EOF token
  while (pos_ < tokens_.size() - 1) {
    result_.Statements.push_back(std::move(parseStatement()));
    if (is_panic_) {
      handlePanic();
    }
  }

  return result_;
}

auto Parser::handlePanic() -> void {
  while (peek().Type != TokenType::END_OF_FILE) {
    switch (peek().Type) {
    case TokenType::CLASS:
    case TokenType::FN:
    case TokenType::PRINT:
    case TokenType::ARRAY:
      return;
    case TokenType::SEMICOLON:
      consume(); // semi colon is not a great way to start statements
      return;
    default:
      break;
    }
    consume();
  }
}

auto Parser::expect(TokenType type, std::string_view error) -> bool {
  const auto &curr_token = tokens_[pos_];
  if (curr_token.Type != type) {
    reportError(error, filename_, curr_token.Line);
    return false;
  }
  return true;
}

auto Parser::consume() -> Token {
  ++pos_;
  return tokens_[pos_ - 1];
}

auto Parser::peek(std::size_t n) -> Token {
  if (pos_ + n < tokens_.size()) {
    return tokens_[pos_ + n];
  }
  return Token{.Type = TokenType::END_OF_FILE};
}

auto Parser::parseStatement() -> StatementPtr {
  switch (peek().Type) {
  case TokenType::PRINT:
    return parsePrint();
  case TokenType::IDENTIFIER:
    return parseIdentifier();
  case TokenType::L_BRACE:
    return parseBlock();
  default: {
    auto invalid_stmt = std::make_unique<InvalidStatement>();
    invalid_stmt->Line = tokens_[pos_].Line; // a little lazy but close
    is_panic_ = true;
    reportError(filename_, std::format("Invalid statement {}!", peek().Lexeme),
                invalid_stmt->Line);
    return invalid_stmt;
  }
  }
}

auto Parser::parseExpression() -> ExpressionPtr { return parseEquality(); }

auto Parser::parseEquality() -> ExpressionPtr {
  // take in the left side of the operation
  auto this_node = parseRelational();

  while (pos_ < tokens_.size() && (peek().Type == TokenType::EQUALITY) ||
         (peek().Type == TokenType::INEQUALITY)) {
    const auto &this_tok = consume();
    auto op = this_tok.Type; // get that juicy operator
    // take in the right side of the operation
    this_node =
        std::make_unique<BinaryOperation>(op, this_node, parseEquality());
    this_node->Line = this_tok.Line;
  }
  // !NOTE: unique_ptr safely casts to base types with a converting constructor
  return this_node;
}

auto Parser::parseRelational() -> ExpressionPtr {
  auto this_node = parseTerm(); // left hand side
  // all the relational operators
  auto relational_token_types = std::array<TokenType, 4>{
      TokenType::LESS_THAN, TokenType::LESS_THAN_OR_EQUAL,
      TokenType::GREATER_THAN, TokenType::GREATER_THAN_OR_EQUAL};
  // parse
  while (pos_ < tokens_.size() &&
         std::find(relational_token_types.begin(), relational_token_types.end(),
                   peek().Type) != relational_token_types.end()) {
    const auto &this_tok = consume();
    auto op = this_tok.Type; // juicy...
    // get the right side
    this_node = std::make_unique<BinaryOperation>(op, this_node, parseTerm());
    this_node->Line = this_tok.Line;
  }
  return this_node;
}

auto Parser::parseTerm() -> ExpressionPtr {
  // get the left side
  auto this_node = parseFactor();
  // parse the other terms
  while (pos_ < tokens_.size() &&
         (peek().Type == TokenType::PLUS || peek().Type == TokenType::MINUS)) {
    const auto &this_tok = consume();
    auto op = this_tok.Type;
    // handle the right hand side
    this_node = std::make_unique<BinaryOperation>(op, this_node, parseFactor());
    this_node->Line = this_tok.Line;
  }
  return this_node;
}

auto Parser::parseFactor() -> ExpressionPtr {
  auto this_node = parseUnary(); // parse left
  // parse the other factors
  while (pos_ < tokens_.size() &&
         (peek().Type == TokenType::MUL || peek().Type == TokenType::DIV)) {
    const auto &this_tok = consume();
    auto op = this_tok.Type;
    // handle the right hand side
    this_node = std::make_unique<BinaryOperation>(op, this_node, parseUnary());
    this_node->Line = this_tok.Line;
  }
  return this_node;
}

auto Parser::parseUnary() -> ExpressionPtr {
  // expect there to be a prefixed ! or - operator
  if (peek().Type == TokenType::MINUS || peek().Type == TokenType::NOT) {
    const auto &this_tok = consume();
    auto op = this_tok.Type;
    // handle the operand
    auto this_node = std::make_unique<UnaryOperation>(op, parsePrimary());
    this_node->Line = this_tok.Line;
    return this_node;
  }
  // otherwise its a literal or grouping
  return parsePrimary();
}

auto Parser::parsePrimary() -> ExpressionPtr {
  switch (peek().Type) {
  default: {
    // TODO: error handling later
    reportError(
        std::format("Expected a valid expression, got '{}'!", peek().Lexeme),
        filename_, peek().Line);
    auto error_node = std::make_unique<InvalidExpression>();
    error_node->Line =
        consume().Line; // we consume to get rid of the offending character
    is_panic_ = true;
    return error_node;
  }
  case TokenType::IDENTIFIER: {
    const auto &this_tok = consume();
    auto this_node = std::make_unique<VariableEval>(
        this_tok.Lexeme); // store the variable name
    this_node->Line = this_tok.Line;
    return this_node;
  }
  case TokenType::STRING_LITERAL: {
    const auto &this_tok = consume();
    auto this_node = std::make_unique<Literal>(this_tok.Value);
    this_node->Line = this_tok.Line;
    return this_node;
  }
  case TokenType::FLOAT_LITERAL: {
    const auto &this_tok = consume();
    auto this_node = std::make_unique<Literal>(this_tok.Value);
    this_node->Line = this_tok.Line;
    return this_node;
  }
  case TokenType::TRUE: {
    auto line = consume().Line;
    auto this_node = std::make_unique<Literal>(true);
    this_node->Line = line;
    return this_node;
  }
  case TokenType::FALSE: {
    auto line = consume().Line;
    auto this_node = std::make_unique<Literal>(false);
    this_node->Line = line;
    return this_node;
  }
  case TokenType::NIL: {
    auto line = consume().Line;
    auto this_node = std::make_unique<Literal>(Nil{});
    this_node->Line = line;
    return this_node;
  }
  case TokenType::L_PAREN: // handle grouping
    auto line = consume().Line;
    auto expr = parseExpression();
    // expect a right parenthesis
    if (expect(TokenType::R_PAREN, "Invalid Grouping expression!")) {
      consume(); // get rid of the right parenthesis
      auto this_node = std::make_unique<Grouping>(std::move(expr));
      this_node->Line = line;
      return this_node;
    }
    // so... if you forgot your parenthesis
    consume(); // get rid of offending token.
    auto error_node = std::make_unique<InvalidExpression>();
    error_node->Line = line;
    is_panic_ = true;
    return error_node;
  }
}

auto Parser::parsePrint() -> StatementPtr {
  auto line = consume().Line; // PRINT Token
  auto expr = parseExpression();
  if (expect(TokenType::SEMICOLON,
             "[rookie mistake] Expected ';' after statement")) {
    consume(); // the ;
    auto print_node = std::make_unique<PrintStatement>(std::move(expr));
    print_node->Line = line;
    return print_node;
  }
  // bro forgot the semicolon
  return errorStatement(consume());
}

// TODO: handle function calls
auto Parser::parseIdentifier() -> StatementPtr {
  auto identifier = consume();
  auto next = peek();
  if (next.Type == TokenType::ASSIGNMENT) {
    return parseAssignment(identifier);
  }
  return parseVarDecl(identifier);
}

auto Parser::parseVarDecl(const Token &identifier) -> StatementPtr {
  auto line = identifier.Line;
  auto name = identifier.Lexeme;
  // syntax: a: Int -> 5;
  if (!expect(TokenType::COLON, "Expected ':' after identifier.")) {
    return errorStatement(consume()); // get rid of offending (i know)
  }
  consume(); // remove :
  bool found_valid_id = false;
  if (!::builtin_types.contains(peek().Type)) {
    std::cerr << "Expected a valid type after variable name.\n";
    return errorStatement(consume());
  }
  auto type = consume().Lexeme;
  if (!expect(TokenType::ASSIGNMENT,
              "Expected -> after variable declaration.")) {
    return errorStatement(consume());
  }
  consume(); // remove ->
  auto value = parseExpression();
  if (!expect(TokenType::SEMICOLON, "Expected ; after statement")) {
    return errorStatement(consume());
  }
  consume(); // get rid of ;
  auto var_decl_stmt = std::make_unique<GlobalDeclaration>();
  var_decl_stmt->Type = type;
  var_decl_stmt->Name = name;
  var_decl_stmt->AssignedValue = std::move(value);
  return var_decl_stmt;
}

auto Parser::parseAssignment(const Token &identifier) -> StatementPtr {
  if (!expect(TokenType::ASSIGNMENT, "Expected -> after identifier.")) {
    return errorStatement(consume());
  }
  consume(); // ->
  auto assigned_value = parseExpression();
  if (!expect(TokenType::SEMICOLON, "Expected ; after identifier")) {
    return errorStatement(consume());
  }
  consume(); // ->
  auto stmt = std::make_unique<Assignment>();
  stmt->Line = identifier.Line;
  stmt->Name = identifier.Lexeme;
  stmt->AssignmentValue = std::move(assigned_value);
  return stmt;
}

auto Parser::errorStatement(const Token &token) -> StatementPtr {
  auto invalid_stmt = std::make_unique<InvalidStatement>();
  invalid_stmt->Line = token.Line;
  return invalid_stmt;
}

auto Parser::parseBlock() -> StatementPtr {
  ++current_scope_depth_;
  auto block = std::make_unique<BlockScope>();
  block->Line = consume().Line;
  // increase depth of scope
  block->ScopeDepth = current_scope_depth_;
  while (peek().Type != TokenType::END_OF_FILE &&
         peek().Type != TokenType::R_BRACE) {
    std::cout << "UWU" << std::endl;
    block->Statements.emplace_back(std::move(parseStatement()));
    if (is_panic_) {
      return errorStatement(peek(-1));
    }
  }
  --current_scope_depth_; // decrease the depth of scope
  if (!expect(TokenType::R_BRACE, "Expected } to close block!")) {
    return errorStatement(consume());
  }
  std::cout << toString(consume().Type) << std::endl;
  std::cout << toString(peek().Type) << std::endl;
  ; // the }
  return block;
}
