//! @file parser.hpp
//! @brief Pimento parser
//! @author Logan Thomas

#pragma once

#include <memory>
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
    auto &logger = utils::get_logger();

    while (peek(m_index).has_value()) {
      tokenization::Token current_token = m_tokens[m_index];
      std::optional<tokenization::Token> next;

      switch (current_token.token_type) {
      case tokenization::TokenType::TT_ELSE: {
        break;
      }
      case tokenization::TokenType::TT_EXIT: {
        // Parse format exit([Expr]);
        expect_token(tokenization::TokenType::TT_LEFT_PAREN);

        node::StmtNode stmt;
        if (auto expr = parse_expression()) {
          stmt.node = std::make_unique<node::StmtExitNode>(
              std::make_unique<node::ExprNode>(std::move(expr.value())));
        } else {
          logger.error("Expected expression at TODO Line & Column number");
          exit(EXIT_FAILURE);
        }

        expect_token(tokenization::TokenType::TT_RIGHT_PAREN);
        expect_token(tokenization::TokenType::TT_SEMI);
        break;
      }
      case tokenization::TokenType::TT_IF: {
        break;
      }
      case tokenization::TokenType::TT_ELIF: {
        break;
      }
      case tokenization::TokenType::TT_LET: {
        break;
      }
      case tokenization::TokenType::TT_LEFT_CURLY: {
        break;
      }
      case tokenization::TokenType::TT_LEFT_PAREN: {
        break;
      }
      case tokenization::TokenType::TT_RIGHT_CURLY: {
        break;
      }
      case tokenization::TokenType::TT_RIGHT_PAREN: {
        break;
      }
      case tokenization::TokenType::TT_DOUBLE_CARET: {
        break;
      }
      case tokenization::TokenType::TT_FORWARD_SLASH: {
        break;
      }
      case tokenization::TokenType::TT_MINUS: {
        break;
      }
      case tokenization::TokenType::TT_PERCENT: {
        break;
      }
      case tokenization::TokenType::TT_PLUS: {
        break;
      }
      case tokenization::TokenType::TT_STAR: {
        break;
      }
      case tokenization::TokenType::TT_EQUAL: {
        break;
      }
      case tokenization::TokenType::TT_IDENTIFIER: {
        break;
      }
      case tokenization::TokenType::TT_INT_LITERAL: {
        break;
      }
      case tokenization::TokenType::TT_SEMI: {
        break;
      }
      default: {
        throw std::runtime_error("Unknown token");
      }
      }

      logger.trace("Finished parsing token: {}",
                   tokenization::TokenTypeUtil::get_type_as_str(
                       current_token.token_type));
      consume();
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
    if (m_index + lookahead < m_tokens.size()) {
      return m_tokens[m_index + lookahead];
    }
    return {};
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

  //! @brief Expect the next token to be a specific TokenType.
  //!
  //! Expect the next token to be a specific TokenType. If the token type
  //! matches, consume it, otherwise log an error and exit.
  //! @param token_type tokenization::TokenType The token type to expect.
  inline void expect_token(tokenization::TokenType token_type) noexcept {
    auto next = try_consume();
    if (!next.has_value() || next.value().token_type != token_type) {
      auto &logger = utils::get_logger();
      logger.error("Expected `{}` at TODO Line & Column number",
                   tokenization::TokenTypeUtil::get_token_str(token_type));
      exit(EXIT_FAILURE);
    }
  }

  // TODO(lthomas): Fill out this function body.
  std::optional<node::ExprNode> parse_expression() { return {}; }

  //! @brief The current parsing index
  size_t m_index{0};
  //! @brief The tokens to parse.
  const std::vector<tokenization::Token> m_tokens;
};

} // namespace pimento::ast