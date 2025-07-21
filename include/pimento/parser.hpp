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
    while (peek(m_index).has_value()) {
      std::unique_ptr<node::StmtNode> stmt = parse_statement();
      m_prog.statements.emplace_back(std::move(stmt));

      // switch (current_token.token_type) {
      // case tokenization::TokenType::TT_ELSE: {
      //   break;
      // }
      // case tokenization::TokenType::TT_EXIT: {
      //   // Parse format exit([Expr]);
      //   expect_token(tokenization::TokenType::TT_LEFT_PAREN);

      //   node::StmtNode stmt;
      //   if (auto expr = parse_expression()) {
      //     stmt.node = std::make_unique<node::StmtExitNode>(
      //         std::make_unique<node::ExprNode>(std::move(expr.value())));
      //   } else {
      //     logger.error("Expected expression at TODO Line & Column number");
      //     exit(EXIT_FAILURE);
      //   }

      //   expect_token(tokenization::TokenType::TT_RIGHT_PAREN);
      //   expect_token(tokenization::TokenType::TT_SEMI);
      //   break;
      // }
      // case tokenization::TokenType::TT_IF: {
      //   expect_token(tokenization::TokenType::TT_LEFT_PAREN);

      //   if (auto expr = parse_expression()) {
      //     // TODO(lthomas): Fill out node
      //   } else {
      //     logger.error("Expected expression at TODO Line & Column number");
      //     exit(EXIT_FAILURE);
      //   }

      //   expect_token(tokenization::TokenType::TT_RIGHT_PAREN);

      //   if (auto scope = parse_scope()) {
      //     // TODO(lthomas): Fill out node
      //   } else {
      //     logger.error("Expected scope at TODO Line & Column number");
      //     exit(EXIT_FAILURE);
      //   }

      //   // TODO(lthomas): Parse IfPred

      //   break;
      // }
      // case tokenization::TokenType::TT_ELIF: {
      //   break;
      // }
      // case tokenization::TokenType::TT_LET: {
      //   tokenization::Token identifier =
      //       expect_token(tokenization::TokenType::TT_IDENTIFIER);

      //   expect_token(tokenization::TokenType::TT_EQUAL);

      //   if (auto expr = parse_expression()) {
      //     // TODO(lthomas): Fill out node
      //   } else {
      //     logger.error("Expected expression at TODO Line & Column number");
      //     exit(EXIT_FAILURE);
      //   }

      //   expect_token(tokenization::TokenType::TT_SEMI);
      //   break;
      // }
      // case tokenization::TokenType::TT_LEFT_CURLY: {
      //   break;
      // }
      // case tokenization::TokenType::TT_LEFT_PAREN: {
      //   break;
      // }
      // case tokenization::TokenType::TT_RIGHT_CURLY: {
      //   break;
      // }
      // case tokenization::TokenType::TT_RIGHT_PAREN: {
      //   break;
      // }
      // case tokenization::TokenType::TT_DOUBLE_CARET: {
      //   break;
      // }
      // case tokenization::TokenType::TT_FORWARD_SLASH: {
      //   break;
      // }
      // case tokenization::TokenType::TT_MINUS: {
      //   break;
      // }
      // case tokenization::TokenType::TT_PERCENT: {
      //   break;
      // }
      // case tokenization::TokenType::TT_PLUS: {
      //   break;
      // }
      // case tokenization::TokenType::TT_STAR: {
      //   break;
      // }
      // case tokenization::TokenType::TT_EQUAL: {
      //   break;
      // }
      // case tokenization::TokenType::TT_IDENTIFIER: {
      //   expect_token(tokenization::TokenType::TT_EQUAL);

      //   if (auto expr = parse_expression()) {
      //     // TODO(lthomas): Fill out node
      //   } else {
      //     logger.error("Expected expression at TODO Line & Column number");
      //     exit(EXIT_FAILURE);
      //   }

      //   expect_token(tokenization::TokenType::TT_SEMI);
      //   break;
      // }
      // case tokenization::TokenType::TT_INT_LITERAL: {
      //   break;
      // }
      // case tokenization::TokenType::TT_SEMI: {
      //   break;
      // }
      // default: {
      //   throw std::runtime_error("Unknown token");
      // }
      // }

      // logger.trace("Finished parsing token: {}",
      //              tokenization::TokenTypeUtil::get_type_as_str(
      //                  current_token.token_type));
      // consume();
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

  //! @brief Assert the next token to be a specific TokenType.
  //!
  //! Assert the next token to be a specific TokenType. If the token type
  //! matches, consume it, otherwise log an error and exit.
  //! @param token_type tokenization::TokenType The token type we are asserting
  //! is next.
  inline std::pair<bool, tokenization::Token>
  compare_next_token(tokenization::TokenType token_type) noexcept {
    auto next = try_consume();
    if (!next.has_value()) {
      // TODO(lthomas): I don't know if I want to log and exit here
      auto &logger = utils::get_logger();
      logger.error("Expected `{}` at TODO Line & Column number",
                   tokenization::TokenTypeUtil::get_token_str(token_type));
      exit(EXIT_FAILURE);
    }
    if (next.value().token_type != token_type) {
      return std::make_pair<bool, tokenization::Token &>(false, next.value());
    }
    return std::make_pair<bool, tokenization::Token &>(true, next.value());
  }

  //! @brief Parse a statement into the AST.
  //! @return std::unique_ptr<node::StmtNode> The Statement node of the AST.
  std::unique_ptr<node::StmtNode> parse_statement() {
    auto &logger = utils::get_logger();
    std::unique_ptr<node::StmtNode> stmt = std::make_unique<node::StmtNode>();
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

      std::unique_ptr<node::ExprNode> expression = parse_expression();

      try_consume(tokenization::TokenType::TT_RIGHT_PAREN);
      try_consume(tokenization::TokenType::TT_SEMI);

      stmt->node = std::make_unique<node::StmtExitNode>(std::move(expression));
      break;
    }
    // Parse format let ident = [Expr];
    case tokenization::TokenType::TT_LET: {
      try_consume(tokenization::TokenType::TT_LET);
      tokenization::Token identifier =
          try_consume(tokenization::TokenType::TT_IDENTIFIER);

      try_consume(tokenization::TokenType::TT_EQUAL);

      std::unique_ptr<node::ExprNode> expression = parse_expression();

      try_consume(tokenization::TokenType::TT_SEMI);

      stmt->node = std::make_unique<node::StmtLetNode>(identifier,
                                                       std::move(expression));
      break;
    }
    // Parse format ident = [Expr];
    case tokenization::TokenType::TT_IDENTIFIER: {
      try_consume(tokenization::TokenType::TT_IDENTIFIER);
      try_consume(tokenization::TokenType::TT_EQUAL);

      std::unique_ptr<node::ExprNode> expression = parse_expression();

      try_consume(tokenization::TokenType::TT_SEMI);

      stmt->node = std::make_unique<node::StmtAssignNode>(
          current_token, std::move(expression));
      break;
    }
    // Parse format if ([Expr]) [Scope] [IfPred]
    case tokenization::TokenType::TT_IF: {
      try_consume(tokenization::TokenType::TT_IF);
      try_consume(tokenization::TokenType::TT_LEFT_PAREN);

      std::unique_ptr<node::ExprNode> expression = parse_expression();

      try_consume(tokenization::TokenType::TT_RIGHT_PAREN);

      std::unique_ptr<node::ScopeNode> scope = parse_scope();

      // TODO(lthomas): Decide if should handle empty ifpred (valid grammar)
      // here or somewhere else.
      std::unique_ptr<node::IfPredNode> ifpred = parse_ifpred();

      stmt->node = std::make_unique<node::StmtIfNode>(
          std::move(expression), std::move(scope), std::move(ifpred));

      break;
    }
    // Parse format {[Stmt]*}
    case tokenization::TokenType::TT_LEFT_CURLY: {
      try_consume(tokenization::TokenType::TT_LEFT_CURLY);
      stmt->node = parse_scope();
      break;
    }
    default: {
      logger.error("Expected statemetnt at TODO Line & Column number");
      exit(EXIT_FAILURE);
    }
    }
    return stmt;
  }

  // TODO(lthomas): Fill out this function body.
  //! @brief Parse an expression into the AST.
  //! @return std::unique_ptr<node::ExprNode> The expression node of the AST.
  std::unique_ptr<node::ExprNode> parse_expression(uint8_t min_precedence = 0) {
    // Precedence climbing pseudocode
    // result = compute_atom()
    // while cur token is a binary operator with precedence >= min_prec:
    //   prec, assoc = precedence and associativity of current token
    //   if assoc is left:
    //     next_min_prec = prec + 1
    //   else:
    //     next_min_prec = prec
    //   rhs = compute_expr(next_min_prec)
    //   result = compute operator(result, rhs)
    // return result

    auto &logger = utils::get_logger();
    std::unique_ptr<node::ExprNode> expr = std::make_unique<node::ExprNode>();

    std::unique_ptr<node::TermNode> term_lhs = parse_term();

    while (std::optional<tokenization::Token> next = peek()) {
      tokenization::Token current_token = next.value();

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

        std::unique_ptr<node::ExprNode> lhs =
            std::make_unique<node::ExprNode>(std::move(term_lhs));
        std::unique_ptr<node::BinExprPowerNode> bin_expr_power =
            std::make_unique<node::BinExprPowerNode>(
                std::move(lhs), std::move(parse_expression(next_prec)));
        expr->node =
            std::make_unique<node::BinExprNode>(std::move(bin_expr_power));
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

        std::unique_ptr<node::ExprNode> lhs =
            std::make_unique<node::ExprNode>(std::move(term_lhs));
        std::unique_ptr<node::BinExprModNode> bin_expr_power =
            std::make_unique<node::BinExprModNode>(
                std::move(lhs), std::move(parse_expression(next_prec)));
        expr->node =
            std::make_unique<node::BinExprNode>(std::move(bin_expr_power));

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

        std::unique_ptr<node::ExprNode> lhs =
            std::make_unique<node::ExprNode>(std::move(term_lhs));
        std::unique_ptr<node::BinExprMulNode> bin_expr_power =
            std::make_unique<node::BinExprMulNode>(
                std::move(lhs), std::move(parse_expression(next_prec)));
        expr->node =
            std::make_unique<node::BinExprNode>(std::move(bin_expr_power));

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

        std::unique_ptr<node::ExprNode> lhs =
            std::make_unique<node::ExprNode>(std::move(term_lhs));
        std::unique_ptr<node::BinExprDivNode> bin_expr_power =
            std::make_unique<node::BinExprDivNode>(
                std::move(lhs), std::move(parse_expression(next_prec)));
        expr->node =
            std::make_unique<node::BinExprNode>(std::move(bin_expr_power));

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

        std::unique_ptr<node::ExprNode> lhs =
            std::make_unique<node::ExprNode>(std::move(term_lhs));
        std::unique_ptr<node::BinExprPlusNode> bin_expr_power =
            std::make_unique<node::BinExprPlusNode>(
                std::move(lhs), std::move(parse_expression(next_prec)));
        expr->node =
            std::make_unique<node::BinExprNode>(std::move(bin_expr_power));

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

        std::unique_ptr<node::ExprNode> lhs =
            std::make_unique<node::ExprNode>(std::move(term_lhs));
        std::unique_ptr<node::BinExprMinusNode> bin_expr_power =
            std::make_unique<node::BinExprMinusNode>(
                std::move(lhs), std::move(parse_expression(next_prec)));
        expr->node =
            std::make_unique<node::BinExprNode>(std::move(bin_expr_power));

        break;
      }
      default: {
        expr->node = std::move(term_lhs);
      }
      }

      return expr;
    }
  }

  // TODO(lthomas): Fill out this function body.
  //! @brief Parse a scope into the AST.
  //! @return std::unique_ptr<node::ScopeNode> The scope node of the AST.
  std::unique_ptr<node::ScopeNode> parse_scope() {

    std::unique_ptr<node::StmtNode> statement = parse_statement();

    try_consume(tokenization::TokenType::TT_RIGHT_CURLY);
  }

  // TODO(lthomas): Fill out this function body.
  //! @brief Parse an if predicate into the AST.
  //! @return std::unique_ptr<node::IfPred> The if predicate node of the AST.
  std::unique_ptr<node::IfPredNode> parse_ifpred() {
    auto &logger = utils::get_logger();
    logger.error("Expected ifpred at TODO Line & Column number");
    exit(EXIT_FAILURE);
  }

  // TODO(lthomas): Fill out this function body.
  //! @brief Parse a term into the AST.
  //! @return std::unique_ptr<node::TermNode> The term node of the AST.
  std::unique_ptr<node::TermNode> parse_term() {
    auto &logger = utils::get_logger();
    std::unique_ptr<node::TermNode> term = std::make_unique<node::TermNode>();
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
      term->node = std::make_unique<node::TermIdentNode>(current_token);
      break;
    }
    // Parse format int_lit
    case tokenization::TokenType::TT_INT_LITERAL: {
      term->node = std::make_unique<node::TermIntLitNode>(current_token);
      break;
    }
    // Parse format ident = ([Expr]);
    case tokenization::TokenType::TT_LEFT_PAREN: {
      std::unique_ptr<node::ExprNode> expression = parse_expression();
      try_consume(tokenization::TokenType::TT_RIGHT_PAREN);

      term->node = std::make_unique<node::TermExprNode>(std::move(expression));
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