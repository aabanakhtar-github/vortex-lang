#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

struct TokenizedFile {
  bool Valid;
  std::vector<TokenType> &tokens;
};

class Lexer {
public:
  explicit Lexer(std::string_view file, std::string_view filename,
                 std::ofstream *debug_file = nullptr);
  auto getTokens() const -> const std::vector<Token> & { return tokens_; }
  auto lex() -> void;

private:
  auto peek() -> char;
  auto consume() -> char;
  auto convertIdentifier() -> TokenType;
  auto addToken(TokenType type, LiteralVariant value = None{}) -> void;

  auto addNumber() -> void;
  auto addString() -> void;
  auto addIdentifierOrKeyword() -> void;

private:
  std::size_t token_start_ = 0;
  std::size_t pos_ = 0;
  std::size_t line_ = 1;
  std::ofstream *debug_file_;
  std::string file_;
  std::string filename_;
  std::vector<Token> tokens_;
};

#endif // LEXER_H
