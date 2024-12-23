#include "CodeGenVisitor.h"
#include "AST.h"
#include "Error.h"
#include "Program.h"
#include "Token.h"
#include "Util.h"
#include "VortexTypes.h"
#include <chrono>
#include <format>
#include <iostream>
#include <iterator>

CodeGen::CodeGen(Program &program) : program_{program} {}

auto CodeGen::visit(Expression *node) -> void {}

auto CodeGen::visit(BinaryOperation *node) -> void {
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

auto CodeGen::visit(UnaryOperation *node) -> void {
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

auto CodeGen::visit(VariableEval *node) -> void {
  // if we are in a scope and we find the variable name as a local
  auto local_iter = std::find_if(
      local_table_.begin(), local_table_.end(),
      [&](const Local &local) -> bool { return local.Name == node->Name; });

  if (current_scope_depth_ != 0 && local_iter != local_table_.end()) {
    // we need to do the magic of getting a local now
    auto local_offset = std::distance(
        local_table_.begin(), local_iter); // the offset from the stack base
    auto local_offset_index = program_.addConstant(VortexValue{
        .Type = ValueType::DOUBLE,
        .Value = {.AsDouble = static_cast<double>(
                      local_offset)}}); // the index as a vortex value so we can
                                        // push it onto the stack :(
    auto local_offset_indicies = sizeToTriByte(local_offset_index);
    program_.pushCode(PUSHC, node->Line);
    program_.pushCode(std::get<0>(local_offset_indicies), node->Line);
    program_.pushCode(std::get<1>(local_offset_indicies), node->Line);
    program_.pushCode(std::get<2>(local_offset_indicies), node->Line);
    // get the local based on it's stack offset
    program_.pushCode(GET_LOCAL, node->Line);
    return;
  } else if (!program_.globalExists(
                 node->Name)) { // it must be global or crash!
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

auto CodeGen::visit(Grouping *node) -> void { node->Expr->acceptVisitor(this); }

auto CodeGen::visit(Literal *node) -> void {
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

auto CodeGen::visit(InvalidExpression *node) -> void {}

auto CodeGen::visit(Statement *statement) -> void {}

auto CodeGen::visit(InvalidStatement *statement) -> void {}

auto CodeGen::visit(VariableDeclaration *statement) -> void {
  statement->AssignedValue->acceptVisitor(this); // handle the value
  if (current_scope_depth_ != 0) {               // if its local
    if (std::find_if(local_table_.begin(), local_table_.end(),
                     [&](const Local &local) -> bool {
                       return local.Name == statement->Name;
                     }) != local_table_.end() ||
        program_.globalExists(statement->Name)) {
      // we already have this variable in a global or local scope
      reportError("Cannot have duplicate variable!", "TODO: filename",
                  statement->Line);
      return;
    }
    // create a local variable from the evaluated expression on the stack
    program_.pushCode(ADD_LOCAL, statement->Line);
    local_table_.push_back(Local{
        .Depth = current_scope_depth_,
        .Name = statement->Name,
    });
    return;
  }
  // otherwise its a global
  auto global = program_.createGlobal(statement->Name, {});
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

auto CodeGen::visit(PrintStatement *statement) -> void {
  statement->Expr->acceptVisitor(this);
  program_.pushCode(PRINT, statement->Line);
}

auto CodeGen::visit(Assignment *statement) -> void {
  statement->AssignmentValue->acceptVisitor(
      this); // push the assigned value onto the stack
  // check if its a local
  if (current_scope_depth_ != 0) {
    auto it = std::find_if(local_table_.begin(), local_table_.end(),
                           [&](const Local &local) -> bool {
                             return local.Name == statement->Name;
                           });
    if (!(it == local_table_.end())) {
      auto local_offset = std::distance(local_table_.begin(), it);
      auto local_offset_index = program_.addConstant(VortexValue{
          .Type = ValueType::DOUBLE,
          .Value = {.AsDouble = static_cast<double>(
                        local_offset)}}); // as an index in the constants table
      auto local_offset_bytes = sizeToTriByte(local_offset_index);
      program_.pushCode(PUSHC, statement->Line);
      program_.pushCode(std::get<0>(local_offset_bytes), statement->Line);
      program_.pushCode(std::get<1>(local_offset_bytes), statement->Line);
      program_.pushCode(std::get<2>(local_offset_bytes), statement->Line);
      program_.pushCode(SET_LOCAL, statement->Line);
      return;
    }
  }
  // otherwise its global
  if (!program_.globalExists(statement->Name)) { // if we didnt find it
    reportError(std::format("Cannot find variable {}!", statement->Name),
                "TODO: filename", statement->Line);
    return;
  }
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

auto CodeGen::visit(BlockScope *statement) -> void {
  ++current_scope_depth_;
  for (auto &statement : statement->Statements) {
    statement->acceptVisitor(this);
  }
  // clean up local variables
  for (auto &local : local_table_) {
    if (local.Depth == current_scope_depth_) {
      program_.pushCode(POP_LOCAL,
                        (statement->Statements.end() - 1)->get()->Line);
      local_table_.erase(local_table_.begin() + local_table_.size() - 1);
    }
  }
  --current_scope_depth_;
}

auto CodeGen::visit(IfStatement *node) -> void {
  node->Condition->acceptVisitor(this);
  auto tribyte = sizeToTriByte(0);
  program_.pushCode(PUSHC, node->Line);
  auto b1 = program_.pushCode(0, node->Line);
  auto b2 = program_.pushCode(0, node->Line);
  auto b3 = program_.pushCode(0, node->Line); // these bytes will be filled in
                                              // after calculating the offsets
  program_.pushCode(JMP_TO_IF_FALSE,
                    node->Line); // moves to the else or regular code execution
  // determine the sizes beforehand so we can write correct jump code
  auto initial_program_size = program_.Bytecode.size();
  node->IfBody->acceptVisitor(this);
  auto if_code_size = program_.Bytecode.size() - initial_program_size;
  std::size_t eb1, eb2, eb3;
  if (node->ElseBody.has_value()) {
    program_.pushCode(PUSHC, node->Line);
    eb1 = program_.pushCode(0, node->Line);
    eb2 = program_.pushCode(0, node->Line);
    eb3 = program_.pushCode(
        0, node->Line); // these bytes will be filled in
                        // after calculating the offsets
                        //  these are  the bytes for skipping the else
    program_.pushCode(JMP_TO, node->Line);
    node->ElseBody->get()->acceptVisitor(this);
    std::cout << "GOOO";
  }
  auto else_code_size =
      program_.Bytecode.size() - if_code_size - initial_program_size;
  std::cout << if_code_size << std::endl;
  std::cout << else_code_size << std::endl;
  // the offsets are now calculated  (zero indexed)
  auto offset_else_or_false = initial_program_size + if_code_size +
                              (node->ElseBody.has_value() ? 5 : 0);
  auto offset_skip_else = initial_program_size + if_code_size + else_code_size;
  std::cout << "! " << offset_else_or_false << std::endl;
  std::cout << "! " << offset_skip_else << std::endl;
  auto offset_else_or_false_idx = program_.addConstant(VortexValue{
      .Type = ValueType::DOUBLE,
      .Value = {.AsDouble = static_cast<double>(offset_else_or_false)}});
  auto offset_skip_else_idx = program_.addConstant(VortexValue{
      .Type = ValueType::DOUBLE,
      .Value = {.AsDouble = static_cast<double>(offset_skip_else)}});
  // ITS AN ACRONYM
  auto oefi_tribyte = sizeToTriByte(offset_else_or_false_idx);
  auto osei_tribyte = sizeToTriByte(offset_skip_else_idx);
  auto &bytes = program_.Bytecode;
  // update the bytes with the offsets
  bytes[b1] = std::get<0>(oefi_tribyte);
  bytes[b2] = std::get<1>(oefi_tribyte);
  bytes[b3] = std::get<2>(oefi_tribyte);
  if (node->ElseBody.has_value()) {
    bytes[eb1] = std::get<0>(osei_tribyte);
    bytes[eb2] = std::get<1>(osei_tribyte);
    bytes[eb3] = std::get<2>(osei_tribyte);
  }
}
