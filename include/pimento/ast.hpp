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
  ExprNode *expression;

  ~TermExprNode() { delete expression; }
};

struct TermIdentNode {
  tokenization::Token identifier_token;
};

struct TermIntLitNode {
  tokenization::Token int_lit_token;
};

struct BinExprMinusNode {
  ExprNode *left;
  ExprNode *right;

  ~BinExprMinusNode() {
    delete left;
    delete right;
  }
};

struct BinExprPlusNode {
  ExprNode *left;
  ExprNode *right;

  ~BinExprPlusNode() {
    delete left;
    delete right;
  }
};

struct BinExprDivNode {
  ExprNode *left;
  ExprNode *right;

  ~BinExprDivNode() {
    delete left;
    delete right;
  }
};

struct BinExprMulNode {
  ExprNode *left;
  ExprNode *right;

  ~BinExprMulNode() {
    delete left;
    delete right;
  }
};

struct BinExprModNode {
  ExprNode *left;
  ExprNode *right;

  ~BinExprModNode() {
    delete left;
    delete right;
  }
};

struct BinExprPowerNode {
  ExprNode *left;
  ExprNode *right;

  ~BinExprPowerNode() {
    delete left;
    delete right;
  }
};

struct IfPredElseNode {
  ScopeNode *scope;

  ~IfPredElseNode() { delete scope; }
};

struct IfPredElifNode {
  ExprNode *expression;
  ScopeNode *scope;
  std::optional<IfPredNode *> ifpred;

  ~IfPredElifNode() {
    delete expression;
    delete scope;
    if (ifpred.has_value()) {
      delete ifpred.value();
    }
  }
};

struct StmtExitNode {
  ExprNode *expression;

  ~StmtExitNode() { delete expression; }
};

struct StmtLetNode {
  tokenization::Token identifier;
  ExprNode *expression;

  ~StmtLetNode() { delete expression; }
};

struct StmtAssignNode {
  tokenization::Token identifier;
  ExprNode *expression;

  ~StmtAssignNode() { delete expression; }
};

struct StmtIfNode {
  ExprNode *expression;
  ScopeNode *scope;
  std::optional<IfPredNode *> ifpred;

  ~StmtIfNode() {
    delete expression;
    delete scope;
    if (ifpred.has_value()) {
      delete ifpred.value();
    }
  }
};

struct TermNode {
  std::variant<TermIntLitNode *, TermIdentNode *, TermExprNode *> node;

  ~TermNode() {
    const auto visitor = overloads{[](TermIntLitNode *p) { delete p; },
                                   [](TermIdentNode *p) { delete p; },
                                   [](TermExprNode *p) { delete p; }};

    std::visit(visitor, node);
  }

private:
  template <class... Ts> struct overloads : Ts... { using Ts::operator()...; };
};

struct BinExprNode {
  std::variant<BinExprPowerNode *, BinExprModNode *, BinExprMulNode *,
               BinExprDivNode *, BinExprPlusNode *, BinExprMinusNode *>
      node;

  ~BinExprNode() {
    const auto visitor = overloads{[](BinExprPowerNode *p) { delete p; },
                                   [](BinExprModNode *p) { delete p; },
                                   [](BinExprMulNode *p) { delete p; },
                                   [](BinExprDivNode *p) { delete p; },
                                   [](BinExprPlusNode *p) { delete p; },
                                   [](BinExprMinusNode *p) { delete p; }};

    std::visit(visitor, node);
  }

private:
  template <class... Ts> struct overloads : Ts... { using Ts::operator()...; };
};

struct ExprNode {
  std::variant<TermNode *, BinExprNode *> node;

  ~ExprNode() {
    const auto visitor = overloads{[](TermNode *p) { delete p; },
                                   [](BinExprNode *p) { delete p; }};

    std::visit(visitor, node);
  }

private:
  template <class... Ts> struct overloads : Ts... { using Ts::operator()...; };
};

struct IfPredNode {
  std::variant<IfPredElifNode *, IfPredElseNode *> node;

  ~IfPredNode() {
    const auto visitor = overloads{[](IfPredElifNode *p) { delete p; },
                                   [](IfPredElseNode *p) { delete p; }};

    std::visit(visitor, node);
  }

private:
  template <class... Ts> struct overloads : Ts... { using Ts::operator()...; };
};

struct StmtNode {
  std::variant<StmtExitNode *, StmtLetNode *, StmtAssignNode *, StmtIfNode *,
               ScopeNode *>
      node;

  // ~StmtNode() {
  //   const auto visitor = overloads{
  //       [](StmtExitNode *p) { delete p; }, [](StmtLetNode *p) { delete p; },
  //       [](StmtAssignNode *p) { delete p; }, [](StmtIfNode *p) { delete p; },
  //       [](StmtIfNode *p) { delete p; }};

  //   std::visit(visitor, node);
  // }

private:
  template <class... Ts> struct overloads : Ts... { using Ts::operator()...; };
};

struct ScopeNode {
  std::vector<StmtNode *> statements;

  ~ScopeNode() {
    for (auto p : statements) {
      delete p;
    }
  }
};

struct ProgNode {
  std::vector<StmtNode *> statements;

  ~ProgNode() {
    for (auto p : statements) {
      delete p;
    }
  }
};

} // namespace pimento::ast::node