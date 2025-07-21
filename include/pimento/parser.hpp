//! @file parser.hpp
//! @brief Pimento parser
//! @author Logan Thomas

#pragma once

#include <optional>
#include <pimento/ast.hpp>
#include <pimento/tokens.hpp>
#include <pimento/utils.hpp>
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
      prog.statements.emplace_back(std::move(stmt));

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
    if (auto ret = peek(1)) {
      consume();
      return ret;
    }
    return {};
  }

  //! @brief Assert the next token to be a specific TokenType.
  //!
  //! Assert the next token to be a specific TokenType. If the token type
  //! matches, consume it, otherwise log an error and exit.
  //! @param token_type tokenization::TokenType The token type we are asserting
  //! is next.
  inline tokenization::Token
  assert_next_token(tokenization::TokenType token_type) noexcept {
    auto next = try_consume();
    if (!next.has_value() || next.value().token_type != token_type) {
      auto &logger = utils::get_logger();
      logger.error("Expected `{}` at TODO Line & Column number",
                   tokenization::TokenTypeUtil::get_token_str(token_type));
      exit(EXIT_FAILURE);
    }
    return next.value();
  }

  // TODO(lthomas): Fill out this function body.
  std::unique_ptr<node::StmtNode> parse_statement() {
    auto &logger = utils::get_logger();
    std::unique_ptr<node::StmtNode> stmt;
    tokenization::Token current_token = m_tokens[m_index];

    switch (current_token.token_type) {
    // Parse format exit([Expr]);
    case tokenization::TokenType::TT_EXIT: {
      assert_next_token(tokenization::TokenType::TT_LEFT_PAREN);

      std::unique_ptr<node::ExprNode> expression = parse_expression();

      assert_next_token(tokenization::TokenType::TT_RIGHT_PAREN);
      assert_next_token(tokenization::TokenType::TT_SEMI);

      stmt->node = std::unique_ptr<node::StmtExitNode>(
          new node::StmtExitNode{.expression = std::move(expression)});
      break;
    }
    // Parse format let ident = [Expr];
    case tokenization::TokenType::TT_LET: {
      tokenization::Token identifier =
          assert_next_token(tokenization::TokenType::TT_IDENTIFIER);

      assert_next_token(tokenization::TokenType::TT_EQUAL);

      std::unique_ptr<node::ExprNode> expression = parse_expression();

      assert_next_token(tokenization::TokenType::TT_SEMI);

      stmt->node = std::unique_ptr<node::StmtLetNode>(new node::StmtLetNode{
          .identifier = identifier, .expression = std::move(expression)});
      break;
    }
    // Parse format ident = [Expr];
    case tokenization::TokenType::TT_IDENTIFIER: {
      assert_next_token(tokenization::TokenType::TT_EQUAL);

      std::unique_ptr<node::ExprNode> expression = parse_expression();

      assert_next_token(tokenization::TokenType::TT_SEMI);

      stmt->node = std::unique_ptr<node::StmtAssignNode>(
          new node::StmtAssignNode{.identifier = current_token,
                                   .expression = std::move(expression)});
      break;
    }
    // Parse format if ([Expr]) [Scope] [IfPred]
    case tokenization::TokenType::TT_IF: {
      assert_next_token(tokenization::TokenType::TT_LEFT_PAREN);

      std::unique_ptr<node::ExprNode> expression = parse_expression();

      assert_next_token(tokenization::TokenType::TT_RIGHT_PAREN);

      std::unique_ptr<node::ScopeNode> scope = parse_scope();

      // TODO(lthomas): Decide if should handle empty ifpred (valid grammar)
      // here or somewhere else.
      std::unique_ptr<node::IfPredNode> ifpred = parse_ifpred();

      stmt->node = std::unique_ptr<node::StmtIfNode>(
          new node::StmtIfNode{.expression = std::move(expression),
                               .scope = std::move(scope),
                               .ifpred = std::move(ifpred)});

      break;
    }
    default: {
      // stmt.node = parse_scope();
      logger.debug("Need to parse scope?");
    }
    }
    return stmt;
  }

  // TODO(lthomas): Fill out this function body.
  std::unique_ptr<node::ExprNode> parse_expression() {
    auto &logger = utils::get_logger();
    logger.error("Expected expression at TODO Line & Column number");
    exit(EXIT_FAILURE);
  }

  // TODO(lthomas): Fill out this function body.
  std::unique_ptr<node::ScopeNode> parse_scope() {
    assert_next_token(tokenization::TokenType::TT_LEFT_CURLY);

    do {
      tokenization::Token next = try_consume();
      tokenization::Token current_token = m_tokens[m_index];
    } while (next != tokenization::TokenType::TT_RIGHT_CURLY);

    std::unique_ptr<node::StmtNode> statement = parse_statement();

    assert_next_token(tokenization::TokenType::TT_RIGHT_CURLY);
  }

  // TODO(lthomas): Fill out this function body.
  std::unique_ptr<node::IfPredNode> parse_ifpred() {
    auto &logger = utils::get_logger();
    logger.error("Expected ifpred at TODO Line & Column number");
    exit(EXIT_FAILURE);
  }

  // TODO(lthomas): Fill out this function body.
  std::unique_ptr<node::BinExprNode> parse_bin_expression() {
    auto &logger = utils::get_logger();
    logger.error("Expected binexpr at TODO Line & Column number");
    exit(EXIT_FAILURE);
  }

  // TODO(lthomas): Fill out this function body.
  std::unique_ptr<node::TermNode> parse_term() {
    auto &logger = utils::get_logger();
    logger.error("Expected term at TODO Line & Column number");
    exit(EXIT_FAILURE);
  }

  //! @brief The current parsing index
  size_t m_index{0};
  //! @brief The tokens to parse.
  const std::vector<tokenization::Token> m_tokens;

  //! @brief The root node of the ast
  node::ProgNode prog;
};

} // namespace pimento::ast