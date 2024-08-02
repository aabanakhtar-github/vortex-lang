#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Token.h"

// AST generation device
class Parser {
public:
  explicit Parser(std::string_view filename, const std::vector<Token> &tokens);

  auto parse() -> Expression *;

private:
  auto consume() -> Token;
  auto peek() -> Token;
  auto expect(TokenType type, std::string_view error) -> bool;

  // NOTE: precedence is from least to highest
  auto parseExpression() -> ExpressionPtr;
  //  TODO: auto parseAssignment() -> ExpressionPtr;
  //  TODO: auto parseLogical() -> ExpressionPtr {};
  auto parseEquality() -> ExpressionPtr;
  auto parseRelational() -> ExpressionPtr; // note: <=
  auto parseTerm() -> ExpressionPtr;
  auto parseFactor() -> ExpressionPtr;
  auto parseUnary() -> ExpressionPtr;
  // TODO: auto parseFunctionCall();
  // TODO: auto parseArraySubscript();
  auto parsePrimary() -> ExpressionPtr;

private:
  std::size_t pos_ = 0;
  std::string filename_;
  const std::vector<Token> &tokens_;
  std::unique_ptr<Expression> result_;
  bool is_panic_;
};

#endif // !PARSER_H
