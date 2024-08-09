#ifndef AST_EXPRESSION_PRINTER_H
#define AST_EXPRESSION_PRINTER_H

#include "AST.h"

class PrettyPrintExpressionVisitor : public NodeVisitor {
public:
  PrettyPrintExpressionVisitor() = default;
  auto visit(Expression *node) -> void override;

  auto visit(BinaryOperation *node) -> void override;
  auto visit(UnaryOperation *node) -> void override;
  auto visit(Grouping *node) -> void override;
  auto visit(Literal *node) -> void override;

  auto visit(InvalidExpression *node) -> void override;
};

#endif // !AST_EXPRESSION_PRINTER_H
