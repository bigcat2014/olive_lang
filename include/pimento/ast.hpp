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
  std::shared_ptr<ExprNode> expression;
};

struct TermIdentNode {
  tokenization::Token identifier_token;
};

struct TermIntLitNode {
  tokenization::Token int_lit_token;
};

struct BinExprLessThanNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;
};

struct BinExprGreaterThanNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;
};

struct BinExprMinusNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;
};

struct BinExprPlusNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;
};

struct BinExprDivNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;
};

struct BinExprMulNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;
};

struct BinExprModNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;
};

struct BinExprPowerNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;
};

struct IfPredElseNode {
  std::shared_ptr<ScopeNode> scope;

  // ~IfPredElseNode() { delete scope; }
};

struct IfPredElifNode {
  std::shared_ptr<ExprNode> expression;
  std::shared_ptr<ScopeNode> scope;
  std::optional<std::shared_ptr<IfPredNode>> ifpred;
};

struct StmtExitNode {
  std::shared_ptr<ExprNode> expression;
};

struct StmtLetNode {
  tokenization::Token identifier;
  std::shared_ptr<ExprNode> expression;
};

struct StmtAssignNode {
  tokenization::Token identifier;
  std::shared_ptr<ExprNode> expression;
};

struct StmtIfNode {
  std::shared_ptr<ExprNode> expression;
  std::shared_ptr<ScopeNode> scope;
  std::optional<std::shared_ptr<IfPredNode>> ifpred;
};

struct TermNode {
  std::variant<std::shared_ptr<TermIntLitNode>, std::shared_ptr<TermIdentNode>,
               std::shared_ptr<TermExprNode>>
      node;
};

struct BinExprNode {
  std::variant<
      std::shared_ptr<BinExprPowerNode>, std::shared_ptr<BinExprModNode>,
      std::shared_ptr<BinExprMulNode>, std::shared_ptr<BinExprDivNode>,
      std::shared_ptr<BinExprPlusNode>, std::shared_ptr<BinExprMinusNode>,
      std::shared_ptr<BinExprLessThanNode>,
      std::shared_ptr<BinExprGreaterThanNode>>
      node;
};

struct ExprNode {
  std::variant<std::shared_ptr<TermNode>, std::shared_ptr<BinExprNode>> node;
};

struct IfPredNode {
  std::variant<std::shared_ptr<IfPredElifNode>, std::shared_ptr<IfPredElseNode>>
      node;
};

struct StmtNode {
  std::variant<std::shared_ptr<StmtExitNode>, std::shared_ptr<StmtLetNode>,
               std::shared_ptr<StmtAssignNode>, std::shared_ptr<StmtIfNode>,
               std::shared_ptr<ScopeNode>>
      node;
};

struct ScopeNode {
  std::vector<std::shared_ptr<StmtNode>> statements;
};

struct ProgNode {
  std::vector<std::shared_ptr<StmtNode>> statements;
};

} // namespace pimento::ast::node