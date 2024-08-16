#include "CodeGenVisitor.h"
#include "Error.h"
#include "Program.h"
#include "Token.h"
#include "VortexTypes.h"
#include <tuple>

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
  case TokenType::EQUALITY:
    program_.pushCode(EQ, node->Line);
    break;
  case TokenType::LESS_THAN_OR_EQUAL:
    program_.pushCode(LESS_EQ, node->Line);
    break;
  case TokenType::GREATER_THAN_OR_EQUAL:
    program_.pushCode(GREATER_EQ, node->Line);
    break;
  case TokenType::LESS_THAN:
    program_.pushCode(LESS, node->Line);
    break;
  case TokenType::GREATER_THAN:
    program_.pushCode(GREATER, node->Line);
    break;
  default:
    // TODO: fill in filename for error and push an invalid operator to crash
    // the interpeter
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
  case TokenType::NOT:
    program_.pushCode(NOT, node->Line);
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
  // returns a tuple of 3 bytes that contain the individual indices from 0, 1,
  // and then finally 2
  auto size_to_24_bit = [](std::size_t i)
      -> std::tuple<std::uint8_t, std::uint8_t, std::uint8_t> {
    auto b1 = static_cast<std::uint8_t>(i >> 16);
    auto b2 = static_cast<std::uint8_t>(i >> 8);
    auto b3 = static_cast<std::uint8_t>(i);
    return {b1, b2, b3};
  };

  switch (LiteralVariantType{node->Value.index()}) {
  case LiteralVariantType::DOUBLE: {
    VortexValue val = {.Type = ValueType::DOUBLE,
                       .Value = {std::get<double>(node->Value)}};
    auto index = program_.addConstant(val);
    // ran out of space for all the constants
    // TODO: use 24 bit numbers
    if (index == -1) {
      reportError("Could not enough space for all program constants. Program "
                  "is too large.");
    }
    program_.pushCode(PUSHC, node->Line);
    // 3 byte index operand
    auto indices = size_to_24_bit(index);
    program_.pushCode(std::get<0>(indices), node->Line);
    program_.pushCode(std::get<1>(indices), node->Line);
    program_.pushCode(std::get<2>(indices), node->Line);
    break;
  }
  case LiteralVariantType::NIL: {
    program_.pushCode(PUSH_NIL, node->Line);
    break;
  }
  case LiteralVariantType::BOOL:
    switch (static_cast<int>(std::get<bool>(node->Value))) {
    case 1:
      program_.pushCode(PUSH_TRUE, node->Line);
      break;
    case 0:
      program_.pushCode(PUSH_FALSE, node->Line);
    }
    break;
  case LiteralVariantType::STRING: {
    // get the actual string
    auto literal = std::get<std::string>(node->Value);
    // get the pointer to the string as a generic (Object*)
    auto ptr = program_.createString(literal);
    // save this as a constant
    auto val =
        VortexValue{.Type = ValueType::OBJECT, .Value = {.AsObject = ptr}};
    // add it to the constants table
    auto index = program_.addConstant(val);
    // convert the index to 24 bit
    auto indicies = size_to_24_bit(index);
    program_.pushCode(PUSHC, node->Line); // load
    program_.pushCode(std::get<0>(indicies), node->Line);
    program_.pushCode(std::get<1>(indicies), node->Line);
    program_.pushCode(std::get<2>(indicies), node->Line);
    break;
  }
  }
}

auto CodeGenVisitor::visit(InvalidExpression *node) -> void {}

auto CodeGenVisitor::wrapUp() -> void { program_.pushCode(HALT, 0); }
