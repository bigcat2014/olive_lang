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
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>();
  pimento::tokenization::Lexer lexer(iss);

  EXPECT_TRUE(lexer.tokens().empty());
}

TEST(Lexer, TT_ELSE) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("else");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_ELSE);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_EXIT) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("exit");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_EXIT);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_IF) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("if");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_IF);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_ELIF) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("elif");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_ELIF);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_LET) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("let");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_LET);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_LEFT_CURLY) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("{");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_LEFT_CURLY);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_LEFT_PAREN) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("(");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_LEFT_PAREN);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_RIGHT_CURLY) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("}");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_RIGHT_CURLY);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_RIGHT_PAREN) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>(")");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_RIGHT_PAREN);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_DOUBLE_CARET) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("^^");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
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
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("/");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
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
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("-");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
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
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("%");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
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
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("+");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
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
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("*");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
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
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("<");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
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
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>(">");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
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
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("=");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_EQUAL);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_SEMI) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>(";");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_SEMI);
  EXPECT_TRUE(
      std::holds_alternative<std::monostate>(lexer.tokens()[0].properties));
}

TEST(Lexer, TT_INT_LITERAL) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("123456");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_INT_LITERAL);
  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IntLitProperties>(
      lexer.tokens()[0].properties));

  pimento::tokenization::IntLitProperties properties =
      std::get<pimento::tokenization::IntLitProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.value, std::stoull(iss->str()));
}

TEST(Lexer, TT_IDENTIFIER) {
  const std::shared_ptr<std::istringstream> iss =
      std::make_shared<std::istringstream>("qwerty");
  pimento::tokenization::Lexer lexer(iss);

  ASSERT_FALSE(lexer.tokens().empty());
  EXPECT_EQ(lexer.tokens()[0].token_type,
            pimento::tokenization::TokenType::TT_IDENTIFIER);

  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IdentProperties>(
      lexer.tokens()[0].properties));

  pimento::tokenization::IdentProperties properties =
      std::get<pimento::tokenization::IdentProperties>(
          lexer.tokens()[0].properties);
  EXPECT_EQ(properties.identifier, iss->str());
}