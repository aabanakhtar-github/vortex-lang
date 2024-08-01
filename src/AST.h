#ifndef AST_H
#define AST_H

#include "Token.h"
#include <array>
#include <string>
#include <vector>

// Operator Precedence (I think) From most important to least, based on C++
// Primary Expressions
// Function Call / Array subscript
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

// Node for assigning variables values
struct Assignment : Expression {
  std::string Variable;
  Expression *value;
};

// struct SimpleMethodCall ?
// Node for calling member functions
struct MethodCall : Expression {
  Expression *Caller;
  std::string MethodName;
  std::vector<Expression *> Parameters;
};

// Node for If/Else
struct IfElse : Expression {
  Expression *Condition;
  Expression *TrueBody;  // If true
  Expression *FalseBody; // If false
};

// Node for {expr1;expr2;} blocks
struct BlockOfExpressions : Expression {
  std::vector<Expression *> ExpressionsList;
};

// struct LetBinding {
//  name : str;
//  type : str;
//  initializer: Optional[Expr]
// }
//
// struct LetExpression {
//  bindings: List[LetBinding];
//  body: Expr;
// }

// Node for declaring variables in a function
struct LocalVariable : Expression {
  std::string Type;
  std::string Name;
  Expression *Initializer;
};

// Node for while loops
struct WhileLoop : Expression {
  Expression *Condition;
  Expression *Body;
};

// Node for for loop
struct ForLoop : Expression {
  // Chose expression because the statement can be empty
  // i.e for ;;; {}
  std::array<Expression *, 3> Segments; // the i=0, i<x; i->i+1, etc.
  Expression *Body;
};

// Node For making new Objects
struct Create : Expression {
  std::string Type;
};

// Node for +, -, !=, etc.
struct BinaryOperation : Expression {
  TokenType Operator;
  Expression *Left;
  Expression *Right;
};

// Node for -x, !x, etc.
struct UnaryOperation : Expression {
  TokenType Operator;
  Expression *Left;
  Expression *Right;
};

// Node for (x)
struct Grouping : Expression {
  Expression *Expr;
};

// Node for names
struct Identifier : Expression {
  std::string Name;
};

// Node for Array operations
struct ArrayIndex : Expression {
  std::string Name;
  std::size_t Index;
};

// Node for raw bools, doubles, and strings.
struct Literal : Expression {
  LiteralVariant Value;
};

// Node for method declarations
struct Method {
  // Holds the type info of the parameters, and not the actual parameter
  struct ParameterMeta {
    std::string Type;
    Token Value;
  };
  std::string Name;
  std::string ReturnType;
  bool IsConstructor;
  std::vector<ParameterMeta> Params;
  Expression *body;
};

// Node for member variables
struct Attribute {
  std::string Type;
  std::string Name;
  Token Value;
};

// Node for classes
struct Class {
  std::string Name;
  std::string SuperClass;
  std::vector<Method> Methods;
  std::vector<Attribute> Attributes;
};

// Node for Program
struct Program {
  std::vector<Class> Classes;
};

#endif // !AST_H
