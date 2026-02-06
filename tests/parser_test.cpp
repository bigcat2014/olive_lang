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

TEST(Parser, EmptyStream)
{
    std::istringstream iss;
    Parser parser(iss);

    EXPECT_TRUE(parser.getProgram().statements.empty());
}

TEST(Parser, StmtExit)
{
    std::istringstream iss{"exit(0);"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Exit AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtExitNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtExitNode> stmtExitNode
        = std::get<std::shared_ptr<node::StmtExitNode>>(parser.getProgram().statements[0]->node);

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtExitNode->expression->node));
    std::shared_ptr<node::TermNode> termNode
        = std::get<std::shared_ptr<node::TermNode>>(stmtExitNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 0);
}

TEST(Parser, StmtLet)
{
    std::istringstream iss{"let x = 1;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::TermNode> termNode = std::get<std::shared_ptr<node::TermNode>>(stmtLetNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 1);
}

TEST(Parser, StmtAssign)
{
    std::istringstream iss{"x = 1;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Assign AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtAssignNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtAssignNode> stmtAssignNode
        = std::get<std::shared_ptr<node::StmtAssignNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmt_assign_node->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtAssignNode->expression->node));
    std::shared_ptr<node::TermNode> termNode
        = std::get<std::shared_ptr<node::TermNode>>(stmtAssignNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 1);
}

TEST(Parser, StmtWhile)
{
    std::istringstream iss{"while 0 {\n    let x = 1;\n}"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement While AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtWhileNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtWhileNode> stmtWhileNode
        = std::get<std::shared_ptr<node::StmtWhileNode>>(parser.getProgram().statements[0]->node);

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtWhileNode->expression->node));
    std::shared_ptr<node::TermNode> termNode
        = std::get<std::shared_ptr<node::TermNode>>(stmtWhileNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 0);

    EXPECT_EQ(stmtWhileNode->scope->statements.size(), 1);
}

TEST(Parser, Scope)
{
    std::istringstream iss{"{\n    let x = 1;\n}"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Scope AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::ScopeNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::ScopeNode> scopeNode
        = std::get<std::shared_ptr<node::ScopeNode>>(parser.getProgram().statements[0]->node);

    ASSERT_EQ(scopeNode->statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(scopeNode->statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(scopeNode->statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::TermNode> termNode = std::get<std::shared_ptr<node::TermNode>>(stmtLetNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 1);
}

TEST(Parser, StmtIf)
{
    std::istringstream iss{"if 0 {\n    let x = 1;\n}"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement If AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtIfNode> stmtIfNode
        = std::get<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node);

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node));
    std::shared_ptr<node::TermNode> termNode = std::get<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 0);

    EXPECT_EQ(stmtIfNode->scope->statements.size(), 1);
    EXPECT_FALSE(stmtIfNode->ifpred.has_value());
}

TEST(Parser, IfPredElif)
{
    std::istringstream iss{"if 0 {\n    let x = 1;\n} elif 2 {\n    let x = 3;\n}"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement If AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtIfNode> stmtIfNode
        = std::get<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node);

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node));
    std::shared_ptr<node::TermNode> termNode = std::get<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 0);

    EXPECT_EQ(stmtIfNode->scope->statements.size(), 1);

    ASSERT_TRUE(stmtIfNode->ifpred.has_value());
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(stmtIfNode->ifpred.value()->node));
    std::shared_ptr<node::IfPredElifNode> ifpredElifNode
        = std::get<std::shared_ptr<node::IfPredElifNode>>(stmtIfNode->ifpred.value()->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(ifpredElifNode->expression->node));
    std::shared_ptr<node::TermNode> elifTermNode
        = std::get<std::shared_ptr<node::TermNode>>(ifpredElifNode->expression->node);

    //   TEST_INT_LIT_TERM(elifTermNode->node, 2);

    EXPECT_EQ(ifpredElifNode->scope->statements.size(), 1);

    EXPECT_FALSE(ifpredElifNode->ifpred.has_value());
}

TEST(Parser, IfPredElse)
{
    std::istringstream iss{"if 0 {\n    let x = 1;\n} else {\n    let x = 3;\n}"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement If AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtIfNode> stmtIfNode
        = std::get<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node);

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node));
    std::shared_ptr<node::TermNode> termNode = std::get<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 0);

    EXPECT_EQ(stmtIfNode->scope->statements.size(), 1);

    ASSERT_TRUE(stmtIfNode->ifpred.has_value());
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElseNode>>(stmtIfNode->ifpred.value()->node));
    std::shared_ptr<node::IfPredElseNode> ifpredElseNode
        = std::get<std::shared_ptr<node::IfPredElseNode>>(stmtIfNode->ifpred.value()->node);

    EXPECT_EQ(ifpredElseNode->scope->statements.size(), 1);
}

TEST(Parser, IfPredElifElse)
{
    std::istringstream iss{
        "if 0 {\n    let x = 1;\n} elif 2 {\n    let x = "
        "3;\n} else {\n    let x = 4;\n}"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement If AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtIfNode> stmtIfNode
        = std::get<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node);

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node));
    std::shared_ptr<node::TermNode> termNode = std::get<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 0);

    EXPECT_EQ(stmtIfNode->scope->statements.size(), 1);

    ASSERT_TRUE(stmtIfNode->ifpred.has_value());
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(stmtIfNode->ifpred.value()->node));
    std::shared_ptr<node::IfPredElifNode> ifpredElifNode
        = std::get<std::shared_ptr<node::IfPredElifNode>>(stmtIfNode->ifpred.value()->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(ifpredElifNode->expression->node));
    std::shared_ptr<node::TermNode> elifTermNode
        = std::get<std::shared_ptr<node::TermNode>>(ifpredElifNode->expression->node);

    //   TEST_INT_LIT_TERM(elifTermNode->node, 2);

    EXPECT_EQ(ifpredElifNode->scope->statements.size(), 1);

    ASSERT_TRUE(ifpredElifNode->ifpred.has_value());
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElseNode>>(ifpredElifNode->ifpred.value()->node));
    std::shared_ptr<node::IfPredElseNode> ifpredElseNode
        = std::get<std::shared_ptr<node::IfPredElseNode>>(ifpredElifNode->ifpred.value()->node);

    EXPECT_EQ(ifpredElseNode->scope->statements.size(), 1);
}

TEST(Parser, IfPredElifElif)
{
    std::istringstream iss{
        "if 0 {\n    let x = 1;\n} elif 2 {\n    let x = "
        "3;\n} elif 4 {\n    let x = 5;\n}"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement If AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtIfNode> stmtIfNode
        = std::get<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node);

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node));
    std::shared_ptr<node::TermNode> termNode = std::get<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 0);

    EXPECT_EQ(stmtIfNode->scope->statements.size(), 1);

    ASSERT_TRUE(stmtIfNode->ifpred.has_value());
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(stmtIfNode->ifpred.value()->node));
    std::shared_ptr<node::IfPredElifNode> ifpredElifNode1
        = std::get<std::shared_ptr<node::IfPredElifNode>>(stmtIfNode->ifpred.value()->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(ifpredElifNode1->expression->node));
    std::shared_ptr<node::TermNode> elifTermNode1
        = std::get<std::shared_ptr<node::TermNode>>(ifpredElifNode1->expression->node);

    //   TEST_INT_LIT_TERM(elifTermNode1->node, 2);

    EXPECT_EQ(ifpredElifNode1->scope->statements.size(), 1);

    ASSERT_TRUE(ifpredElifNode1->ifpred.has_value());
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(ifpredElifNode1->ifpred.value()->node));
    std::shared_ptr<node::IfPredElifNode> ifpredElifNode2
        = std::get<std::shared_ptr<node::IfPredElifNode>>(ifpredElifNode1->ifpred.value()->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(ifpredElifNode2->expression->node));
    std::shared_ptr<node::TermNode> elifTermNode2
        = std::get<std::shared_ptr<node::TermNode>>(ifpredElifNode2->expression->node);

    //   TEST_INT_LIT_TERM(elifTermNode2->node, 4);

    EXPECT_EQ(ifpredElifNode2->scope->statements.size(), 1);
    EXPECT_FALSE(ifpredElifNode2->ifpred.has_value());
}

TEST(Parser, IfPredElifElifElse)
{
    std::istringstream iss{
        "if 0 {\n    let x = 1;\n} elif 2 {\n    let x = "
        "3;\n} elif 4 {\n    let x = 5;\n} else {\n    let x = 6;\n}"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement If AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtIfNode> stmtIfNode
        = std::get<std::shared_ptr<node::StmtIfNode>>(parser.getProgram().statements[0]->node);

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node));
    std::shared_ptr<node::TermNode> termNode = std::get<std::shared_ptr<node::TermNode>>(stmtIfNode->expression->node);

    //   TEST_INT_LIT_TERM(termNode->node, 0);

    EXPECT_EQ(stmtIfNode->scope->statements.size(), 1);

    ASSERT_TRUE(stmtIfNode->ifpred.has_value());
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(stmtIfNode->ifpred.value()->node));
    std::shared_ptr<node::IfPredElifNode> ifpredElifNode1
        = std::get<std::shared_ptr<node::IfPredElifNode>>(stmtIfNode->ifpred.value()->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(ifpredElifNode1->expression->node));
    std::shared_ptr<node::TermNode> elifTermNode1
        = std::get<std::shared_ptr<node::TermNode>>(ifpredElifNode1->expression->node);

    //   TEST_INT_LIT_TERM(elifTermNode1->node, 2);

    EXPECT_EQ(ifpredElifNode1->scope->statements.size(), 1);

    ASSERT_TRUE(ifpredElifNode1->ifpred.has_value());
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElifNode>>(ifpredElifNode1->ifpred.value()->node));
    std::shared_ptr<node::IfPredElifNode> ifpredElifNode2
        = std::get<std::shared_ptr<node::IfPredElifNode>>(ifpredElifNode1->ifpred.value()->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(ifpredElifNode2->expression->node));
    std::shared_ptr<node::TermNode> elifTermNode2
        = std::get<std::shared_ptr<node::TermNode>>(ifpredElifNode2->expression->node);

    //   TEST_INT_LIT_TERM(elifTermNode2->node, 4);

    EXPECT_EQ(ifpredElifNode2->scope->statements.size(), 1);

    ASSERT_TRUE(ifpredElifNode2->ifpred.has_value());
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::IfPredElseNode>>(ifpredElifNode2->ifpred.value()->node));
    std::shared_ptr<node::IfPredElseNode> ifpredElseNode
        = std::get<std::shared_ptr<node::IfPredElseNode>>(ifpredElifNode2->ifpred.value()->node);

    EXPECT_EQ(ifpredElseNode->scope->statements.size(), 1);
}

TEST(Parser, TermIdent)
{
    std::istringstream iss{"let x = y;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::TermNode> termNode = std::get<std::shared_ptr<node::TermNode>>(stmtLetNode->expression->node);

    //   TEST_IDENTIFIER_TERM(termNode->node, "y");
}

TEST(Parser, BinExprPower)
{
    std::istringstream iss{"let x = 2 ^^ 3;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::BinExprNode> binexprNode
        = std::get<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprPowerNode>>(binexprNode->node));
    std::shared_ptr<node::BinExprPowerNode> binexprPowerNode
        = std::get<std::shared_ptr<node::BinExprPowerNode>>(binexprNode->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprPowerNode->left->node));
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprPowerNode->right->node));

    std::shared_ptr<node::TermNode> leftTerm = std::get<std::shared_ptr<node::TermNode>>(binexprPowerNode->left->node);
    std::shared_ptr<node::TermNode> rightTerm
        = std::get<std::shared_ptr<node::TermNode>>(binexprPowerNode->right->node);

    //   TEST_INT_LIT_TERM(leftTerm->node, 2);
    //   TEST_INT_LIT_TERM(rightTerm->node, 3);
}

TEST(Parser, BinExprMod)
{
    std::istringstream iss{"let x = 2 % 3;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::BinExprNode> binexprNode
        = std::get<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprModNode>>(binexprNode->node));
    std::shared_ptr<node::BinExprModNode> binexprModNode
        = std::get<std::shared_ptr<node::BinExprModNode>>(binexprNode->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprModNode->left->node));
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprModNode->right->node));

    std::shared_ptr<node::TermNode> leftTerm  = std::get<std::shared_ptr<node::TermNode>>(binexprModNode->left->node);
    std::shared_ptr<node::TermNode> rightTerm = std::get<std::shared_ptr<node::TermNode>>(binexprModNode->right->node);

    //   TEST_INT_LIT_TERM(leftTerm->node, 2);
    //   TEST_INT_LIT_TERM(rightTerm->node, 3);
}

TEST(Parser, BinExprDiv)
{
    std::istringstream iss{"let x = 2 / 3;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::BinExprNode> binexprNode
        = std::get<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprDivNode>>(binexprNode->node));
    std::shared_ptr<node::BinExprDivNode> binexprDivNode
        = std::get<std::shared_ptr<node::BinExprDivNode>>(binexprNode->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprDivNode->left->node));
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprDivNode->right->node));

    std::shared_ptr<node::TermNode> leftTerm  = std::get<std::shared_ptr<node::TermNode>>(binexprDivNode->left->node);
    std::shared_ptr<node::TermNode> rightTerm = std::get<std::shared_ptr<node::TermNode>>(binexprDivNode->right->node);

    //   TEST_INT_LIT_TERM(leftTerm->node, 2);
    //   TEST_INT_LIT_TERM(rightTerm->node, 3);
}

TEST(Parser, BinExprMul)
{
    std::istringstream iss{"let x = 2 * 3;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::BinExprNode> binexprNode
        = std::get<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprMulNode>>(binexprNode->node));
    std::shared_ptr<node::BinExprMulNode> binexprMulNode
        = std::get<std::shared_ptr<node::BinExprMulNode>>(binexprNode->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprMulNode->left->node));
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprMulNode->right->node));

    std::shared_ptr<node::TermNode> leftTerm  = std::get<std::shared_ptr<node::TermNode>>(binexprMulNode->left->node);
    std::shared_ptr<node::TermNode> rightTerm = std::get<std::shared_ptr<node::TermNode>>(binexprMulNode->right->node);

    //   TEST_INT_LIT_TERM(leftTerm->node, 2);
    //   TEST_INT_LIT_TERM(rightTerm->node, 3);
}

TEST(Parser, BinExprPlus)
{
    std::istringstream iss{"let x = 2 + 3;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::BinExprNode> binexprNode
        = std::get<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprPlusNode>>(binexprNode->node));
    std::shared_ptr<node::BinExprPlusNode> binexprPlusNode
        = std::get<std::shared_ptr<node::BinExprPlusNode>>(binexprNode->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprPlusNode->left->node));
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprPlusNode->right->node));

    std::shared_ptr<node::TermNode> leftTerm  = std::get<std::shared_ptr<node::TermNode>>(binexprPlusNode->left->node);
    std::shared_ptr<node::TermNode> rightTerm = std::get<std::shared_ptr<node::TermNode>>(binexprPlusNode->right->node);

    //   TEST_INT_LIT_TERM(leftTerm->node, 2);
    //   TEST_INT_LIT_TERM(rightTerm->node, 3);
}

TEST(Parser, BinExprMinus)
{
    std::istringstream iss{"let x = 2 - 3;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::BinExprNode> binexprNode
        = std::get<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprMinusNode>>(binexprNode->node));
    std::shared_ptr<node::BinExprMinusNode> binexprMinusNode
        = std::get<std::shared_ptr<node::BinExprMinusNode>>(binexprNode->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprMinusNode->left->node));
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprMinusNode->right->node));

    std::shared_ptr<node::TermNode> leftTerm = std::get<std::shared_ptr<node::TermNode>>(binexprMinusNode->left->node);
    std::shared_ptr<node::TermNode> rightTerm
        = std::get<std::shared_ptr<node::TermNode>>(binexprMinusNode->right->node);

    //   TEST_INT_LIT_TERM(leftTerm->node, 2);
    //   TEST_INT_LIT_TERM(rightTerm->node, 3);
}

TEST(Parser, BinExprGT)
{
    std::istringstream iss{"let x = 2 > 3;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::BinExprNode> binexprNode
        = std::get<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprGreaterThanNode>>(binexprNode->node));
    std::shared_ptr<node::BinExprGreaterThanNode> binexprGTNode
        = std::get<std::shared_ptr<node::BinExprGreaterThanNode>>(binexprNode->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprGTNode->left->node));
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprGTNode->right->node));

    std::shared_ptr<node::TermNode> leftTerm  = std::get<std::shared_ptr<node::TermNode>>(binexprGTNode->left->node);
    std::shared_ptr<node::TermNode> rightTerm = std::get<std::shared_ptr<node::TermNode>>(binexprGTNode->right->node);

    //   TEST_INT_LIT_TERM(leftTerm->node, 2);
    //   TEST_INT_LIT_TERM(rightTerm->node, 3);
}

TEST(Parser, BinExprLT)
{
    std::istringstream iss{"let x = 2 < 3;"};
    Parser parser(iss);

    ASSERT_EQ(parser.getProgram().statements.size(), 1);
    // Ensure node is "Statement Let AST node"
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node));
    std::shared_ptr<node::StmtLetNode> stmtLetNode
        = std::get<std::shared_ptr<node::StmtLetNode>>(parser.getProgram().statements[0]->node);

    // Check the token type and properties are correct
    //   TEST_IDENTIFIER_TOKEN(stmtLetNode->identifier, "x");

    // Check that the expression is correct
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node));
    std::shared_ptr<node::BinExprNode> binexprNode
        = std::get<std::shared_ptr<node::BinExprNode>>(stmtLetNode->expression->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::BinExprLessThanNode>>(binexprNode->node));
    std::shared_ptr<node::BinExprLessThanNode> binexprLTNode
        = std::get<std::shared_ptr<node::BinExprLessThanNode>>(binexprNode->node);

    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprLTNode->left->node));
    ASSERT_TRUE(std::holds_alternative<std::shared_ptr<node::TermNode>>(binexprLTNode->right->node));

    std::shared_ptr<node::TermNode> leftTerm  = std::get<std::shared_ptr<node::TermNode>>(binexprLTNode->left->node);
    std::shared_ptr<node::TermNode> rightTerm = std::get<std::shared_ptr<node::TermNode>>(binexprLTNode->right->node);

    //   TEST_INT_LIT_TERM(leftTerm->node, 2);
    //   TEST_INT_LIT_TERM(rightTerm->node, 3);
}