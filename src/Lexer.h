#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>
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
  // consume() is one character behind of the pos_ variable,
  // so for matching cases like '->' or '!=' this is useful
  auto peek() -> char;
  auto consume() -> char;
  auto convertIdentifier() -> TokenType;
  auto addToken(TokenType type, LiteralVariant value = None{}) -> void;
  auto addNumber() -> void;
  auto addString() -> void;
  // ch = first character of the lexeme
  auto addIdentifierOrKeyword(char ch) -> void;
  auto removeComment() -> void;

private:
  std::size_t token_start_ = 0;
  std::size_t pos_ = 0;
  std::size_t line_ = 1;
  std::ofstream *debug_file_;
  std::string file_;
  std::string filename_;
  std::vector<Token> tokens_;

  std::unordered_map<std::string, TokenType> token_to_keyword_ = {
      {"Bool", TokenType::BOOL},     {"Float", TokenType::FLOAT},
      {"String", TokenType::STRING}, {"if", TokenType::IF},
      {"for", TokenType::FOR},       {"while", TokenType::WHILE},
      {"create", TokenType::CREATE}, {"return", TokenType::RETURN},
      {"class", TokenType::CLASS},   {"super", TokenType::SUPER},
      {"extern", TokenType::EXTERN}, {"true", TokenType::TRUE},
      {"false", TokenType::FALSE},   {"Array", TokenType::ARRAY},
      {"class", TokenType::CLASS}};
};

#endif // LEXER_H
