#include "AST.h"
#include "Program.h"

// Bytecode generator!
class ExprCodeGen : public NodeVisitor {
public:
  explicit ExprCodeGen(Program &program);
  auto visit(Expression *node) -> void override;
  auto visit(BinaryOperation *node) -> void override;
  auto visit(UnaryOperation *node) -> void override;
  auto visit(Grouping *node) -> void override;
  auto visit(Literal *node) -> void override;
  auto visit(InvalidExpression *node) -> void override;
  auto visit(VariableEval *node) -> void override;

private:
  Program &program_;
};

class StatementCodeGen : public StatementVisitor {
public:
  explicit StatementCodeGen(Program &program);
  auto visit(Statement *statement) -> void override;
  auto visit(VariableDeclaration *statement) -> void override;
  auto visit(PrintStatement *statement) -> void override;
  auto visit(InvalidStatement *statement) -> void override;
  auto visit(Assignment *statement) -> void override;
  auto visit(BlockScope *statement) -> void override;

private:
  std::size_t current_scope_depth_ = 0;
  Program &program_;
  std::stack<Symbol> symbol_table_;
};

inline auto wrapUp(Program &program) -> void { program.pushCode(HALT, 0); }
