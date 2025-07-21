//! @file parser_test.cpp
//! @brief Pimento parser unit tests
//! @author Logan Thomas

#include <memory>
#include <sstream>
#include <variant>

#include <gtest/gtest.h>
#include <pimento/parser.hpp>
#include <pimento/tokens.hpp>

TEST(Parser, EmptyStream) {
  std::istringstream iss;
  pimento::ast::Parser parser(iss);

  EXPECT_TRUE(parser.get_program().statements.empty());
}

TEST(Parser, StmtLet) {
  std::istringstream iss{"let x = 1;"};
  pimento::ast::Parser parser(iss);

  ASSERT_EQ(parser.get_program().statements.size(), 1);
  // Ensure node is "Statement Let AST node"
  ASSERT_TRUE(
      std::holds_alternative<std::shared_ptr<pimento::ast::node::StmtLetNode>>(
          parser.get_program().statements[0]->node));
  std::shared_ptr<pimento::ast::node::StmtLetNode> stmt_let_node =
      std::get<std::shared_ptr<pimento::ast::node::StmtLetNode>>(
          parser.get_program().statements[0]->node);

  // Check the token type and properties are correct
  EXPECT_EQ(stmt_let_node->identifier.token_type,
            pimento::tokenization::TokenType::TT_IDENTIFIER);
  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IdentProperties>(
      stmt_let_node->identifier.properties));
  pimento::tokenization::IdentProperties ident_properties =
      std::get<pimento::tokenization::IdentProperties>(
          stmt_let_node->identifier.properties);
  EXPECT_EQ(ident_properties.identifier, "x");

  // Check that the expression is correct
  ASSERT_TRUE(
      std::holds_alternative<std::shared_ptr<pimento::ast::node::TermNode>>(
          stmt_let_node->expression->node));
  std::shared_ptr<pimento::ast::node::TermNode> term_node =
      std::get<std::shared_ptr<pimento::ast::node::TermNode>>(
          stmt_let_node->expression->node);
  ASSERT_TRUE(std::holds_alternative<
              std::shared_ptr<pimento::ast::node::TermIntLitNode>>(
      term_node->node));
  std::shared_ptr<pimento::ast::node::TermIntLitNode> term_int_lit_node =
      std::get<std::shared_ptr<pimento::ast::node::TermIntLitNode>>(
          term_node->node);
  EXPECT_EQ(term_int_lit_node->int_lit_token.token_type,
            pimento::tokenization::TokenType::TT_INT_LITERAL);
  ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IntLitProperties>(
      term_int_lit_node->int_lit_token.properties));
  pimento::tokenization::IntLitProperties int_lit_properties =
      std::get<pimento::tokenization::IntLitProperties>(
          term_int_lit_node->int_lit_token.properties);
  EXPECT_EQ(int_lit_properties.value, 1);
}