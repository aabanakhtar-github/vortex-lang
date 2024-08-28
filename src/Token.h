#ifndef TOKEN_H
#define TOKEN_H
#include "Util.h"
#include <string>
#include <variant>
#include <vector>

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
  NIL, // nothing
  // Data Types
  TRUE,
  FALSE,          // boolean literals e.g true false
  FLOAT_LITERAL,  // e.g., 2.3
  STRING_LITERAL, // e.g., "Hello World"

  // Operators
  PLUS,                  // +
  MINUS,                 // -
  MUL,                   // *
  DIV,                   // /
  AND,                   // &&
  OR,                    // ||
  NOT,                   // !
  LESS_THAN,             // <
  GREATER_THAN,          // >
  GREATER_THAN_OR_EQUAL, // >=
  LESS_THAN_OR_EQUAL,    // <=
  THICK_ARROW,           // =>
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
  PRINT,
  FN,
  // External Modules names
  EXTERN_MODULE, // extern

  // End of File or Invalid
  END_OF_FILE,
  INVALID
};

enum class LiteralVariantType : std::size_t { NIL = 0, STRING, DOUBLE, BOOL };

using LiteralVariant = std::variant<None, std::string, double, bool>;
// changed my mind but don't wanna break code.
using Nil = None;

struct Token {
  std::string Lexeme;
  std::size_t Line;
  LiteralVariant Value;
  TokenType Type;
};

auto toString(TokenType type) -> std::string;
auto prettyPrint(std::ostream &stream,
                 const std::vector<Token> &tokens) -> void;

#endif // !TOKEN_H
