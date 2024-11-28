#ifndef AST_H
#define AST_H

#include "Token.h"
#include <memory>
#include <optional>

// TODO: figure out the consturcotrs mixing ExpressionPtr& and ExpressionPtr&&
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
  virtual auto visit(class VariableEval *node) -> void = 0;
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

struct VariableEval : Expression {
  std::string Name;

  VariableEval(const std::string &name) : Name{name} {}

  virtual auto acceptVisitor(class NodeVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

struct InvalidExpression : Expression {
  virtual auto acceptVisitor(class NodeVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

class StatementVisitor {
public:
  virtual auto visit(class Statement *statement) -> void = 0;
  virtual auto visit(class InvalidStatement *statement) -> void = 0;
  virtual auto visit(class PrintStatement *statement) -> void = 0;
  virtual auto visit(class VariableDeclaration *statement) -> void = 0;
  virtual auto visit(class Assignment *assignment) -> void = 0;
  virtual auto visit(class BlockScope *block_scope) -> void = 0;
  virtual auto visit(class IfStatement *if_statement) -> void = 0;
};

// *STATEMENTS ARE INDIVIDUAL UNITS OF EXECUTION*
struct Statement {
  std::size_t Line;

  virtual ~Statement() {}
  virtual auto acceptVisitor(class StatementVisitor *visitor) -> void = 0;
};
using StatementPtr = std::unique_ptr<Statement>;

// representing statements that *SOMEONE* messed up
struct InvalidStatement : Statement {
  virtual auto acceptVisitor(class StatementVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

struct PrintStatement : Statement {
  ExpressionPtr Expr;

  PrintStatement(ExpressionPtr &&expr) : Expr(std::move(expr)) {}
  virtual auto acceptVisitor(class StatementVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

struct VariableDeclaration : Statement {
  std::string Type;
  std::string Name;
  ExpressionPtr AssignedValue;
  virtual auto acceptVisitor(class StatementVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

struct Assignment : Statement {
  std::string Name;
  ExpressionPtr AssignmentValue;
  virtual auto acceptVisitor(class StatementVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

struct BlockScope : Statement {
  std::vector<std::unique_ptr<Statement>> Statements;
  std::size_t ScopeDepth = 0;
  virtual auto acceptVisitor(class StatementVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

struct IfStatement : Statement {
  StatementPtr IfBody;
  std::optional<StatementPtr> ElseBody;
  ExpressionPtr Condition;
  virtual auto acceptVisitor(class StatementVisitor *visitor) -> void {
    visitor->visit(this);
  }
};

struct ProgramNode {
  std::vector<StatementPtr> Statements;
};

#endif // !AST_H
