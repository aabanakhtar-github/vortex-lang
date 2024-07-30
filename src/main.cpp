#include "Lexer.h"
#include "Token.h"
#include <fstream>
#include <iostream>

auto main(int argc, char *argv[]) -> int {
  auto files = std::string{};
  auto s = std::string{};
  auto file = std::ifstream{"main.vrtx"};
  while (std::getline(file, s)) {
    files += s + '\n';
  }
  std::cout << files << std::endl;
  auto lex = Lexer{files, "main.vrtx"};
  lex.lex();
  auto x = lex.getTokens();
  prettyPrint(std::cout, x);
  std::cin.get();
  return 0;
}
