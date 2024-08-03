#ifndef AST_EXPRESSION_PRINTER_H
#define AST_EXPRESSION_PRINTER_H

#include "AST.h"
#include "Token.h"
#include <iostream>

class PrettyPrintExpressionVisitor : public NodeVisitor {
public:
  PrettyPrintExpressionVisitor() = default;
  auto visit(Expression *node) -> void override { std::cout << "why?"; }

  auto visit(BinaryOperation *node) -> void override {
    std::cout << "(";
    node->Left->acceptVisitor(this);
    std::cout << " " << toString(node->Operator) << " ";
    node->Right->acceptVisitor(this);
    std::cout << ")";
  }

  auto visit(UnaryOperation *node) -> void override {
    std::cout << "(" << toString(node->Operator) << " ";
    node->Right->acceptVisitor(this);
    std::cout << ")";
  }

  auto visit(Grouping *node) -> void override {
    std::cout << "(";
    node->Expr->acceptVisitor(this);
    std::cout << ")";
  }

  auto visit(Literal *node) -> void override {
    switch (LiteralVariantIndex{node->Value.index()}) {
    case LiteralVariantIndex::NONE:
      std::cout << "NIL";
      break;
    case LiteralVariantIndex::BOOL:
      std::cout << ((std::get<bool>(node->Value)) ? "true" : "false");
      break;
    case LiteralVariantIndex::STRING:
      std::cout << std::get<std::string>(node->Value);
      break;
    case LiteralVariantIndex::DOUBLE:
      std::cout << std::get<double>(node->Value);
      break;
    }
  }

  auto visit(InvalidExpression *node) -> void override {
    std::cout << "(INVALID)";
  }
};

#endif // !AST_EXPRESSION_PRINTER_H
