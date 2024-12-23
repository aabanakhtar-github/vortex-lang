#include "AST.h"
#include "CodeGenVisitor.h"
#include "Lexer.h"
#include "Parser.h"
#include "Program.h"
#include "VM.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>

auto main(int argc, char *argv[]) -> int {
  auto program = std::ifstream("main.vrtx", std::ios_base::in);
  auto program_str = std::string{};
  auto s = std::string{};
  while (std::getline(program, s)) {
    program_str += s + '\n';
  }
  auto file = std::ofstream{"lexer_output.txt", std::ios_base::out};
  auto lexer = Lexer{program_str, "main.vrtx", &file};
  lexer.lex();
  auto parser = Parser{"main.vrtx", lexer.getTokens()};
  auto &e{parser.parse()};
  auto p = Program{};
  auto g = CodeGen{p};
  for (auto &stmt : e.Statements) {
    stmt->acceptVisitor(&g);
  }

  wrapUp(p);
  p.dissassemble("main.vbyte");
  auto vm = VM{p};
  std::cout << "Vortex interpreter:\n";
  vm.run();
  program.close();
  /*auto p = Program{};
  auto c = CodeGenVisitor{p};
  e[0]->acceptVisitor(&c);
  c.wrapUp();
  p.dissassemble("compiler_output");
  std::cout << "Check out this illegal access:"
            << int(p.Bytecode[p.Bytecode.size()]) << std::endl;
  ;
  auto vm = VM{p};
  std::cout << "Process vortex interpreter:\n";
  vm.run(); */
  return 0;
}
