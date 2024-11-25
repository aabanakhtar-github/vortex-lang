#include "Lexer.h"
#include "gtest/gtest.h"
#include <string>

using namespace std::string_literals;
TEST(Lexer, Identifers) {
  auto src = "abc _ _abc a2b3c4 a_b_c _abcdefghijklmnopqrstuvwxyz1234567890_"s;
  auto lexer = Lexer{src, "tests.vrtx"};
  lexer.lex();
  auto &tokens = lexer.getTokens();
  // cases
  EXPECT_EQ(tokens.size(), 7);
  EXPECT_EQ(tokens[0].Lexeme, "abc");
  EXPECT_EQ(tokens[1].Lexeme, "_");
  EXPECT_EQ(tokens[2].Lexeme, "_abc");
  EXPECT_EQ(tokens[3].Lexeme, "a2b3c4");
  EXPECT_EQ(tokens[4].Lexeme, "a_b_c");
  EXPECT_EQ(tokens[5].Lexeme, "_abcdefghijklmnopqrstuvwxyz1234567890_");
}
