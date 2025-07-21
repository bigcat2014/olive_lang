//! @file parser.hpp
//! @brief Pimento parser
//! @author Logan Thomas

#pragma once

#include <variant>
#include <vector>

#include <pimento/tokens.hpp>

namespace pimento::ast::node {

struct ExprNode;
struct IfPredNode;
struct ScopeNode;

struct TermExprNode {
  std::shared_ptr<ExprNode> expression;

  TermExprNode(std::shared_ptr<ExprNode> expression) : expression(expression) {}
};

struct TermIdentNode {
  tokenization::Token identifier_token;

  TermIdentNode(tokenization::Token token) : identifier_token(token) {}
};

struct TermIntLitNode {
  tokenization::Token int_lit_token;

  TermIntLitNode(tokenization::Token token) : int_lit_token(token) {}
};

struct BinExprLessThanNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;

  BinExprLessThanNode(std::shared_ptr<ExprNode> left,
                      std::shared_ptr<ExprNode> right)
      : left(left), right(right) {}
};

struct BinExprGreaterThanNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;

  BinExprGreaterThanNode(std::shared_ptr<ExprNode> left,
                         std::shared_ptr<ExprNode> right)
      : left(left), right(right) {}
};

struct BinExprMinusNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;

  BinExprMinusNode(std::shared_ptr<ExprNode> left,
                   std::shared_ptr<ExprNode> right)
      : left(left), right(right) {}
};

struct BinExprPlusNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;

  BinExprPlusNode(std::shared_ptr<ExprNode> left,
                  std::shared_ptr<ExprNode> right)
      : left(left), right(right) {}
};

struct BinExprDivNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;

  BinExprDivNode(std::shared_ptr<ExprNode> left,
                 std::shared_ptr<ExprNode> right)
      : left(left), right(right) {}
};

struct BinExprMulNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;

  BinExprMulNode(std::shared_ptr<ExprNode> left,
                 std::shared_ptr<ExprNode> right)
      : left(left), right(right) {}
};

struct BinExprModNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;

  BinExprModNode(std::shared_ptr<ExprNode> left,
                 std::shared_ptr<ExprNode> right)
      : left(left), right(right) {}
};

struct BinExprPowerNode {
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;

  BinExprPowerNode(std::shared_ptr<ExprNode> left,
                   std::shared_ptr<ExprNode> right)
      : left(left), right(right) {}
};

struct IfPredElseNode {
  std::shared_ptr<ScopeNode> scope;

  IfPredElseNode(std::shared_ptr<ScopeNode> scope) : scope(scope) {}
};

struct IfPredElifNode {
  std::shared_ptr<ExprNode> expression;
  std::shared_ptr<ScopeNode> scope;
  std::optional<std::shared_ptr<IfPredNode>> ifpred;

  IfPredElifNode(std::shared_ptr<ExprNode> expression,
                 std::shared_ptr<ScopeNode> scope,
                 std::optional<std::shared_ptr<IfPredNode>> ifpred)
      : expression(expression), scope(scope), ifpred(ifpred) {}
};

struct StmtExitNode {
  std::shared_ptr<ExprNode> expression;

  StmtExitNode(std::shared_ptr<ExprNode> expression) : expression(expression) {}
};

struct StmtLetNode {
  tokenization::Token identifier;
  std::shared_ptr<ExprNode> expression;

  StmtLetNode(tokenization::Token identifier,
              std::shared_ptr<ExprNode> expression)
      : identifier(identifier), expression(expression) {}
};

struct StmtAssignNode {
  tokenization::Token identifier;
  std::shared_ptr<ExprNode> expression;

  StmtAssignNode(tokenization::Token identifier,
                 std::shared_ptr<ExprNode> expression)
      : identifier(identifier), expression(expression) {}
};

struct StmtIfNode {
  std::shared_ptr<ExprNode> expression;
  std::shared_ptr<ScopeNode> scope;
  std::optional<std::shared_ptr<IfPredNode>> ifpred;

  StmtIfNode(std::shared_ptr<ExprNode> expression,
             std::shared_ptr<ScopeNode> scope,
             std::optional<std::shared_ptr<IfPredNode>> ifpred)
      : expression(expression), scope(scope), ifpred(ifpred) {}
};

struct TermNode {
  using TermVariant = std::variant<std::shared_ptr<TermIntLitNode>,
                                   std::shared_ptr<TermIdentNode>,
                                   std::shared_ptr<TermExprNode>>;

  TermVariant node;

  TermNode(TermVariant node) : node(node) {}
};

struct BinExprNode {
  using BinExprVariant = std::variant<
      std::shared_ptr<BinExprPowerNode>, std::shared_ptr<BinExprModNode>,
      std::shared_ptr<BinExprMulNode>, std::shared_ptr<BinExprDivNode>,
      std::shared_ptr<BinExprPlusNode>, std::shared_ptr<BinExprMinusNode>,
      std::shared_ptr<BinExprLessThanNode>,
      std::shared_ptr<BinExprGreaterThanNode>>;

  BinExprVariant node;

  BinExprNode(BinExprVariant node) : node(node) {}
};

struct ExprNode {
  using ExprVariant =
      std::variant<std::shared_ptr<TermNode>, std::shared_ptr<BinExprNode>>;

  ExprVariant node;

  ExprNode(ExprVariant node) : node(node) {}
};

struct IfPredNode {
  using IfPredVariant = std::variant<std::shared_ptr<IfPredElifNode>,
                                     std::shared_ptr<IfPredElseNode>>;

  IfPredVariant node;

  IfPredNode(IfPredVariant node) : node(node) {}
};

struct StmtNode {
  using StmtVariant =
      std::variant<std::shared_ptr<StmtExitNode>, std::shared_ptr<StmtLetNode>,
                   std::shared_ptr<StmtAssignNode>, std::shared_ptr<StmtIfNode>,
                   std::shared_ptr<ScopeNode>>;

  StmtVariant node;

  StmtNode(StmtVariant node) : node(node) {}
};

struct ScopeNode {
  std::vector<std::shared_ptr<StmtNode>> statements;
};

struct ProgNode {
  std::vector<std::shared_ptr<StmtNode>> statements;
};

} // namespace pimento::ast::node