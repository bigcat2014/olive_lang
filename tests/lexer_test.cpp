//! @file lexer_test.cpp
//! @brief Pimento lexer unit tests
//! @author Logan Thomas

#include <memory>
#include <sstream>
#include <variant>

#include <gtest/gtest.h>
#include <pimento/lexer.hpp>
#include <pimento/tokens.hpp>

TEST(Lexer, EmptyStream) {
  std::istringstream iss;
  pimento::tokenization::Lexer lexer(iss);

  EXPECT_TRUE(lexer.tokens().empty());
}

TEST(Lexer, TT_ELSE) {
  std::istringstream iss{"else"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_ELSE);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_EXIT) {
  std::istringstream iss{"exit"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_EXIT);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_IF) {
  std::istringstream iss{"if"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_IF);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_ELIF) {
  std::istringstream iss{"elif"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_ELIF);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_LET) {
  std::istringstream iss{"let"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_LET);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_LEFT_CURLY) {
  std::istringstream iss{"{"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_LEFT_CURLY);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_LEFT_PAREN) {
  std::istringstream iss{"("};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_LEFT_PAREN);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_RIGHT_CURLY) {
  std::istringstream iss{"}"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_RIGHT_CURLY);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_RIGHT_PAREN) {
  std::istringstream iss{")"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_RIGHT_PAREN);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_DOUBLE_CARET) {
  std::istringstream iss{"^^"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_DOUBLE_CARET);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::BinOpProperties>(
      lexer.tokens()[0].properties));

  pimento::tokenization::BinOpProperties properties =
      std::get<pimento::tokenization::BinOpProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.associativity,
            pimento::tokenization::BinOpProperties::Associativity::RIGHT);
  EXPECT_EQ(properties.precedence, 3);
}

TEST(Lexer, TT_FORWARD_SLASH) {
  std::istringstream iss{"/"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_FORWARD_SLASH);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::BinOpProperties>(
      lexer.tokens()[0].properties));
  pimento::tokenization::BinOpProperties properties =
      std::get<pimento::tokenization::BinOpProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.associativity,
            pimento::tokenization::BinOpProperties::Associativity::LEFT);
  EXPECT_EQ(properties.precedence, 2);
}

TEST(Lexer, TT_MINUS) {
  std::istringstream iss{"-"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_MINUS);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::BinOpProperties>(
      lexer.tokens()[0].properties));
  pimento::tokenization::BinOpProperties properties =
      std::get<pimento::tokenization::BinOpProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.associativity,
            pimento::tokenization::BinOpProperties::Associativity::LEFT);
  EXPECT_EQ(properties.precedence, 1);
}

TEST(Lexer, TT_PERCENT) {
  std::istringstream iss{"%"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_PERCENT);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::BinOpProperties>(
      lexer.tokens()[0].properties));
  pimento::tokenization::BinOpProperties properties =
      std::get<pimento::tokenization::BinOpProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.associativity,
            pimento::tokenization::BinOpProperties::Associativity::LEFT);
  EXPECT_EQ(properties.precedence, 2);
}

TEST(Lexer, TT_PLUS) {
  std::istringstream iss{"+"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_PLUS);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::BinOpProperties>(
      lexer.tokens()[0].properties));
  pimento::tokenization::BinOpProperties properties =
      std::get<pimento::tokenization::BinOpProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.associativity,
            pimento::tokenization::BinOpProperties::Associativity::LEFT);
  EXPECT_EQ(properties.precedence, 1);
}

TEST(Lexer, TT_STAR) {
  std::istringstream iss{"*"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_STAR);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::BinOpProperties>(
      lexer.tokens()[0].properties));
  pimento::tokenization::BinOpProperties properties =
      std::get<pimento::tokenization::BinOpProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.associativity,
            pimento::tokenization::BinOpProperties::Associativity::LEFT);
  EXPECT_EQ(properties.precedence, 2);
}

TEST(Lexer, TT_LT) {
  std::istringstream iss{"<"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_LT);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::BinOpProperties>(
      lexer.tokens()[0].properties));
  pimento::tokenization::BinOpProperties properties =
      std::get<pimento::tokenization::BinOpProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.associativity,
            pimento::tokenization::BinOpProperties::Associativity::LEFT);
  EXPECT_EQ(properties.precedence, 0);
}

TEST(Lexer, TT_GT) {
  std::istringstream iss{">"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_GT);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::BinOpProperties>(
      lexer.tokens()[0].properties));
  pimento::tokenization::BinOpProperties properties =
      std::get<pimento::tokenization::BinOpProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.associativity,
            pimento::tokenization::BinOpProperties::Associativity::LEFT);
  EXPECT_EQ(properties.precedence, 0);
}

TEST(Lexer, TT_EQUAL) {
  std::istringstream iss{"="};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_EQUAL);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_SEMI) {
  std::istringstream iss{";"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_SEMI);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_WHILE) {
  std::istringstream iss{"while"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_WHILE);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_INT_LITERAL) {
  std::istringstream iss{"123456"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_INT_LITERAL);
  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IntLitProperties>(
      lexer.tokens()[0].properties));

  pimento::tokenization::IntLitProperties properties =
      std::get<pimento::tokenization::IntLitProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.value, std::stoull(iss.str()));
}

TEST(Lexer, TT_IDENTIFIER) {
  std::istringstream iss{"qwerty"};
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_IDENTIFIER);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IdentProperties>(
      lexer.tokens()[0].properties));

  pimento::tokenization::IdentProperties properties =
      std::get<pimento::tokenization::IdentProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.identifier, iss.str());
}

TEST(Lexer, MAX_TOKEN_LENGTH) {
  std::istringstream iss{
      "MaxTokenLengthExceededMaxTokenLengthExceededMaxTokenLengthExceeded"};
  EXPECT_EXIT(pimento::tokenization::Lexer lexer(iss),
              testing::ExitedWithCode(EXIT_FAILURE),
              "Max token length .*exceeded.");
}