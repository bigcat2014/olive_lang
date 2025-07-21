//! @file parser.hpp
//! @brief Pimento parser
//! @author Logan Thomas

#pragma once

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
  // std::variant<Expr *> expr;
};

struct StmtLetNode {};

struct StmtIdentNode {};

struct StmtIfNode {};

struct Term {
  std::variant<TermIntLitNode *, TermIdentNode *, TermExprNode *> node;
};

struct BinExpr {
  std::variant<BinExprPowerNode *, BinExprModNode *, BinExprMulNode *,
               BinExprDivNode *, BinExprPlusNode *, BinExprMinusNode *>
      node;
};

struct Expr {
  std::variant<ExprTermNode *, ExprBinExprNode *> node;
};

struct IfPred {
  std::variant<IfPredElifNode *, IfPredElseNode> node;
};

struct Scope {};

struct Stmt {
  std::variant<StmtExitNode *, StmtLetNode *, StmtIdentNode *, StmtIfNode *,
               StmtScopeNode *>
      node;
};

struct ProgNode {
  std::vector<Stmt *> statements;
};

} // namespace pimento::ast::node