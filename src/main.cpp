#include "Lexer.h"
#include "Parser.h"
#include <ios>
#include <iostream>

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
  auto parser = Parser{lexer.getTokens()};
  parser.parse();
  program.close();
  file.close();
  return 0;
}
