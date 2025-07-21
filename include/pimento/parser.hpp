//! @file parser.hpp
//! @brief Pimento parser
//! @author Logan Thomas

#pragma once

#include <optional>
#include <pimento/ast.hpp>
#include <pimento/tokens.hpp>
#include <pimento/utils.hpp>
#include <utility>
#include <vector>

namespace pimento::ast {

//! @brief Token parser.
class Parser {
public:
  //! @brief Construct a new Parser object.
  //! @param tokens std::vector<tokenization::Token> The tokens to parse.
  inline explicit Parser(const std::vector<tokenization::Token> &tokens)
      : m_tokens(tokens) {}

  //! @brief Parse all tokens.
  void parse() {
    while (peek().has_value()) {
      node::StmtNode *stmt = parse_statement();
      m_prog.statements.push_back(stmt);
    }
  }

  [[nodiscard]] const node::ProgNode &get_program() const noexcept {
    return m_prog;
  }

private:
  //! @brief Peek at a token at an offset from the current token in the buffer.
  //! @param current_index size_t The index of the current token in the
  //! buffer.
  //! @param tokens std::vector<tokenization::Token>& The buffer from which to
  //! get the token.
  //! @param lookahead size_t Optional lookahead distance to peek.
  //! @return std::optional<tokenization::Token> The token at `lookahead` offset
  //! from the current index or {} if attempting to peek out of bounds.
  [[nodiscard]] inline std::optional<tokenization::Token>
  peek(size_t lookahead = 0) const noexcept {
    try {
      return m_tokens.at(m_index + lookahead);
    } catch (const std::out_of_range &) {
      return {};
    }
  }

  //! @brief Consume the current token.
  inline void consume() noexcept { m_index++; }

  //! @brief Attempt to consume the next token.
  //! @return std::optional<tokenization::Token> The token that was consumed.
  [[nodiscard]] inline std::optional<tokenization::Token>
  try_consume() noexcept {
    if (auto ret = peek()) {
      consume();
      return ret;
    }
    return {};
  }

  //! @brief Try to consume the next token if it is the provided TokenType.
  //!
  //! Try to consume the next token if it is a specific TokenType. If the token
  //! type matches, consume it, otherwise log an error and exit.
  //! @param token_type tokenization::TokenType The token type we are asserting
  //! is next.
  inline tokenization::Token
  try_consume(tokenization::TokenType token_type) noexcept {
    auto next = try_consume();
    if (!next.has_value() || next.value().token_type != token_type) {
      // TODO(lthomas): I don't know if I want to log and exit here
      auto &logger = utils::get_logger();
      logger.error("Expected `{}` at TODO Line & Column number",
                   tokenization::TokenTypeUtil::get_token_str(token_type));
      exit(EXIT_FAILURE);
    }
    return next.value();
  }

  //! @brief Parse a statement into the AST.
  //! @return std::unique_ptr<node::StmtNode> The Statement node of the AST.
  node::StmtNode *parse_statement() {
    auto &logger = utils::get_logger();

    node::StmtNode *stmt = new node::StmtNode;
    tokenization::Token current_token;

    if (auto token_opt = peek()) {
      current_token = token_opt.value();
    } else {
      logger.error("Expected statement at TODO Line & Column number");
      exit(EXIT_FAILURE);
    }

    switch (current_token.token_type) {
    // Parse format exit([Expr]);
    case tokenization::TokenType::TT_EXIT: {
      try_consume(tokenization::TokenType::TT_EXIT);
      try_consume(tokenization::TokenType::TT_LEFT_PAREN);

      node::ExprNode *expression = parse_expression();

      try_consume(tokenization::TokenType::TT_RIGHT_PAREN);
      try_consume(tokenization::TokenType::TT_SEMI);

      stmt->node = new node::StmtExitNode{.expression = expression};
      break;
    }
    // Parse format let ident = [Expr];
    case tokenization::TokenType::TT_LET: {
      try_consume(tokenization::TokenType::TT_LET);
      tokenization::Token identifier =
          try_consume(tokenization::TokenType::TT_IDENTIFIER);

      try_consume(tokenization::TokenType::TT_EQUAL);

      node::ExprNode *expression = parse_expression();

      try_consume(tokenization::TokenType::TT_SEMI);

      stmt->node = new node::StmtLetNode{.identifier = identifier,
                                         .expression = expression};
      break;
    }
    // Parse format ident = [Expr];
    case tokenization::TokenType::TT_IDENTIFIER: {
      try_consume(tokenization::TokenType::TT_IDENTIFIER);
      try_consume(tokenization::TokenType::TT_EQUAL);

      node::ExprNode *expression = parse_expression();

      try_consume(tokenization::TokenType::TT_SEMI);

      stmt->node = new node::StmtAssignNode{.identifier = current_token,
                                            .expression = expression};
      break;
    }
    // Parse format if ([Expr]) [Scope] [IfPred]
    case tokenization::TokenType::TT_IF: {
      try_consume(tokenization::TokenType::TT_IF);
      // try_consume(tokenization::TokenType::TT_LEFT_PAREN);

      node::ExprNode *expression = parse_expression();

      // try_consume(tokenization::TokenType::TT_RIGHT_PAREN);

      node::ScopeNode *scope = parse_scope();

      std::optional<node::IfPredNode *> ifpred = parse_ifpred();

      stmt->node = new node::StmtIfNode{
          .expression = expression, .scope = scope, .ifpred = ifpred};

      break;
    }
    // Parse format {[Stmt]*}
    case tokenization::TokenType::TT_LEFT_CURLY: {
      stmt->node = parse_scope();
      break;
    }
    default: {
      logger.error("Expected statement at TODO Line & Column number");
      exit(EXIT_FAILURE);
    }
    }
    return stmt;
  }

  //! @brief Parse an expression into the AST.
  //! @return std::unique_ptr<node::ExprNode> The expression node of the AST.
  node::ExprNode *parse_expression(uint8_t min_precedence = 0) {
    auto &logger = utils::get_logger();
    node::ExprNode *expr = new node::ExprNode;

    node::TermNode *term_lhs = parse_term();

    while (std::optional<tokenization::Token> next = peek()) {
      tokenization::Token current_token = next.value();

      // TODO(lthomas): I don't like this logic here. Lots of repetitive code
      switch (current_token.token_type) {
      case tokenization::TokenType::TT_DOUBLE_CARET: {
        try_consume(tokenization::TokenType::TT_DOUBLE_CARET);
        std::pair<uint8_t, tokenization::Associativity> properties =
            tokenization::TokenTypeUtil::get_bin_expr_properties(
                current_token.token_type);

        uint8_t next_prec =
            properties.second == tokenization::Associativity::LEFT
                ? properties.first + 1
                : properties.first;

        node::ExprNode *lhs = new node::ExprNode{.node = term_lhs};
        node::BinExprPowerNode *bin_expr = new node::BinExprPowerNode{
            .left = lhs, .right = parse_expression()};
        expr->node = new node::BinExprNode{.node = bin_expr};
        break;
      }
      case tokenization::TokenType::TT_PERCENT: {
        try_consume(tokenization::TokenType::TT_PERCENT);
        std::pair<uint8_t, tokenization::Associativity> properties =
            tokenization::TokenTypeUtil::get_bin_expr_properties(
                current_token.token_type);

        uint8_t next_prec =
            properties.second == tokenization::Associativity::LEFT
                ? properties.first + 1
                : properties.first;

        node::ExprNode *lhs = new node::ExprNode{.node = term_lhs};
        node::BinExprModNode *bin_expr =
            new node::BinExprModNode{.left = lhs, .right = parse_expression()};
        expr->node = new node::BinExprNode{.node = bin_expr};
        break;
      }
      case tokenization::TokenType::TT_STAR: {
        try_consume(tokenization::TokenType::TT_STAR);
        std::pair<uint8_t, tokenization::Associativity> properties =
            tokenization::TokenTypeUtil::get_bin_expr_properties(
                current_token.token_type);

        uint8_t next_prec =
            properties.second == tokenization::Associativity::LEFT
                ? properties.first + 1
                : properties.first;

        node::ExprNode *lhs = new node::ExprNode{.node = term_lhs};
        node::BinExprMulNode *bin_expr =
            new node::BinExprMulNode{.left = lhs, .right = parse_expression()};
        expr->node = new node::BinExprNode{.node = bin_expr};
        break;
      }
      case tokenization::TokenType::TT_FORWARD_SLASH: {
        try_consume(tokenization::TokenType::TT_FORWARD_SLASH);
        std::pair<uint8_t, tokenization::Associativity> properties =
            tokenization::TokenTypeUtil::get_bin_expr_properties(
                current_token.token_type);

        uint8_t next_prec =
            properties.second == tokenization::Associativity::LEFT
                ? properties.first + 1
                : properties.first;

        node::ExprNode *lhs = new node::ExprNode{.node = term_lhs};
        node::BinExprDivNode *bin_expr =
            new node::BinExprDivNode{.left = lhs, .right = parse_expression()};
        expr->node = new node::BinExprNode{.node = bin_expr};
        break;
      }
      case tokenization::TokenType::TT_PLUS: {
        try_consume(tokenization::TokenType::TT_PLUS);
        std::pair<uint8_t, tokenization::Associativity> properties =
            tokenization::TokenTypeUtil::get_bin_expr_properties(
                current_token.token_type);

        uint8_t next_prec =
            properties.second == tokenization::Associativity::LEFT
                ? properties.first + 1
                : properties.first;

        node::ExprNode *lhs = new node::ExprNode{.node = term_lhs};
        node::BinExprPlusNode *bin_expr =
            new node::BinExprPlusNode{.left = lhs, .right = parse_expression()};
        expr->node = new node::BinExprNode{.node = bin_expr};
        break;
      }
      case tokenization::TokenType::TT_MINUS: {
        try_consume(tokenization::TokenType::TT_MINUS);
        std::pair<uint8_t, tokenization::Associativity> properties =
            tokenization::TokenTypeUtil::get_bin_expr_properties(
                current_token.token_type);

        uint8_t next_prec =
            properties.second == tokenization::Associativity::LEFT
                ? properties.first + 1
                : properties.first;

        node::ExprNode *lhs = new node::ExprNode{.node = term_lhs};
        node::BinExprMinusNode *bin_expr = new node::BinExprMinusNode{
            .left = lhs, .right = parse_expression()};
        expr->node = new node::BinExprNode{.node = bin_expr};
        break;
      }
      case tokenization::TokenType::TT_LT: {
        try_consume(tokenization::TokenType::TT_LT);
        std::pair<uint8_t, tokenization::Associativity> properties =
            tokenization::TokenTypeUtil::get_bin_expr_properties(
                current_token.token_type);

        uint8_t next_prec =
            properties.second == tokenization::Associativity::LEFT
                ? properties.first + 1
                : properties.first;

        node::ExprNode *lhs = new node::ExprNode{.node = term_lhs};
        node::BinExprLessThanNode *bin_expr = new node::BinExprLessThanNode{
            .left = lhs, .right = parse_expression()};
        expr->node = new node::BinExprNode{.node = bin_expr};
        break;
      }
      case tokenization::TokenType::TT_GT: {
        try_consume(tokenization::TokenType::TT_GT);
        std::pair<uint8_t, tokenization::Associativity> properties =
            tokenization::TokenTypeUtil::get_bin_expr_properties(
                current_token.token_type);

        uint8_t next_prec =
            properties.second == tokenization::Associativity::LEFT
                ? properties.first + 1
                : properties.first;

        node::ExprNode *lhs = new node::ExprNode{.node = term_lhs};
        node::BinExprGreaterThanNode *bin_expr =
            new node::BinExprGreaterThanNode{.left = lhs,
                                             .right = parse_expression()};
        expr->node = new node::BinExprNode{.node = bin_expr};
        break;
      }
      default: {
        expr->node = term_lhs;
      }
      }

      return expr;
    }
  }

  //! @brief Parse a scope into the AST.
  //! @return std::unique_ptr<node::ScopeNode> The scope node of the AST.
  node::ScopeNode *parse_scope() {
    try_consume(tokenization::TokenType::TT_LEFT_CURLY);

    node::ScopeNode *scope = new node::ScopeNode;
    while (true) {
      std::optional<tokenization::Token> next = peek();
      if (!next.has_value() ||
          next.value().token_type == tokenization::TokenType::TT_RIGHT_CURLY) {
        break;
      }

      node::StmtNode *statement = parse_statement();
      scope->statements.push_back(statement);
    }

    try_consume(tokenization::TokenType::TT_RIGHT_CURLY);
    return scope;
  }

  // TODO(lthomas): Fill out this function body.
  //! @brief Parse an if predicate into the AST.
  //! @return std::unique_ptr<node::IfPred> The if predicate node of the AST.
  std::optional<node::IfPredNode *> parse_ifpred() {
    auto &logger = utils::get_logger();
    std::optional<node::IfPredNode *> ifpred;
    tokenization::Token current_token;

    if (auto token_opt = peek()) {
      current_token = token_opt.value();
    } else {
      logger.error("Expected term at TODO Line & Column number");
      exit(EXIT_FAILURE);
    }

    switch (current_token.token_type) {
    // Parse format ident;
    case tokenization::TokenType::TT_ELIF: {
      try_consume(tokenization::TokenType::TT_ELIF);
      node::ExprNode *expression = parse_expression();
      node::ScopeNode *scope = parse_scope();
      std::optional<node::IfPredNode *> if_pred_elif = parse_ifpred();
      ifpred = new node::IfPredNode{
          .node = new node::IfPredElifNode{.expression = expression,
                                           .scope = scope,
                                           .ifpred = if_pred_elif}};
      break;
    }
    // Parse format int_lit
    case tokenization::TokenType::TT_ELSE: {
      try_consume(tokenization::TokenType::TT_ELSE);
      node::ScopeNode *scope = parse_scope();
      ifpred = new node::IfPredNode{
          .node = new node::IfPredElseNode{.scope = scope}};
      break;
    }
    default:
      ifpred = {};
    }

    return ifpred;
  }

  //! @brief Parse a term into the AST.
  //! @return std::unique_ptr<node::TermNode> The term node of the AST.
  node::TermNode *parse_term() {
    auto &logger = utils::get_logger();
    node::TermNode *term = new node::TermNode;
    tokenization::Token current_token;

    if (auto token_opt = try_consume()) {
      current_token = token_opt.value();
    } else {
      logger.error("Expected term at TODO Line & Column number");
      exit(EXIT_FAILURE);
    }

    switch (current_token.token_type) {
    // Parse format ident;
    case tokenization::TokenType::TT_IDENTIFIER: {
      term->node = new node::TermIdentNode{.identifier_token = current_token};
      break;
    }
    // Parse format int_lit
    case tokenization::TokenType::TT_INT_LITERAL: {
      term->node = new node::TermIntLitNode{.int_lit_token = current_token};
      break;
    }
    // Parse format ident = ([Expr]);
    case tokenization::TokenType::TT_LEFT_PAREN: {
      node::ExprNode *expression = parse_expression();
      try_consume(tokenization::TokenType::TT_RIGHT_PAREN);

      term->node = new node::TermExprNode{.expression = expression};
      break;
    }
    default:
      logger.error("Expected term at TODO Line & Column number");
      exit(EXIT_FAILURE);
    }

    return term;
  }

private:
  //! @brief The current parsing index
  size_t m_index{0};
  //! @brief The tokens to parse.
  const std::vector<tokenization::Token> m_tokens;
  //! @brief The root node of the ast
  node::ProgNode m_prog;
};

} // namespace pimento::ast