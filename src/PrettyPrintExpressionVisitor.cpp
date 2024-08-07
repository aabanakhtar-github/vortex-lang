#include "PrettyPrintExpressionVisitor.h"
#include <iostream>

auto PrettyPrintExpressionVisitor::visit(Expression *node) -> void {
  std::cout << "unknown expression node";
}

auto PrettyPrintExpressionVisitor::visit(BinaryOperation *node) -> void {
  std::cout << "(";
  node->Left->acceptVisitor(this);
  std::cout << " " << toString(node->Operator) << " ";
  node->Right->acceptVisitor(this);
  std::cout << ")";
}

auto PrettyPrintExpressionVisitor::visit(UnaryOperation *node) -> void {
  std::cout << "(" << toString(node->Operator) << " ";
  node->Right->acceptVisitor(this);
  std::cout << ")";
}

auto PrettyPrintExpressionVisitor::visit(Grouping *node) -> void {
  std::cout << "(";
  node->Expr->acceptVisitor(this);
  std::cout << ")";
}

auto PrettyPrintExpressionVisitor::visit(Literal *node) -> void {
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

auto PrettyPrintExpressionVisitor::visit(InvalidExpression *node) -> void {
  std::cout << "(INVALID)";
}
