//! @file parser.hpp
//! @brief Pimento parser
//! @author Logan Thomas

#pragma once

#include <pimento/tokens.hpp>
#include <variant>
#include <vector>

namespace pimento::ast::node {

struct ExprNode;
struct IfPredNode;
struct ScopeNode;

struct TermExprNode {
  std::unique_ptr<ExprNode> expression;
};

struct TermIdentNode {
  tokenization::Token identifier_token;
};

struct TermIntLitNode {
  tokenization::Token int_lit_token;
};

struct BinExprMinusNode {
  std::unique_ptr<ExprNode> left;
  std::unique_ptr<ExprNode> right;
};

struct BinExprPlusNode {
  std::unique_ptr<ExprNode> left;
  std::unique_ptr<ExprNode> right;
};

struct BinExprDivNode {
  std::unique_ptr<ExprNode> left;
  std::unique_ptr<ExprNode> right;
};

struct BinExprMulNode {
  std::unique_ptr<ExprNode> left;
  std::unique_ptr<ExprNode> right;
};

struct BinExprModNode {
  std::unique_ptr<ExprNode> left;
  std::unique_ptr<ExprNode> right;
};

struct BinExprPowerNode {
  std::unique_ptr<ExprNode> left;
  std::unique_ptr<ExprNode> right;
};

struct IfPredElseNode {
  std::unique_ptr<ScopeNode> scope;
};

struct IfPredElifNode {
  std::unique_ptr<ExprNode> expression;
  std::unique_ptr<ScopeNode> scope;
  std::unique_ptr<IfPredNode> ifpred;
};

struct StmtExitNode {
  std::unique_ptr<ExprNode> expression;
};

struct StmtLetNode {
  tokenization::Token identifier;
  std::unique_ptr<ExprNode> expression;
};

struct StmtAssignNode {
  tokenization::Token identifier;
  std::unique_ptr<ExprNode> expression;
};

struct StmtIfNode {
  std::unique_ptr<ExprNode> expression;
  std::unique_ptr<ScopeNode> scope;
  std::unique_ptr<IfPredNode> ifpred;
};

struct TermNode {
  std::variant<std::unique_ptr<TermIntLitNode>, std::unique_ptr<TermIdentNode>,
               std::unique_ptr<TermExprNode>>
      node;
};

struct BinExprNode {
  std::variant<
      std::unique_ptr<BinExprPowerNode>, std::unique_ptr<BinExprModNode>,
      std::unique_ptr<BinExprMulNode>, std::unique_ptr<BinExprDivNode>,
      std::unique_ptr<BinExprPlusNode>, std::unique_ptr<BinExprMinusNode>>
      node;
};

struct ExprNode {
  std::variant<std::unique_ptr<TermNode>, std::unique_ptr<BinExprNode>> node;
};

struct IfPredNode {
  std::variant<std::unique_ptr<IfPredElifNode>, std::unique_ptr<IfPredElseNode>>
      node;
};

struct StmtNode {
  std::variant<std::unique_ptr<StmtExitNode>, std::unique_ptr<StmtLetNode>,
               std::unique_ptr<StmtAssignNode>, std::unique_ptr<StmtIfNode>,
               std::unique_ptr<ScopeNode>>
      node;
};

struct ScopeNode {
  std::vector<std::unique_ptr<StmtNode>> statements;
};

struct ProgNode {
  std::vector<std::unique_ptr<StmtNode>> statements;
};

} // namespace pimento::ast::node