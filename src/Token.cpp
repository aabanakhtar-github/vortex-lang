#include "Token.h"
#include <sstream>

auto toString(TokenType type) -> std::string {
  std::stringstream ss;
  switch (type) {
  // Keywords
  case TokenType::IDENTIFIER:
    ss << "IDENTIFIER";
    break;
  case TokenType::BOOL:
    ss << "BOOL";
    break;
  case TokenType::FLOAT:
    ss << "FLOAT";
    break;
  case TokenType::FLOAT_LITERAL:
    ss << "FLOAT_LITERAL";
    break;
  case TokenType::STRING:
    ss << "STRING";
    break;
  case TokenType::IF:
    ss << "IF";
    break;
  case TokenType::FOR:
    ss << "FOR";
    break;
  case TokenType::WHILE:
    ss << "WHILE";
    break;
  case TokenType::CREATE:
    ss << "CREATE";
    break;
  case TokenType::RETURN:
    ss << "RETURN";
    break;
  case TokenType::CLASS:
    ss << "CLASS";
    break;
  case TokenType::SUPER:
    ss << "SUPER";
    break;
  case TokenType::EXTERN:
    ss << "EXTERN";
    break;
  // Operators
  case TokenType::PLUS:
    ss << "PLUS";
    break;
  case TokenType::MINUS:
    ss << "MINUS";
    break;
  case TokenType::AND:
    ss << "AND";
    break;
  case TokenType::OR:
    ss << "OR";
    break;
  case TokenType::MUL:
    ss << "MUL";
    break;
  case TokenType::DIV:
    ss << "DIV";
    break;
  case TokenType::NOT:
    ss << "NOT";
    break;
  case TokenType::LESS_THAN:
    ss << "LESS_THAN";
    break;
  case TokenType::GREATER_THAN:
    ss << "GREATER_THAN";
    break;
  case TokenType::ASSIGNMENT:
    ss << "ASSIGNMENT";
    break;
  case TokenType::EQUALITY:
    ss << "EQUALITY";
    break;
  case TokenType::INEQUALITY:
    ss << "INEQUALITY";
    break;
  case TokenType::DOT:
    ss << "DOT";
    break;
  // Symbols
  case TokenType::L_PAREN:
    ss << "L_PAREN";
    break;
  case TokenType::R_PAREN:
    ss << "R_PAREN";
    break;
  case TokenType::L_BRACE:
    ss << "L_BRACE";
    break;
  case TokenType::R_BRACE:
    ss << "R_BRACE";
    break;
  case TokenType::L_BRACKET:
    ss << "L_BRACKET";
    break;
  case TokenType::R_BRACKET:
    ss << "R_BRACKET";
    break;
  case TokenType::SEMICOLON:
    ss << "SEMICOLON";
    break;
  case TokenType::COMMA:
    ss << "COMMA";
    break;
  // Collection Types
  case TokenType::ARRAY:
    ss << "ARRAY";
    break;
  // External Modules names
  case TokenType::EXTERN_MODULE:
    ss << "EXTERN_MODULE";
    break;

  // End of File or Invalid
  case TokenType::END_OF_FILE:
    ss << "END_OF_FILE";
    break;
  case TokenType::INVALID:
    ss << "INVALID";
    break;
  case TokenType::THICK_ARROW:
    ss << "THICK_ARROW";
    break;
  case TokenType::NIL:
    ss << "NIL";
    break;
  case TokenType::FN:
    ss << "FN";
    break;
  case TokenType::PRINT:
    ss << "PRINT";
    break;
  default:
    ss << "Bro, what?";
  }
  return ss.str();
}

auto prettyPrint(std::ostream &stream,
                 const std::vector<Token> &tokens) -> void {
  for (auto &tok : tokens) {
    // print out the type
    auto token_typename = toString(tok.Type);
    stream << "[TYPE: " << token_typename << "]; ";
    auto token_index = tok.Value.index();
    // print out the literal value
    switch (LiteralVariantType{token_index}) {
    case LiteralVariantType::NIL:
      stream << "[Value: NIL]; ";
      break;
    case LiteralVariantType::DOUBLE:
      stream << "[Value: " << std::get<double>(tok.Value) << "]; ";
      break;
    case LiteralVariantType::STRING:
      stream << "[Value: " << std::get<std::string>(tok.Value) << "]; ";
      break;
    case LiteralVariantType::BOOL:
      stream << "[Value: " << std::boolalpha << std::get<bool>(tok.Value)
             << "]; ";
      break;
    }
    stream << "[LINE: " << tok.Line << "]; [Lexeme: {" << tok.Lexeme << "}];\n";
  }
}
