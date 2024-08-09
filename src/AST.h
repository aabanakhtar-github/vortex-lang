#ifndef AST_H
#define AST_H

#include "Token.h"
#include <memory>

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

class NodeVisitor {
public:
  virtual auto visit(class Expression *node) -> void = 0;
  virtual auto visit(class BinaryOperation *node) -> void = 0;
  virtual auto visit(class UnaryOperation *node) -> void = 0;
  virtual auto visit(class Grouping *node) -> void = 0;
  virtual auto visit(class Literal *node) -> void = 0;
  virtual auto visit(class InvalidExpression *node) -> void = 0;
};

struct ASTNode {};

struct Expression {
  std::size_t Line;
  // base for all kinds of expressions
  virtual auto acceptVisitor(class NodeVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

using ExpressionPtr = std::unique_ptr<Expression>;

// Node for +, -, !=, etc.
struct BinaryOperation : Expression {
  TokenType Operator;
  ExpressionPtr Left;
  ExpressionPtr Right;

  BinaryOperation(TokenType op, ExpressionPtr &lhs, ExpressionPtr &&rhs)
      : Operator{op}, Left{std::move(lhs)}, Right{std::move(rhs)} {}

  virtual auto acceptVisitor(class NodeVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

// Node for -x, !x, etc.
struct UnaryOperation : Expression {
  TokenType Operator;
  ExpressionPtr Right;

  UnaryOperation(TokenType op, ExpressionPtr &&rhs)
      : Operator{op}, Right{std::move(rhs)} {}

  virtual auto acceptVisitor(class NodeVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

// Node for (x)
struct Grouping : Expression {
  ExpressionPtr Expr;

  Grouping(ExpressionPtr &&expr) : Expr{std::move(expr)} {}

  auto acceptVisitor(NodeVisitor *visitor) -> void override {
    visitor->visit(this);
  }
};

// Node for raw bools, doubles, and strings.
struct Literal : Expression {
  LiteralVariant Value;

  Literal(const LiteralVariant &value) : Value{value} {}

  virtual auto acceptVisitor(class NodeVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

struct InvalidExpression : Expression {
  virtual auto acceptVisitor(class NodeVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

#endif // !AST_H
