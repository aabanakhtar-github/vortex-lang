#include "Lexer.h"
#include "Error.h"
#include "Token.h"
#include <format>
#include <iostream>
Lexer::Lexer(std::string_view file, std::string_view filename,
             std::ofstream *debug_file)
    : file_{file}, filename_{filename}, debug_file_{debug_file} {}

auto Lexer::lex() -> void {
  while (pos_ < file_.size()) {
    switch (char ch = file_[pos_]) {
    default:
      std::cout << ch << std::endl;
      reportError(std::format("Unexpected token '{}'!", ch), filename_, line_);
      Token invalid_token = {.Lexeme = "",
                             .Line = line_,
                             .Value = None{},
                             .Type = TokenType::INVALID};
      tokens_.push_back(invalid_token);
      break;
    }
    // move on to the next ch
    consume();
  }
  Token EOF_token = {.Lexeme = "",
                     .Line = line_,
                     .Value = None{},
                     .Type = TokenType::END_OF_FILE};
  tokens_.push_back(EOF_token);
}

auto Lexer::peek() -> char {
  if (pos_ + 1 > file_.size()) {
    return '\0';
  }
  return file_[pos_ + 1];
}

auto Lexer::consume() -> char { return file_[pos_++]; }

auto Lexer::convertIdentifier() -> TokenType { return {}; }
