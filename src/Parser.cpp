#include "Parser.h"
#include "AST.h"
#include "Error.h"
#include "Token.h"
#include <algorithm>
#include <array>
#include <format>
#include <iostream>

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
         peek().Type == TokenType::INEQUALITY) {
    auto op = consume().Type; // get that juicy operator
    // take in the right side of the operation
    this_node =
        std::make_unique<BinaryOperation>(op, this_node, parseEquality());
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
    auto op = consume().Type; // juicy...
    // get the right side
    this_node = std::make_unique<BinaryOperation>(op, this_node, parseTerm());
  }
  return this_node;
}

auto Parser::parseTerm() -> ExpressionPtr {
  auto this_node = parseFactor();
  while (pos_ < tokens_.size() &&
         (peek().Type == TokenType::PLUS || peek().Type == TokenType::MINUS)) {
    auto op = consume().Type;
    this_node = std::make_unique<BinaryOperation>(op, this_node, parseFactor());
  }
  return this_node;
}

auto Parser::parseFactor() -> ExpressionPtr {
  auto this_node = parseUnary();
  while (pos_ < tokens_.size() &&
         (peek().Type == TokenType::MUL || peek().Type == TokenType::DIV)) {
    auto op = consume().Type;
    this_node = std::make_unique<BinaryOperation>(op, this_node, parseUnary());
  }
  return this_node;
}

auto Parser::parseUnary() -> ExpressionPtr {
  if (peek().Type == TokenType::MINUS || peek().Type == TokenType::NOT) {
    auto op = consume().Type;
    return std::make_unique<UnaryOperation>(op, parsePrimary());
  }
  return parsePrimary();
}

auto Parser::parsePrimary() -> ExpressionPtr {
  switch (peek().Type) {
  default:
    // reason for consume: get rid of this silliness, so we don't deal with it
    // later
    reportError(
        std::format("Expected a valid expression, got '{}'!", peek().Lexeme),
        filename_, consume().Line);
    return std::make_unique<InvalidExpression>();
  case TokenType::STRING_LITERAL: {
    const auto &tok = consume();
    return std::make_unique<Literal>(tok.Value);
  }
  case TokenType::FLOAT_LITERAL: {
    const auto &tok = consume();
    return std::make_unique<Literal>(tok.Value);
  }
  case TokenType::TRUE: {
    consume();
    return std::make_unique<Literal>(true);
  }
  case TokenType::FALSE: {
    consume();
    return std::make_unique<Literal>(false);
  }
  case TokenType::NIL: {
    consume();
    return std::make_unique<Literal>(None{});
  }
  case TokenType::L_PAREN: // handle grouping
    consume();
    auto expr = parseExpression();
    if (expect(TokenType::R_PAREN, "Invalid Grouping expression!")) {
      consume();
    }
    return std::make_unique<Grouping>(std::move(expr));
  }
}
