//! @file lexer_test.cpp
//! @brief Pimento lexer unit tests
//! @author Logan Thomas

#include <memory>
#include <sstream>
#include <variant>

#include "lexer_test.hpp"

#include <gtest/gtest.h>
#include <pimento/lexer.hpp>
#include <pimento/tokens.hpp>

using namespace pimento::tokenization;

TEST(Lexer, EmptyStream) {
  std::istringstream iss;
  Lexer lexer(iss);

  EXPECT_TRUE(lexer.tokens().empty());
}

TEST(Lexer, TT_ELSE) {
  std::istringstream iss{"else"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_ELSE);
}

TEST(Lexer, TT_EXIT) {
  std::istringstream iss{"exit"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_EXIT);
}

TEST(Lexer, TT_WHILE) {
  std::istringstream iss{"while"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_WHILE);
}

TEST(Lexer, TT_IF) {
  std::istringstream iss{"if"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_IF);
}

TEST(Lexer, TT_ELIF) {
  std::istringstream iss{"elif"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_ELIF);
}

TEST(Lexer, TT_LET) {
  std::istringstream iss{"let"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_LET);
}

TEST(Lexer, TT_LEFT_CURLY) {
  std::istringstream iss{"{"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_LEFT_CURLY);
}

TEST(Lexer, TT_LEFT_PAREN) {
  std::istringstream iss{"("};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_LEFT_PAREN);
}

TEST(Lexer, TT_RIGHT_CURLY) {
  std::istringstream iss{"}"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_RIGHT_CURLY);
}

TEST(Lexer, TT_RIGHT_PAREN) {
  std::istringstream iss{")"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_RIGHT_PAREN);
}

TEST(Lexer, TT_DOUBLE_CARET) {
  std::istringstream iss{"^^"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_DOUBLE_CARET, 3,
                       BinOpProperties::Associativity::RIGHT);
}

TEST(Lexer, TT_PERCENT) {
  std::istringstream iss{"%"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_PERCENT, 2,
                       BinOpProperties::Associativity::LEFT);
}

TEST(Lexer, TT_STAR) {
  std::istringstream iss{"*"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_STAR, 2,
                       BinOpProperties::Associativity::LEFT);
}

TEST(Lexer, TT_FORWARD_SLASH) {
  std::istringstream iss{"/"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_FORWARD_SLASH, 2,
                       BinOpProperties::Associativity::LEFT);
}

TEST(Lexer, TT_MINUS) {
  std::istringstream iss{"-"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_MINUS, 1,
                       BinOpProperties::Associativity::LEFT);
}

TEST(Lexer, TT_PLUS) {
  std::istringstream iss{"+"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_PLUS, 1,
                       BinOpProperties::Associativity::LEFT);
}

TEST(Lexer, TT_LT) {
  std::istringstream iss{"<"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_LT, 0,
                       BinOpProperties::Associativity::LEFT);
}

TEST(Lexer, TT_GT) {
  std::istringstream iss{">"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_GT, 0,
                       BinOpProperties::Associativity::LEFT);
}

TEST(Lexer, TT_EQUAL) {
  std::istringstream iss{"="};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_EQUAL);
}

TEST(Lexer, TT_SEMI) {
  std::istringstream iss{";"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_SEMI);
}

TEST(Lexer, TT_INT_LITERAL) {
  std::istringstream iss{"123456"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_INT_LIT_TOKEN(lexer.tokens()[0], std::stoull(iss.str()));
}

TEST(Lexer, TT_IDENTIFIER) {
  std::istringstream iss{"qwerty"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_IDENTIFIER_TOKEN(lexer.tokens()[0], iss.str());
}

TEST(Lexer, MAX_TOKEN_LENGTH) {
  std::istringstream iss{
      "MaxTokenLengthExceededMaxTokenLengthExceededMaxTokenLengthExceeded"};
  EXPECT_EXIT(pimento::tokenization::Lexer lexer(iss),
              testing::ExitedWithCode(EXIT_FAILURE),
              "Max token length .*exceeded.");
}