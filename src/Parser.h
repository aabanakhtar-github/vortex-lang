#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Token.h"
#include <memory>

// AST generation device
class Parser {
  using ExpressionPtr = std::unique_ptr<Expression>;
  Parser(std::vector<Token> &tokens_);

  auto parse() -> Program *;

private:
  // NOTE: precedence is from least to highest

  auto parseExpression() -> ExpressionPtr;
  //  TODO: auto parseAssignment() -> ExpressionPtr;
  //  TODO: auto parseLogical() -> ExpressionPtr {};
  auto parseEquality() -> ExpressionPtr;
  auto parseRelational() -> ExpressionPtr;
  auto parseTerm() -> ExpressionPtr;
  auto parseFactor() -> ExpressionPtr;
  auto parseUnary() -> ExpressionPtr;
  // TODO: auto parseFunctionCall();
  // TODO: auto parseArraySubscript();
  auto parsePrimary() -> ExpressionPtr;

private:
  std::size_t pos_ = 0;
  std::vector<Token> &tokens_;
  std::unique_ptr<Program> result_;
  bool is_panic_;
};

#endif // !PARSER_H
