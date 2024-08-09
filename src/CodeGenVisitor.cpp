#include "CodeGenVisitor.h"
#include "Error.h"
#include "Program.h"
#include "Token.h"
#include "VortexTypes.h"

// TODO: add line numbers

CodeGenVisitor::CodeGenVisitor(Program &program) : program_{program} {}

auto CodeGenVisitor::visit(Expression *node) -> void {}

auto CodeGenVisitor::visit(BinaryOperation *node) -> void {
  node->Left->acceptVisitor(
      this); // generate the code for the right side and push it on
  node->Right->acceptVisitor(this);
  switch (node->Operator) {
  case TokenType::PLUS:
    program_.pushCode(ADD, node->Line);
    break;
  case TokenType::MINUS:
    program_.pushCode(SUB, node->Line);
    break;
  case TokenType::MUL:
    program_.pushCode(MUL, node->Line);
    break;
  case TokenType::DIV:
    program_.pushCode(DIV, node->Line);
    break;
  case TokenType::NOT:
    program_.pushCode(NEGATE, node->Line);
    break;
  default:
    // TODO: fill in filename for error
    reportError("Parser generated unexpected op for binary node.", "",
                node->Line);
    break;
  }
}
auto CodeGenVisitor::visit(UnaryOperation *node) -> void {
  node->Right->acceptVisitor(this);
  switch (node->Operator) {
  case TokenType::MINUS:
    program_.pushCode(NEGATE, node->Line);
    break;
  default:
    reportError("Parser generated unexpected op for unary node.", "",
                node->Line);
    break;
  }
}

auto CodeGenVisitor::visit(Grouping *node) -> void {
  node->Expr->acceptVisitor(this);
}

auto CodeGenVisitor::visit(Literal *node) -> void {
  switch (LiteralVariantIndex{node->Value.index()}) {
  default:
    reportError("Literal variant of unknown type!", "", node->Line);
    break;
  case LiteralVariantIndex::DOUBLE:
    VortexValue val = {.Type = ValueType::DOUBLE,
                       .Value = {std::get<double>(node->Value)}};
    auto index = program_.addConstant(val);
    // ran out of space for all the constants
    // TODO: use 24 bit numbers
    if (index == -1) {
      reportError("Could not allocate constant! Use more variables!");
    }
    program_.pushCode(PUSHC, node->Line);
    program_.pushCode(index, node->Line);
    break;
  }
}

auto CodeGenVisitor::visit(InvalidExpression *node) -> void {}

auto CodeGenVisitor::wrapUp() -> void { program_.pushCode(HALT, 0); }
