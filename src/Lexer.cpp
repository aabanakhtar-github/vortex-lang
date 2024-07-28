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
    switch (char ch = consume()) {
    case '\n':
      line_++;
      break;
    case ' ':
    case '\t':
    case '\r':
      break;
    case '#':
      // TODO: add comments!!
      break;
    case '(':
      addToken(TokenType::L_PAREN);
      break;
    case ')':
      addToken(TokenType::R_PAREN);
      break;
    case '[':
      addToken(TokenType::L_BRACE);
      break;
    case ']':
      addToken(TokenType::R_BRACE);
      break;
    case '+':
      addToken(TokenType::PLUS);
      break;
    case '.':
      addToken(TokenType::DOT);
      break;
    case ',':
      addToken(TokenType::COMMA);
      break;
    case ';':
      addToken(TokenType::SEMICOLON);
      break;
    case '"':
      consume(); // consume the first quote
      addString();
      break;
    default:
      if (isalpha(ch)) {
        addIdentifierOrKeyword();
        break;
      }
      if (isdigit(ch)) {
        addNumber();
        break;
      }
      reportError(std::format("Unexpected token '{}'!", ch), filename_, line_);
      addToken(TokenType::INVALID);
      break;
    }
    // move on to the next ch
    token_start_ = pos_;
  }
  addToken(TokenType::END_OF_FILE);
}

auto Lexer::peek() -> char {
  if (pos_ + 1 > file_.size()) {
    return '\0';
  }
  return file_[pos_ + 1];
}

auto Lexer::consume() -> char {
  pos_++;
  return file_[pos_ - 1];
}

auto Lexer::convertIdentifier() -> TokenType { return {}; }

auto Lexer::addToken(TokenType type, LiteralVariant value) -> void {
  auto lexeme = file_.substr(token_start_, pos_ - token_start_ + 1);
  Token tok = {.Lexeme = lexeme, .Line = line_, .Value = value, .Type = type};
  tokens_.push_back(tok);
}

auto Lexer::addString() -> void {
  auto error = bool{false};
  auto ch = consume();
  while (ch != '"' && pos_ < file_.size()) {
    if (ch == '\n') {
      std::cout << "hi!" << "\n";
      ++line_;
    }
    ch = consume();
  }
  if (pos_ == file_.size()) {
    reportError("Unterminated string literal!", filename_, line_);
    return;
  } else {
    auto lexeme = file_.substr(token_start_ + 1, pos_ - token_start_ - 2);
    addToken(TokenType::STRING_LITERAL, lexeme);
  }
}

auto Lexer::addIdentifierOrKeyword() -> void {}

auto Lexer::addNumber() -> void {}
