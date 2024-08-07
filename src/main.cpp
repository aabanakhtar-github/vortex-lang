#include "AST.h"
#include "Lexer.h"
#include "Parser.h"
#include "PrettyPrintExpressionVisitor.h"
#include <fstream>
#include <ios>
#include <iostream>
auto main(int argc, char *argv[]) -> int {
  auto program =
      std::ifstream("test_programs/expression_test.vrtx", std::ios_base::in);
  auto program_str = std::string{};
  auto s = std::string{};
  while (std::getline(program, s)) {
    program_str += s + '\n';
  }
  auto file = std::ofstream{"lexer_output.txt", std::ios_base::out};
  auto lexer = Lexer{program_str, "main.vrtx", &file};
  lexer.lex();
  auto parser = Parser{"main.vrtx", lexer.getTokens()};
  auto &result = parser.parse();
  auto visitor = PrettyPrintExpressionVisitor();
  for (auto &r : result)
    r->acceptVisitor(&visitor);
  program.close();
  file.close();
  std::cin.get();
  return 0;
}
