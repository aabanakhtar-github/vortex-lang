#include "Lexer.h"
#include "Error.h"
#include "Token.h"
#include <cctype>
#include <format>

Lexer::Lexer(std::string_view file, std::string_view filename,
             std::ofstream *debug_file)
    : file_{file}, filename_{filename}, debug_file_{debug_file} {}

auto Lexer::lex() -> void {
  while (pos_ < file_.size()) {
    switch (auto ch = consume()) {
    case '\n':
      line_++;
      break;
    case ' ':
    case '\t': // something historic i think
    case '\r':
      break;
    case '#':
      removeComment();
      break;
    case '{':
      addToken(TokenType::L_BRACE);
      break;
    case '}':
      addToken(TokenType::R_BRACE);
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
    case '*':
      addToken(TokenType::MUL);
      break;
    case '/':
      addToken(TokenType::DIV);
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
    case '=':
      if (peek() == '>') {
        consume();
        addToken(TokenType::THICK_ARROW);
        break;
      }
      addToken(TokenType::EQUALITY);
      break;
    case '"':
      addString();
      break;
    case '-':
      if (peek() == '>') {
        consume(); // the right angle
        addToken(TokenType::ASSIGNMENT);
      } else {
        addToken(TokenType::MINUS);
      }
      break;
    case '>':
      if (peek() == '=') {
        consume();
        addToken(TokenType::GREATER_THAN_OR_EQUAL);
      } else {
        addToken(TokenType::GREATER_THAN);
      }
      break;
    case '<':
      if (peek() == '=') {
        consume();
        addToken(TokenType::LESS_THAN_OR_EQUAL);
      } else {
        addToken(TokenType::LESS_THAN);
      }
      break;
    case '!':
      if (peek() == '=') {
        consume();
        addToken(TokenType::INEQUALITY);
      } else {
        addToken(TokenType::NOT);
      }
      break;
    case '&':
      if (peek() != '&') {
        reportError("Expected & after &!", file_, line_);
        addToken(TokenType::INVALID);
        break;
      }
      consume();
      addToken(TokenType::AND);
      break;
    case '|':
      if (peek() != '|') {
        reportError("Expected | after |!", file_, line_);
        addToken(TokenType::INVALID);
        break;
      }
      consume();
      addToken(TokenType::OR);
      break;
    default:
      if (isalpha(ch)) {
        addIdentifierOrKeyword(ch);
        break;
      } else if (isalnum(ch)) {
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

  // log all the tokens for debug purposes
  if (debug_file_ != nullptr) {
    prettyPrint(*debug_file_, tokens_);
  }
}

auto Lexer::consume() -> char {
  ++pos_;
  return file_[pos_ - 1];
}

auto Lexer::peek() -> char {
  if (pos_ < file_.size()) {
    return file_[pos_];
  }
  return '\0';
}

auto Lexer::convertIdentifier() -> TokenType { return {}; }

auto Lexer::addToken(TokenType type, LiteralVariant value) -> void {
  auto lexeme = file_.substr(token_start_, pos_ - token_start_);
  auto tok =
      Token{.Lexeme = lexeme, .Line = line_, .Value = value, .Type = type};
  tokens_.push_back(tok);
}

auto Lexer::addString() -> void {
  auto error = bool{false};
  auto ch = consume();
  while (pos_ < file_.size() && ch != '"') {
    if (ch == '\n') {
      ++line_;
    }
    ch = consume();
  }
  if (pos_ == file_.size()) {
    reportError("Unterminated string literal!", filename_, line_);
    return;
  } else {
    auto literal = file_.substr(token_start_ + 1, pos_ - token_start_ - 2);
    addToken(TokenType::STRING_LITERAL, literal);
  }
}

auto Lexer::addIdentifierOrKeyword(char ch) -> void {
  while (pos_ < file_.size() && isalnum(peek())) {
    consume();
  }
  auto word = file_.substr(token_start_, pos_ - token_start_);
  if (token_to_keyword_.contains(word)) {
    addToken(token_to_keyword_[word]);
    return;
  }
  addToken(TokenType::IDENTIFIER);
}

auto Lexer::addNumber() -> void {
  while (pos_ < file_.size() && isdigit(peek())) {
    consume();
  }
  if (peek() == '.') {
    consume();
    if (!isdigit(peek())) {
      reportError("Expected digits after '.' in floating point literal!",
                  filename_, line_);
      addToken(TokenType::INVALID);
      return;
    }
    while (pos_ < file_.size() && isdigit(peek())) {
      consume();
    }
  }
  auto numstr = file_.substr(token_start_, pos_ - token_start_);
  auto value = std::stod(numstr);
  addToken(TokenType::FLOAT_LITERAL, value);
}

auto Lexer::removeComment() -> void {
  while (pos_ < file_.size() && consume() != '\n') {
  }
  line_++;
}
