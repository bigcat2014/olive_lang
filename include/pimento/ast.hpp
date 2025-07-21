//! @file parser.hpp
//! @brief Pimento parser
//! @author Logan Thomas

#pragma once

#include <memory>
#include <pimento/tokens.hpp>
#include <variant>
#include <vector>

namespace pimento::ast::node {

struct TermExprNode {};

struct TermIdentNode {
  tokenization::Token identifier_token;
};

struct TermIntLitNode {
  tokenization::Token int_lit_token;
};

struct TermNode;
struct BinExprNode;
struct ExprNode;
struct IfPredNode;
struct ScopeNode;
struct StmtNode;
struct ProgNode;

struct BinExprMinusNode {};

struct BinExprPlusNode {};

struct BinExprDivNode {};

struct BinExprMulNode {};

struct BinExprModNode {};

struct BinExprPowerNode {};

struct ExprBinExprNode {};

struct ExprTermNode {};

struct IfPredElseNode {};

struct IfPredElifNode {};

struct StmtScopeNode {};

struct StmtExitNode {
  std::unique_ptr<ExprNode> expression;
};

struct StmtLetNode {};

struct StmtAssignNode {};

struct StmtIfNode {};

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
  std::variant<std::unique_ptr<ExprTermNode>, std::unique_ptr<ExprBinExprNode>>
      node;
};

struct IfPredNode {
  std::variant<std::unique_ptr<IfPredElifNode>, IfPredElseNode> node;
};

struct ScopeNode {
  std::vector<std::unique_ptr<StmtNode>> statements;
};

struct StmtNode {
  std::variant<std::unique_ptr<StmtExitNode>, std::unique_ptr<StmtLetNode>,
               std::unique_ptr<StmtAssignNode>, std::unique_ptr<StmtIfNode>,
               std::unique_ptr<StmtScopeNode>>
      node;
};

struct ProgNode {
  std::vector<std::unique_ptr<StmtNode>> statements;
};

} // namespace pimento::ast::node