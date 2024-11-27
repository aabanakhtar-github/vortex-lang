#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Token.h"

// AST generation device
class Parser {
public:
  explicit Parser(std::string_view filename, const std::vector<Token> &tokens);

  auto parse() -> ProgramNode &;

private:
  auto consume() -> Token;
  auto peek(std::size_t n = 0) -> Token;
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
  auto parsePrimary() -> ExpressionPtr;

  // STATEMENT PARSING
  auto errorStatement(const Token &token) -> StatementPtr;
  auto parseIdentifier() -> StatementPtr;
  auto parsePrint() -> StatementPtr;
  auto parseVarDecl(const Token &identifier_name) -> StatementPtr;
  auto parseAssignment(const Token &identifier_name) -> StatementPtr;
  auto parseBlock() -> StatementPtr;
  auto parseIfStatement() -> StatementPtr;

private:
  std::size_t pos_ = 0;
  bool is_panic_ = false;
  std::size_t current_scope_depth_ = 0;
  std::string filename_;
  const std::vector<Token> &tokens_;
  ProgramNode result_;
};

#endif // !PARSER_H
