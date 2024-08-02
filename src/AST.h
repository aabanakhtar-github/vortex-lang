#ifndef AST_H
#define AST_H

#include "Token.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

// Operator Precedence (I think) From most important to least, based on C++
// Primary Expressions
// Function Call / Array subscriptsdadf
// Unary operations
// Multiplication / Division
// Addition / Subtraction
// Relational Comparisions (<= , >=, >, <)
// Equality Comparisions (==, !=)
// Logical AND
// Logical OR
// Assignment
// Expression*
// * expressions are just goofy

struct Expression {
  // base for all kinds of expressions
};

using ExpressionPtr = std::unique_ptr<Expression>;

// Node for +, -, !=, etc.
struct BinaryOperation : Expression {
  TokenType Operator;
  ExpressionPtr Left;
  ExpressionPtr Right;

  BinaryOperation(TokenType op, ExpressionPtr &lhs, ExpressionPtr &&rhs)
      : Operator{op}, Left{std::move(lhs)}, Right{std::move(rhs)} {}
};

// Node for -x, !x, etc.
struct UnaryOperation : Expression {
  TokenType Operator;
  ExpressionPtr Right;

  UnaryOperation(TokenType op, ExpressionPtr &&rhs)
      : Operator{op}, Right{std::move(rhs)} {}
};

// Node for (x)
struct Grouping : Expression {
  ExpressionPtr Expr;

  Grouping(ExpressionPtr &&expr) : Expr{std::move(expr)} {}
};

// Node for raw bools, doubles, and strings.
struct Literal : Expression {
  LiteralVariant Value;

  Literal(const LiteralVariant &value) : Value{value} {}
};

#endif // !AST_H
