#include "Parser.h"
#include "AST.h"
#include "Error.h"
#include "Token.h"
#include <algorithm>
#include <array>
#include <format>
#include <memory>

Parser::Parser(std::string_view filename, const std::vector<Token> &tokens)
    : filename_{filename}, tokens_{tokens} {}

auto Parser::parse() -> std::vector<ExpressionPtr> & {
  while (pos_ < tokens_.size() - 1) {
    result_.push_back(parseExpression());
  }
  return result_;
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

auto Parser::peek() -> Token {
  if (pos_ < tokens_.size()) {
    return tokens_[pos_];
  }
  return Token{.Type = TokenType::END_OF_FILE};
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
    return error_node;
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
    consume(); // get rid of offending token.
    auto error_node = std::make_unique<InvalidExpression>();
    error_node->Line = line;
    return error_node;
  }
}
