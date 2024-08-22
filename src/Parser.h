#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Token.h"

// AST generation device
class Parser {
public:
  explicit Parser(std::string_view filename, const std::vector<Token> &tokens);

  auto parse() -> Program &;

private:
  auto consume() -> Token;
  auto peek() -> Token;
  auto expect(TokenType type, std::string_view error) -> bool;
  auto handlePanic() -> void;
  // NOTE: precedence is from least to highest
  auto parseExpression() -> ExpressionPtr;
  auto parseStatement() -> StatementPtr;

  // Rules for expressions
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

  // STATEMENT PARSING
  auto parsePrint() -> StatementPtr;
  auto parseGlobalDecl() -> StatementPtr;

private:
  std::size_t pos_ = 0;
  bool is_panic_ = false;
  std::string filename_;
  const std::vector<Token> &tokens_;
  Program result_;
};

#endif // !PARSER_H
