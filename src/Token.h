#ifndef TOKEN_H
#define TOKEN_H
#include "Util.h"
#include <string>
#include <variant>

enum class TokenType {
  // Keywords
  BOOL,
  FLOAT,
  STRING,
  IF,
  FOR,
  WHILE,
  CREATE,
  RETURN,
  CLASS,
  SUPER,
  EXTERN,

  // Data Types
  TRUE,
  FALSE,          // boolean literals e.g true false
  FLOAT_LITERAL,  // e.g., 2.3
  STRING_LITERAL, // e.g., "Hello World"

  // Operators
  PLUS,                  // +
  MINUS,                 // -
  AND,                   // &&
  OR,                    // ||
  NOT,                   // !
  LESS_THAN,             // <
  GREATER_THAN,          // >
  GREATER_THAN_OR_EQUAL, // >=
  LESS_THAN_OR_EQUAL,    // <=
  ASSIGNMENT,            // ->
  EQUALITY,              // =
  INEQUALITY,            // !=
  DOT,                   // .

  // Symbols
  L_PAREN,   // (
  R_PAREN,   // )
  L_BRACE,   // {
  R_BRACE,   // }
  L_BRACKET, // [
  R_BRACKET, // ]
  SEMICOLON, // ;
  COMMA,     // ,

  ARRAY,      // Array keyword
  IDENTIFIER, // name

  // External Modules names
  EXTERN_MODULE, // extern

  // End of File or Invalid
  END_OF_FILE,
  INVALID
};

using LiteralVariant = std::variant<None, std::string>;

struct Token {
  std::string Lexeme;
  std::size_t Line;
  LiteralVariant Value;
  TokenType Type;
};

auto toString(TokenType type) -> std::string;

#endif // !TOKEN_H
