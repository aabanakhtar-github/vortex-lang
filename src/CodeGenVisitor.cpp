#include "CodeGenVisitor.h"
#include "Program.h"

CodeGenVisitor::CodeGenVisitor(Program &program) : program_{program} {}

auto CodeGenVisitor::visit(Expression *node) -> void {}

auto CodeGenVisitor::visit(BinaryOperation *node) -> void {}

auto CodeGenVisitor::visit(UnaryOperation *node) -> void {}

auto CodeGenVisitor::visit(Grouping *node) -> void {}

auto CodeGenVisitor::visit(Literal *node) -> void {}

auto CodeGenVisitor::visit(InvalidExpression *node) -> void {}

auto CodeGenVisitor::wrapUp() -> void { program_.pushCode(HALT, 0); }
