//! @file ast.hpp
//! @brief Pimento AST.
//! @author Logan Thomas

#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include <pimento/tokens.hpp>

namespace pimento::ast::node {

struct ExprNode;
struct IfPredNode;
struct ScopeNode;

//! @brief The Term Expression AST node.
struct TermExprNode
{
    //! @brief The expression of this node.
    std::shared_ptr<ExprNode> expression;

    //! @brief Constructor for the Term Expression AST node.
    //! @param expression The expression of this node.
    explicit TermExprNode(std::shared_ptr<ExprNode> expression)
        : expression(std::move(expression))
    {}
};

//! @brief The Term Identifier AST node.
struct TermIdentNode
{
    //! @brief The identifier token.
    tokenization::Token identifier_token;

    //! @brief Constructor for the Term Identifier AST node.
    //! @param token The identifier token.
    explicit TermIdentNode(tokenization::Token token)
        : identifier_token(std::move(token))
    {}
};

//! @brief The Term Int Literal AST node.
struct TermIntLitNode
{
    //! @brief The int literal token.
    tokenization::Token int_lit_token;

    //! @brief Constructor for the Term Int Literal AST node.
    //! @param token The int literal token.
    explicit TermIntLitNode(tokenization::Token token)
        : int_lit_token(std::move(token))
    {}
};

//! @brief The Binary Expression Less Than Comparison AST node.
struct BinExprLessThanNode
{
    //! @brief The operator's left-side expression.
    std::shared_ptr<ExprNode> left;
    //! @brief The operator's right-side expression.
    std::shared_ptr<ExprNode> right;

    //! @brief Constructor for the Binary Expression Less Than Comparison AST
    //! node.
    //! @param left The operator's left-side expression.
    //! @param right The operator's right-side expression.
    BinExprLessThanNode(std::shared_ptr<ExprNode> left, std::shared_ptr<ExprNode> right)
        : left(std::move(left))
        , right(std::move(right))
    {}
};

//! @brief The Binary Expression Greater Than Comparison AST node.
struct BinExprGreaterThanNode
{
    //! @brief The operator's left-side expression.
    std::shared_ptr<ExprNode> left;
    //! @brief The operator's right-side expression.
    std::shared_ptr<ExprNode> right;

    //! @brief Constructor for the Binary Expression Greater Than Comparison AST
    //! node.
    //! @param left The operator's left-side expression.
    //! @param right The operator's right-side expression.
    BinExprGreaterThanNode(std::shared_ptr<ExprNode> left, std::shared_ptr<ExprNode> right)
        : left(std::move(left))
        , right(std::move(right))
    {}
};

//! @brief The Binary Expression Subtraction AST node.
struct BinExprMinusNode
{
    //! @brief The operator's left-side expression.
    std::shared_ptr<ExprNode> left;
    //! @brief The operator's right-side expression.
    std::shared_ptr<ExprNode> right;

    //! @brief Constructor for the Binary Expression Subtraction AST node.
    //! @param left The operator's left-side expression.
    //! @param right The operator's right-side expression.
    BinExprMinusNode(std::shared_ptr<ExprNode> left, std::shared_ptr<ExprNode> right)
        : left(std::move(left))
        , right(std::move(right))
    {}
};

//! @brief The Binary Expression Addition AST node.
struct BinExprPlusNode
{
    //! @brief The operator's left-side expression.
    std::shared_ptr<ExprNode> left;
    //! @brief The operator's right-side expression.
    std::shared_ptr<ExprNode> right;

    //! @brief Constructor for the Binary Expression Addition AST node.
    //! @param left The operator's left-side expression.
    //! @param right The operator's right-side expression.
    BinExprPlusNode(std::shared_ptr<ExprNode> left, std::shared_ptr<ExprNode> right)
        : left(std::move(left))
        , right(std::move(right))
    {}
};

//! @brief The Binary Expression Division AST node.
struct BinExprDivNode
{
    //! @brief The operator's left-side expression.
    std::shared_ptr<ExprNode> left;
    //! @brief The operator's right-side expression.
    std::shared_ptr<ExprNode> right;

    //! @brief Constructor for the Binary Expression Division AST node.
    //! @param left The operator's left-side expression.
    //! @param right The operator's right-side expression.
    BinExprDivNode(std::shared_ptr<ExprNode> left, std::shared_ptr<ExprNode> right)
        : left(std::move(left))
        , right(std::move(right))
    {}
};

//! @brief The Binary Expression Multiplication AST node.
struct BinExprMulNode
{
    //! @brief The operator's left-side expression.
    std::shared_ptr<ExprNode> left;
    //! @brief The operator's right-side expression.
    std::shared_ptr<ExprNode> right;

    //! @brief Constructor for the Binary Expression Multiplication AST node.
    //! @param left The operator's left-side expression.
    //! @param right The operator's right-side expression.
    BinExprMulNode(std::shared_ptr<ExprNode> left, std::shared_ptr<ExprNode> right)
        : left(std::move(left))
        , right(std::move(right))
    {}
};

//! @brief The Binary Expression Modulus AST node.
struct BinExprModNode
{
    //! @brief The operator's left-side expression.
    std::shared_ptr<ExprNode> left;
    //! @brief The operator's right-side expression.
    std::shared_ptr<ExprNode> right;

    //! @brief Constructor for the Binary Expression Modulus AST node.
    //! @param left The operator's left-side expression.
    //! @param right The operator's right-side expression.
    BinExprModNode(std::shared_ptr<ExprNode> left, std::shared_ptr<ExprNode> right)
        : left(std::move(left))
        , right(std::move(right))
    {}
};

//! @brief The Binary Expression Exponentiation AST node.
struct BinExprPowerNode
{
    //! @brief The operator's left-side expression.
    std::shared_ptr<ExprNode> left;
    //! @brief The operator's right-side expression.
    std::shared_ptr<ExprNode> right;

    //! @brief Constructor for the Binary Expression Exponentiation AST node.
    //! @param left The operator's left-side expression.
    //! @param right The operator's right-side expression.
    BinExprPowerNode(std::shared_ptr<ExprNode> left, std::shared_ptr<ExprNode> right)
        : left(std::move(left))
        , right(std::move(right))
    {}
};

//! @brief The If Predicate Else AST node.
struct IfPredElseNode
{
    //! @brief The scope of this node.
    std::shared_ptr<ScopeNode> scope;

    //! @brief Constructor for the If Predicate Else AST node.
    //! @param scope The scope of this node.
    explicit IfPredElseNode(std::shared_ptr<ScopeNode> scope)
        : scope(std::move(scope))
    {}
};

//! @brief The If Predicate Else If AST node.
struct IfPredElifNode
{
    //! @brief The expression for this node.
    std::shared_ptr<ExprNode> expression;
    //! @brief The scope of this node.
    std::shared_ptr<ScopeNode> scope;
    //! @brief Optional additional If Predicate of this node.
    std::optional<std::shared_ptr<IfPredNode>> ifpred;

    //! @brief Constructor for the If Predicate Else If AST node.
    //! @param expression The Expression for this node.
    //! @param scope The scope of this node.
    //! @param ifpred The optional If Predicate of this node.
    IfPredElifNode(std::shared_ptr<ExprNode> expression,
                   std::shared_ptr<ScopeNode> scope,
                   std::optional<std::shared_ptr<IfPredNode>> ifpred)
        : expression(std::move(expression))
        , scope(std::move(scope))
        , ifpred(std::move(ifpred))
    {}

    //! @brief Constructor for the If Predicate Else If AST node without a chained
    //! If Predicate.
    //! @param expression The Expression for this node.
    //! @param scope The scope of this node.
    IfPredElifNode(std::shared_ptr<ExprNode> expression, std::shared_ptr<ScopeNode> scope)
        : expression(std::move(expression))
        , scope(std::move(scope))
    {}
};

//! @brief The Statement Exit AST node.
struct StmtExitNode
{
    //! @brief The expression of this node.
    std::shared_ptr<ExprNode> expression;

    //! @brief Constructor for the Statement Exit AST node.
    //! @param expression The expression of this node.
    explicit StmtExitNode(std::shared_ptr<ExprNode> expression)
        : expression(std::move(expression))
    {}
};

//! @brief The Statement Let AST node.
struct StmtLetNode
{
    //! @brief The identifier token.
    tokenization::Token identifier;
    //! @brief The expression of this node.
    std::shared_ptr<ExprNode> expression;

    //! @brief Constructor for the Statement Let AST node.
    //! @param identifier The identifier token.
    //! @param expression The expression of this node.
    StmtLetNode(tokenization::Token identifier, std::shared_ptr<ExprNode> expression)
        : identifier(std::move(identifier))
        , expression(std::move(expression))
    {}
};

//! @brief The Statement Assignment AST node.
struct StmtAssignNode
{
    //! @brief The identifier of token.
    tokenization::Token identifier;
    //! @brief The expression of this node.
    std::shared_ptr<ExprNode> expression;

    //! @brief Constructor for the Statement Assignment AST node.
    //! @param identifier The identifier token.
    //! @param expression The expression of this node.
    StmtAssignNode(tokenization::Token identifier, std::shared_ptr<ExprNode> expression)
        : identifier(std::move(identifier))
        , expression(std::move(expression))
    {}
};

//! @brief The Statement If AST node.
struct StmtIfNode
{
    //! @brief The expression for this node.
    std::shared_ptr<ExprNode> expression;
    //! @brief The scope of this node.
    std::shared_ptr<ScopeNode> scope;
    //! @brief Optional additional If Predicate of this node.
    std::optional<std::shared_ptr<IfPredNode>> ifpred;

    //! @brief Constructor for the Statement If AST node.
    //! @param expression The Expression for this node.
    //! @param scope The scope of this node.
    //! @param ifpred The optional If Predicate of this node.
    StmtIfNode(std::shared_ptr<ExprNode> expression,
               std::shared_ptr<ScopeNode> scope,
               std::optional<std::shared_ptr<IfPredNode>> ifpred)
        : expression(std::move(expression))
        , scope(std::move(scope))
        , ifpred(std::move(ifpred))
    {}

    //! @brief Constructor for the Statement If AST node without a chained If
    //! Predicate.
    //! @param expression The Expression for this node.
    //! @param scope The scope of this node.
    StmtIfNode(std::shared_ptr<ExprNode> expression, std::shared_ptr<ScopeNode> scope)
        : expression(std::move(expression))
        , scope(std::move(scope))
    {}
};

//! @brief The Statement While AST node.
struct StmtWhileNode
{
    //! @brief The expression for this node.
    std::shared_ptr<ExprNode> expression;
    //! @brief The scope of this node.
    std::shared_ptr<ScopeNode> scope;

    //! @brief Constructor for the Statement While AST node.
    //! @param expression The Expression for this node.
    //! @param scope The scope of this node.
    StmtWhileNode(std::shared_ptr<ExprNode> expression, std::shared_ptr<ScopeNode> scope)
        : expression(std::move(expression))
        , scope(std::move(scope))
    {}
};

//! @brief The Term AST node.
struct TermNode
{
    using TermVariant
        = std::variant<std::shared_ptr<TermIntLitNode>, std::shared_ptr<TermIdentNode>, std::shared_ptr<TermExprNode>>;

    //! @brief The variant of the Term in this node.
    TermVariant node;

    //! @brief Constructor for the Term AST node.
    //! @param node The variant of the Term in this node.
    explicit TermNode(TermVariant node)
        : node(std::move(node))
    {}
};

//! @brief The Binary Expression AST node.
struct BinExprNode
{
    using BinExprVariant = std::variant<std::shared_ptr<BinExprPowerNode>,
                                        std::shared_ptr<BinExprModNode>,
                                        std::shared_ptr<BinExprMulNode>,
                                        std::shared_ptr<BinExprDivNode>,
                                        std::shared_ptr<BinExprPlusNode>,
                                        std::shared_ptr<BinExprMinusNode>,
                                        std::shared_ptr<BinExprLessThanNode>,
                                        std::shared_ptr<BinExprGreaterThanNode>>;

    //! @brief The variant of the Binary Expression in this node.
    BinExprVariant node;

    //! @brief Constructor for the Binary Expression AST node.
    //! @param node The variant of the Binary Expression in this node.
    explicit BinExprNode(BinExprVariant node)
        : node(std::move(node))
    {}
};

//! @brief The Expression AST node.
struct ExprNode
{
    using ExprVariant = std::variant<std::shared_ptr<TermNode>, std::shared_ptr<BinExprNode>>;

    //! @brief The variant of the Expression in this node.
    ExprVariant node;

    //! @brief Constructor for the Expression AST node.
    //! @param node The variant of the Expression in this node.
    explicit ExprNode(ExprVariant node)
        : node(std::move(node))
    {}
};

//! @brief The If Predicate AST node.
struct IfPredNode
{
    using IfPredVariant = std::variant<std::shared_ptr<IfPredElifNode>, std::shared_ptr<IfPredElseNode>>;

    //! @brief The variant of the If Predicate in this node.
    IfPredVariant node;

    //! @brief Constructor for the If Predicate AST node.
    //! @param node The variant of the If Predicate in this node.
    explicit IfPredNode(IfPredVariant node)
        : node(std::move(node))
    {}
};

//! @brief The Statement AST node.
struct StmtNode
{
    using StmtVariant = std::variant<std::shared_ptr<StmtExitNode>,
                                     std::shared_ptr<StmtLetNode>,
                                     std::shared_ptr<StmtAssignNode>,
                                     std::shared_ptr<StmtIfNode>,
                                     std::shared_ptr<ScopeNode>,
                                     std::shared_ptr<StmtWhileNode>>;

    //! @brief The variant of the Statement in this node.
    StmtVariant node;

    //! @brief Constructor for the Statement AST node.
    //! @param node The variant of the Statement in this node.
    explicit StmtNode(StmtVariant node)
        : node(std::move(node))
    {}
};

//! @brief The Scope AST node.
struct ScopeNode
{
    //! @brief Vector of Statements contained in this scope.
    std::vector<std::shared_ptr<StmtNode>> statements;
};

//! @brief The Program AST node.
struct ProgNode
{
    //! @brief Vector of Statements in this program.
    std::vector<std::shared_ptr<StmtNode>> statements;
};

}  // namespace pimento::ast::node