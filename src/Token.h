#ifndef TOKEN_H
#define TOKEN_H
#include "Util.h"
#include <string>
#include <variant>

enum class TokenType : std::size_t {
  // Keywords
  BOOL = 0,
  FLOAT,
  STRING,
  INT,
  IF,
  FOR,
  WHILE,
  CREATE,
  RETURN,
  CLASS,
  SUPER,
  EXTERN,

  // Data Types
  BOOLEAN_LITERAL, // true, false
  FLOAT_LITERAL,   // e.g., 2.3
  STRING_LITERAL,  // e.g., "Hello World"
  INTEGER_LITERAL, // e.g., 5

  // Operators
  PLUS,         // +
  MINUS,        // -
  AND,          // &&
  OR,           // ||
  NOT,          // !
  LESS_THAN,    // <
  GREATER_THAN, // >
  ASSIGNMENT,   // =
  EQUALITY,     // ==
  INEQUALITY,   // !=
  ARROW,        // ->
  DOT,          // .

  // Symbols
  L_PAREN,   // (
  R_PAREN,   // )
  L_BRACE,   // {
  R_BRACE,   // }
  L_BRACKET, // [
  R_BRACKET, // ]
  SEMICOLON, // ;
  COMMA,     // ,

  // Control Flow
  IF_CONTROL,    // if
  FOR_CONTROL,   // for
  WHILE_CONTROL, // while

  // Collection Types
  ARRAY, // Array keyword

  // Class and Method Definitions
  CLASS_DEF, // class
  METHOD,    // Method names like doSomething, etc.

  // External Modules names
  EXTERN_MODULE, // extern

  // End of File or Invalid
  END_OF_FILE,
  INVALID
};

struct Token {
  std::string Lexeme;
  std::size_t Line;
  std::variant<None> Value;
  TokenType Type;
};

auto toString(TokenType type) -> std::string;
#endif // !TOKEN_H
