#include "CodeGenVisitor.h"
#include "AST.h"
#include "Error.h"
#include "Program.h"
#include "Token.h"
#include "Util.h"
#include "VortexTypes.h"
#include <format>

ExprCodeGen::ExprCodeGen(Program &program) : program_{program} {}

auto ExprCodeGen::visit(Expression *node) -> void {}

auto ExprCodeGen::visit(BinaryOperation *node) -> void {
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
auto ExprCodeGen::visit(UnaryOperation *node) -> void {
  node->Right->acceptVisitor(this);
  switch (node->Operator) {
  case TokenType::MINUS:
    program_.pushCode(NEGATE, node->Line);
    break;
  case TokenType::NOT:
    program_.pushCode(NOT, node->Line);
    break;
  default:
    reportError("Parser generated unexpected op for unary node.",
                "no file name yet", node->Line);
    break;
  }
}

auto ExprCodeGen::visit(VariableEval *node) -> void {
  if (!program_.globalExists(node->Name)) {
    reportError(
        std::format("TODO: add filename, Could not find global variable {}!",
                    node->Name),
        "no file name yet", node->Line);
    return;
  }
  auto index =
      program_.getGlobalIndex(node->Name); // get the index in the globals table
  auto index_index =
      program_.addConstant({.Type = ValueType::DOUBLE,
                            .Value{.AsDouble = static_cast<double>(index)}});
  auto indices = sizeToTriByte(
      index_index); // save the value as an index (i hate this lol)
  // load the global's index onto the stack
  program_.pushCode(PUSHC, node->Line);
  program_.pushCode(std::get<0>(indices), node->Line);
  program_.pushCode(std::get<1>(indices), node->Line);
  program_.pushCode(std::get<2>(indices), node->Line);
  // pop it off and push on the value
  program_.pushCode(LOAD_GLOB, node->Line);
}

auto ExprCodeGen::visit(Grouping *node) -> void {
  node->Expr->acceptVisitor(this);
}

auto ExprCodeGen::visit(Literal *node) -> void {
  // returns a tuple of 3 bytes that contain the individual indices from 0, 1,
  // and then finally 2
  switch (LiteralVariantType{node->Value.index()}) {
  case LiteralVariantType::DOUBLE: {
    VortexValue val = {.Type = ValueType::DOUBLE,
                       .Value = {std::get<double>(node->Value)}};
    auto index = program_.addConstant(val);
    // ran out of space for all the constants
    if (index == -1) {
      reportError("Could not enough space for all program constants. Program "
                  "is too large.");
    }
    program_.pushCode(PUSHC, node->Line);
    // 3 byte index operand
    auto indices = sizeToTriByte(index);
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
    case true:
      program_.pushCode(PUSH_TRUE, node->Line);
      break;
    case false:
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
    auto indicies = sizeToTriByte(index);
    program_.pushCode(PUSHC, node->Line); // load
    program_.pushCode(std::get<0>(indicies), node->Line);
    program_.pushCode(std::get<1>(indicies), node->Line);
    program_.pushCode(std::get<2>(indicies), node->Line);
    break;
  }
  }
}

auto ExprCodeGen::visit(InvalidExpression *node) -> void {}

StatementCodeGen::StatementCodeGen(Program &program) : program_{program} {}

auto StatementCodeGen::visit(Statement *statement) -> void {}

auto StatementCodeGen::visit(InvalidStatement *statement) -> void {}

auto StatementCodeGen::visit(VariableDeclaration *statement) -> void {
  auto expr_generator = ExprCodeGen{program_};
  if (current_scope_depth_ != 0) {
    program_.pushCode(ADD_LOCAL, statement->Line);
    return;
  }
  auto global = program_.createGlobal(statement->Name, {});
  statement->AssignedValue->acceptVisitor(&expr_generator);
  auto index = program_.getGlobalIndex(statement->Name);
  // index of the index in the constant table
  auto index_index =
      program_.addConstant({.Type = ValueType::DOUBLE,
                            .Value = {.AsDouble = static_cast<double>(index)}});
  auto index_as_bytes =
      sizeToTriByte(index_index); // so we can load it from the table of consts.
  program_.pushCode(PUSHC, statement->Line);
  program_.pushCode(std::get<0>(index_as_bytes), statement->Line);
  program_.pushCode(std::get<1>(index_as_bytes), statement->Line);
  program_.pushCode(std::get<2>(index_as_bytes), statement->Line);
  program_.pushCode(SAVE_GLOB, statement->Line);
}

auto StatementCodeGen::visit(PrintStatement *statement) -> void {
  auto expr_generator = ExprCodeGen{program_};
  statement->Expr->acceptVisitor(&expr_generator);
  program_.pushCode(PRINT, statement->Line);
}

auto StatementCodeGen::visit(Assignment *statement) -> void {
  auto expr_generator = ExprCodeGen{program_};
  statement->AssignmentValue->acceptVisitor(&expr_generator);
  auto index = program_.getGlobalIndex(statement->Name);
  auto index_index = program_.addConstant(VortexValue{
      .Type = ValueType::DOUBLE,
      .Value = {.AsDouble = static_cast<double>(
                    index)}}); // get the index of the global index in the
                               // global lookup table (what the index)
  auto index_as_bytes = sizeToTriByte(index_index);
  program_.pushCode(PUSHC, statement->Line);
  program_.pushCode(std::get<0>(index_as_bytes), statement->Line);
  program_.pushCode(std::get<1>(index_as_bytes), statement->Line);
  program_.pushCode(std::get<2>(index_as_bytes), statement->Line);
  program_.pushCode(SAVE_GLOB, statement->Line);
}

auto StatementCodeGen::visit(BlockScope *statement) -> void {
  ++current_scope_depth_;
  for (auto &statement : statement->Statements) {
    statement->acceptVisitor(this);
  }
  --current_scope_depth_;
}
