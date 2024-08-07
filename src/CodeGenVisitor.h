#include "AST.h"

// Bytecode generator!
class CodeGenVisitor : public NodeVisitor {
public:
  explicit CodeGenVisitor(class Program &program);
  auto visit(Expression *node) -> void override;
  auto visit(BinaryOperation *node) -> void override;
  auto visit(UnaryOperation *node) -> void override;
  auto visit(Grouping *node) -> void override;
  auto visit(Literal *node) -> void override;
  auto visit(InvalidExpression *node) -> void override;
  auto wrapUp() -> void; // just adds a halt to the end, must call.
private:
  Program &program_;
};
