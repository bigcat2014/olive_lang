//! @file parser_test.cpp
//! @brief Pimento parser unit tests
//! @author Logan Thomas

#include <memory>
#include <sstream>
#include <variant>

#include "parser_test.hpp"

#include <gtest/gtest.h>
#include <pimento/parser.hpp>
#include <pimento/tokens.hpp>

using namespace pimento::ast;

TEST(Parser, EmptyStream) {
  std::istringstream iss;
  Parser parser(iss);

  EXPECT_TRUE(parser.get_program().statements.empty());
}

TEST(Parser, StmtExit) {
  std::istringstream iss{"exit(0);"};
  Parser parser(iss);

  ASSERT_EQ(parser.get_program().statements.size(), 1);
  // Ensure node is "Statement Exit AST node"
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtExitNode>>(
      parser.get_program().statements[0]->node));
  std::shared_ptr<node::StmtExitNode> stmt_exit_node =
      std::get<std::shared_ptr<node::StmtExitNode>>(
          parser.get_program().statements[0]->node);

  // Check that the expression is correct
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      stmt_exit_node->expression->node));
  std::shared_ptr<node::TermNode> term_node =
      std::get<std::shared_ptr<node::TermNode>>(
          stmt_exit_node->expression->node);

  TEST_INT_LIT_TERM(term_node->node, 0);
}

TEST(Parser, StmtLet) {
  std::istringstream iss{"let x = 1;"};
  Parser parser(iss);

  ASSERT_EQ(parser.get_program().statements.size(), 1);
  // Ensure node is "Statement Let AST node"
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(
      parser.get_program().statements[0]->node));
  std::shared_ptr<node::StmtLetNode> stmt_let_node =
      std::get<std::shared_ptr<node::StmtLetNode>>(
          parser.get_program().statements[0]->node);

  // Check the token type and properties are correct
  TEST_IDENTIFIER_TOKEN(stmt_let_node->identifier, "x");

  // Check that the expression is correct
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::TermNode> term_node =
      std::get<std::shared_ptr<node::TermNode>>(
          stmt_let_node->expression->node);

  TEST_INT_LIT_TERM(term_node->node, 1);
}

TEST(Parser, StmtAssign) {
  std::istringstream iss{"x = 1;"};
  Parser parser(iss);

  ASSERT_EQ(parser.get_program().statements.size(), 1);
  // Ensure node is "Statement Assign AST node"
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtAssignNode>>(
      parser.get_program().statements[0]->node));
  std::shared_ptr<node::StmtAssignNode> stmt_assign_node =
      std::get<std::shared_ptr<node::StmtAssignNode>>(
          parser.get_program().statements[0]->node);

  // Check the token type and properties are correct
  TEST_IDENTIFIER_TOKEN(stmt_assign_node->identifier, "x");

  // Check that the expression is correct
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      stmt_assign_node->expression->node));
  std::shared_ptr<node::TermNode> term_node =
      std::get<std::shared_ptr<node::TermNode>>(
          stmt_assign_node->expression->node);

  TEST_INT_LIT_TERM(term_node->node, 1);
}

TEST(Parser, StmtIf) {
  std::istringstream iss{"if 0 {\n    let x = 1;\n}"};
  Parser parser(iss);

  ASSERT_EQ(parser.get_program().statements.size(), 1);
  // Ensure node is "Statement If AST node"
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtIfNode>>(
      parser.get_program().statements[0]->node));
  std::shared_ptr<node::StmtIfNode> stmt_if_node =
      std::get<std::shared_ptr<node::StmtIfNode>>(
          parser.get_program().statements[0]->node);

  // Check that the expression is correct
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      stmt_if_node->expression->node));
  std::shared_ptr<node::TermNode> term_node =
      std::get<std::shared_ptr<node::TermNode>>(stmt_if_node->expression->node);

  TEST_INT_LIT_TERM(term_node->node, 0);

  EXPECT_EQ(stmt_if_node->scope->statements.size(), 1);
  EXPECT_FALSE(stmt_if_node->ifpred.has_value());
}

TEST(Parser, StmtWhile) {
  std::istringstream iss{"while 0 {\n    let x = 1;\n}"};
  Parser parser(iss);

  ASSERT_EQ(parser.get_program().statements.size(), 1);
  // Ensure node is "Statement While AST node"
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtWhileNode>>(
      parser.get_program().statements[0]->node));
  std::shared_ptr<node::StmtWhileNode> stmt_while_node =
      std::get<std::shared_ptr<node::StmtWhileNode>>(
          parser.get_program().statements[0]->node);

  // Check that the expression is correct
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      stmt_while_node->expression->node));
  std::shared_ptr<node::TermNode> term_node =
      std::get<std::shared_ptr<node::TermNode>>(
          stmt_while_node->expression->node);

  TEST_INT_LIT_TERM(term_node->node, 0);

  EXPECT_EQ(stmt_while_node->scope->statements.size(), 1);
}

TEST(Parser, Scope) {
  std::istringstream iss{"{\n    let x = 1;\n}"};
  Parser parser(iss);

  ASSERT_EQ(parser.get_program().statements.size(), 1);
  // Ensure node is "Scope AST node"
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::ScopeNode>>(
      parser.get_program().statements[0]->node));
  std::shared_ptr<node::ScopeNode> scope_node =
      std::get<std::shared_ptr<node::ScopeNode>>(
          parser.get_program().statements[0]->node);

  ASSERT_EQ(scope_node->statements.size(), 1);
  // Ensure node is "Statement Let AST node"
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(
      scope_node->statements[0]->node));
  std::shared_ptr<node::StmtLetNode> stmt_let_node =
      std::get<std::shared_ptr<node::StmtLetNode>>(
          scope_node->statements[0]->node);

  // Check the token type and properties are correct
  TEST_IDENTIFIER_TOKEN(stmt_let_node->identifier, "x");

  // Check that the expression is correct
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::TermNode> term_node =
      std::get<std::shared_ptr<node::TermNode>>(
          stmt_let_node->expression->node);

  TEST_INT_LIT_TERM(term_node->node, 1);
}