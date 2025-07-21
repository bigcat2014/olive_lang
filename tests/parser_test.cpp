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

TEST(Parser, IfPredElif) {
  std::istringstream iss{
      "if 0 {\n    let x = 1;\n} elif 2 {\n    let x = 3;\n}"};
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

  ASSERT_TRUE(stmt_if_node->ifpred.has_value());
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(
      stmt_if_node->ifpred.value()->node));
  std::shared_ptr<node::IfPredElifNode> ifpred_elif_node =
      std::get<std::shared_ptr<node::IfPredElifNode>>(
          stmt_if_node->ifpred.value()->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      ifpred_elif_node->expression->node));
  std::shared_ptr<node::TermNode> elif_term_node =
      std::get<std::shared_ptr<node::TermNode>>(
          ifpred_elif_node->expression->node);

  TEST_INT_LIT_TERM(elif_term_node->node, 2);

  EXPECT_EQ(ifpred_elif_node->scope->statements.size(), 1);

  EXPECT_FALSE(ifpred_elif_node->ifpred.has_value());
}

TEST(Parser, IfPredElse) {
  std::istringstream iss{"if 0 {\n    let x = 1;\n} else {\n    let x = 3;\n}"};
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

  ASSERT_TRUE(stmt_if_node->ifpred.has_value());
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElseNode>>(
      stmt_if_node->ifpred.value()->node));
  std::shared_ptr<node::IfPredElseNode> ifpred_else_node =
      std::get<std::shared_ptr<node::IfPredElseNode>>(
          stmt_if_node->ifpred.value()->node);

  EXPECT_EQ(ifpred_else_node->scope->statements.size(), 1);
}

TEST(Parser, IfPredElifElse) {
  std::istringstream iss{"if 0 {\n    let x = 1;\n} elif 2 {\n    let x = "
                         "3;\n} else {\n    let x = 4;\n}"};
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

  ASSERT_TRUE(stmt_if_node->ifpred.has_value());
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(
      stmt_if_node->ifpred.value()->node));
  std::shared_ptr<node::IfPredElifNode> ifpred_elif_node =
      std::get<std::shared_ptr<node::IfPredElifNode>>(
          stmt_if_node->ifpred.value()->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      ifpred_elif_node->expression->node));
  std::shared_ptr<node::TermNode> elif_term_node =
      std::get<std::shared_ptr<node::TermNode>>(
          ifpred_elif_node->expression->node);

  TEST_INT_LIT_TERM(elif_term_node->node, 2);

  EXPECT_EQ(ifpred_elif_node->scope->statements.size(), 1);

  ASSERT_TRUE(ifpred_elif_node->ifpred.has_value());
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElseNode>>(
      ifpred_elif_node->ifpred.value()->node));
  std::shared_ptr<node::IfPredElseNode> ifpred_else_node =
      std::get<std::shared_ptr<node::IfPredElseNode>>(
          ifpred_elif_node->ifpred.value()->node);

  EXPECT_EQ(ifpred_else_node->scope->statements.size(), 1);
}

TEST(Parser, IfPredElifElif) {
  std::istringstream iss{"if 0 {\n    let x = 1;\n} elif 2 {\n    let x = "
                         "3;\n} elif 4 {\n    let x = 5;\n}"};
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

  ASSERT_TRUE(stmt_if_node->ifpred.has_value());
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(
      stmt_if_node->ifpred.value()->node));
  std::shared_ptr<node::IfPredElifNode> ifpred_elif_node1 =
      std::get<std::shared_ptr<node::IfPredElifNode>>(
          stmt_if_node->ifpred.value()->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      ifpred_elif_node1->expression->node));
  std::shared_ptr<node::TermNode> elif_term_node1 =
      std::get<std::shared_ptr<node::TermNode>>(
          ifpred_elif_node1->expression->node);

  TEST_INT_LIT_TERM(elif_term_node1->node, 2);

  EXPECT_EQ(ifpred_elif_node1->scope->statements.size(), 1);

  ASSERT_TRUE(ifpred_elif_node1->ifpred.has_value());
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(
      ifpred_elif_node1->ifpred.value()->node));
  std::shared_ptr<node::IfPredElifNode> ifpred_elif_node2 =
      std::get<std::shared_ptr<node::IfPredElifNode>>(
          ifpred_elif_node1->ifpred.value()->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      ifpred_elif_node2->expression->node));
  std::shared_ptr<node::TermNode> elif_term_node2 =
      std::get<std::shared_ptr<node::TermNode>>(
          ifpred_elif_node2->expression->node);

  TEST_INT_LIT_TERM(elif_term_node2->node, 4);

  EXPECT_EQ(ifpred_elif_node2->scope->statements.size(), 1);
  EXPECT_FALSE(ifpred_elif_node2->ifpred.has_value());
}

TEST(Parser, IfPredElifElifElse) {
  std::istringstream iss{
      "if 0 {\n    let x = 1;\n} elif 2 {\n    let x = "
      "3;\n} elif 4 {\n    let x = 5;\n} else {\n    let x = 6;\n}"};
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

  ASSERT_TRUE(stmt_if_node->ifpred.has_value());
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(
      stmt_if_node->ifpred.value()->node));
  std::shared_ptr<node::IfPredElifNode> ifpred_elif_node1 =
      std::get<std::shared_ptr<node::IfPredElifNode>>(
          stmt_if_node->ifpred.value()->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      ifpred_elif_node1->expression->node));
  std::shared_ptr<node::TermNode> elif_term_node1 =
      std::get<std::shared_ptr<node::TermNode>>(
          ifpred_elif_node1->expression->node);

  TEST_INT_LIT_TERM(elif_term_node1->node, 2);

  EXPECT_EQ(ifpred_elif_node1->scope->statements.size(), 1);

  ASSERT_TRUE(ifpred_elif_node1->ifpred.has_value());
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(
      ifpred_elif_node1->ifpred.value()->node));
  std::shared_ptr<node::IfPredElifNode> ifpred_elif_node2 =
      std::get<std::shared_ptr<node::IfPredElifNode>>(
          ifpred_elif_node1->ifpred.value()->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      ifpred_elif_node2->expression->node));
  std::shared_ptr<node::TermNode> elif_term_node2 =
      std::get<std::shared_ptr<node::TermNode>>(
          ifpred_elif_node2->expression->node);

  TEST_INT_LIT_TERM(elif_term_node2->node, 4);

  EXPECT_EQ(ifpred_elif_node2->scope->statements.size(), 1);

  ASSERT_TRUE(ifpred_elif_node2->ifpred.has_value());
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElseNode>>(
      ifpred_elif_node2->ifpred.value()->node));
  std::shared_ptr<node::IfPredElseNode> ifpred_else_node =
      std::get<std::shared_ptr<node::IfPredElseNode>>(
          ifpred_elif_node2->ifpred.value()->node);

  EXPECT_EQ(ifpred_else_node->scope->statements.size(), 1);
}

TEST(Parser, TermIdent) {
  std::istringstream iss{"let x = y;"};
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

  TEST_IDENTIFIER_TERM(term_node->node, "y");
}

TEST(Parser, BinExprPower) {
  std::istringstream iss{"let x = 2 ^^ 3;"};
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
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::BinExprNode> binexpr_node =
      std::get<std::shared_ptr<node::BinExprNode>>(
          stmt_let_node->expression->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprPowerNode>>(
      binexpr_node->node));
  std::shared_ptr<node::BinExprPowerNode> binexpr_power_node =
      std::get<std::shared_ptr<node::BinExprPowerNode>>(binexpr_node->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_power_node->left->node));
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_power_node->right->node));

  std::shared_ptr<node::TermNode> left_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_power_node->left->node);
  std::shared_ptr<node::TermNode> right_term =
      std::get<std::shared_ptr<node::TermNode>>(
          binexpr_power_node->right->node);

  TEST_INT_LIT_TERM(left_term->node, 2);
  TEST_INT_LIT_TERM(right_term->node, 3);
}

TEST(Parser, BinExprMod) {
  std::istringstream iss{"let x = 2 % 3;"};
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
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::BinExprNode> binexpr_node =
      std::get<std::shared_ptr<node::BinExprNode>>(
          stmt_let_node->expression->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprModNode>>(
      binexpr_node->node));
  std::shared_ptr<node::BinExprModNode> binexpr_mod_node =
      std::get<std::shared_ptr<node::BinExprModNode>>(binexpr_node->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_mod_node->left->node));
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_mod_node->right->node));

  std::shared_ptr<node::TermNode> left_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_mod_node->left->node);
  std::shared_ptr<node::TermNode> right_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_mod_node->right->node);

  TEST_INT_LIT_TERM(left_term->node, 2);
  TEST_INT_LIT_TERM(right_term->node, 3);
}

TEST(Parser, BinExprDiv) {
  std::istringstream iss{"let x = 2 / 3;"};
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
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::BinExprNode> binexpr_node =
      std::get<std::shared_ptr<node::BinExprNode>>(
          stmt_let_node->expression->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprDivNode>>(
      binexpr_node->node));
  std::shared_ptr<node::BinExprDivNode> binexpr_div_node =
      std::get<std::shared_ptr<node::BinExprDivNode>>(binexpr_node->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_div_node->left->node));
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_div_node->right->node));

  std::shared_ptr<node::TermNode> left_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_div_node->left->node);
  std::shared_ptr<node::TermNode> right_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_div_node->right->node);

  TEST_INT_LIT_TERM(left_term->node, 2);
  TEST_INT_LIT_TERM(right_term->node, 3);
}

TEST(Parser, BinExprMul) {
  std::istringstream iss{"let x = 2 * 3;"};
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
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::BinExprNode> binexpr_node =
      std::get<std::shared_ptr<node::BinExprNode>>(
          stmt_let_node->expression->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprMulNode>>(
      binexpr_node->node));
  std::shared_ptr<node::BinExprMulNode> binexpr_mul_node =
      std::get<std::shared_ptr<node::BinExprMulNode>>(binexpr_node->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_mul_node->left->node));
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_mul_node->right->node));

  std::shared_ptr<node::TermNode> left_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_mul_node->left->node);
  std::shared_ptr<node::TermNode> right_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_mul_node->right->node);

  TEST_INT_LIT_TERM(left_term->node, 2);
  TEST_INT_LIT_TERM(right_term->node, 3);
}

TEST(Parser, BinExprPlus) {
  std::istringstream iss{"let x = 2 + 3;"};
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
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::BinExprNode> binexpr_node =
      std::get<std::shared_ptr<node::BinExprNode>>(
          stmt_let_node->expression->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprPlusNode>>(
      binexpr_node->node));
  std::shared_ptr<node::BinExprPlusNode> binexpr_plus_node =
      std::get<std::shared_ptr<node::BinExprPlusNode>>(binexpr_node->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_plus_node->left->node));
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_plus_node->right->node));

  std::shared_ptr<node::TermNode> left_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_plus_node->left->node);
  std::shared_ptr<node::TermNode> right_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_plus_node->right->node);

  TEST_INT_LIT_TERM(left_term->node, 2);
  TEST_INT_LIT_TERM(right_term->node, 3);
}

TEST(Parser, BinExprMinus) {
  std::istringstream iss{"let x = 2 - 3;"};
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
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::BinExprNode> binexpr_node =
      std::get<std::shared_ptr<node::BinExprNode>>(
          stmt_let_node->expression->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprMinusNode>>(
      binexpr_node->node));
  std::shared_ptr<node::BinExprMinusNode> binexpr_minus_node =
      std::get<std::shared_ptr<node::BinExprMinusNode>>(binexpr_node->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_minus_node->left->node));
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_minus_node->right->node));

  std::shared_ptr<node::TermNode> left_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_minus_node->left->node);
  std::shared_ptr<node::TermNode> right_term =
      std::get<std::shared_ptr<node::TermNode>>(
          binexpr_minus_node->right->node);

  TEST_INT_LIT_TERM(left_term->node, 2);
  TEST_INT_LIT_TERM(right_term->node, 3);
}

TEST(Parser, BinExprGT) {
  std::istringstream iss{"let x = 2 > 3;"};
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
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::BinExprNode> binexpr_node =
      std::get<std::shared_ptr<node::BinExprNode>>(
          stmt_let_node->expression->node);

  ASSERT_TRUE(
      std::holds_alternative<std::shared_ptr<node::BinExprGreaterThanNode>>(
          binexpr_node->node));
  std::shared_ptr<node::BinExprGreaterThanNode> binexpr_gt_node =
      std::get<std::shared_ptr<node::BinExprGreaterThanNode>>(
          binexpr_node->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_gt_node->left->node));
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_gt_node->right->node));

  std::shared_ptr<node::TermNode> left_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_gt_node->left->node);
  std::shared_ptr<node::TermNode> right_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_gt_node->right->node);

  TEST_INT_LIT_TERM(left_term->node, 2);
  TEST_INT_LIT_TERM(right_term->node, 3);
}

TEST(Parser, BinExprLT) {
  std::istringstream iss{"let x = 2 < 3;"};
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
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(
      stmt_let_node->expression->node));
  std::shared_ptr<node::BinExprNode> binexpr_node =
      std::get<std::shared_ptr<node::BinExprNode>>(
          stmt_let_node->expression->node);

  ASSERT_TRUE(
      std::holds_alternative<std::shared_ptr<node::BinExprLessThanNode>>(
          binexpr_node->node));
  std::shared_ptr<node::BinExprLessThanNode> binexpr_lt_node =
      std::get<std::shared_ptr<node::BinExprLessThanNode>>(binexpr_node->node);

  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_lt_node->left->node));
  ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(
      binexpr_lt_node->right->node));

  std::shared_ptr<node::TermNode> left_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_lt_node->left->node);
  std::shared_ptr<node::TermNode> right_term =
      std::get<std::shared_ptr<node::TermNode>>(binexpr_lt_node->right->node);

  TEST_INT_LIT_TERM(left_term->node, 2);
  TEST_INT_LIT_TERM(right_term->node, 3);
}