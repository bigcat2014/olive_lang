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

class Parser {
public:
  inline explicit Parser(const std::vector<tokenization::Token> &tokens)
      : m_tokens(tokens) {}

  void parse() {
    auto &logger = utils::get_logger();

    size_t i = 0;
    while (peek(i, m_tokens).has_value()) {
      tokenization::Token current_token = m_tokens[i];
      switch (current_token.token_type) {
      case tokenization::TokenType::TT_ELSE:
        break;
      case tokenization::TokenType::TT_EXIT:
        break;
      case tokenization::TokenType::TT_IF:
        break;
      case tokenization::TokenType::TT_ELIF:
        break;
      case tokenization::TokenType::TT_LET:
        break;
      case tokenization::TokenType::TT_LEFT_CURLY:
        break;
      case tokenization::TokenType::TT_LEFT_PAREN:
        break;
      case tokenization::TokenType::TT_RIGHT_CURLY:
        break;
      case tokenization::TokenType::TT_RIGHT_PAREN:
        break;
      case tokenization::TokenType::TT_DOUBLE_CARET:
        break;
      case tokenization::TokenType::TT_FORWARD_SLASH:
        break;
      case tokenization::TokenType::TT_MINUS:
        break;
      case tokenization::TokenType::TT_PERCENT:
        break;
      case tokenization::TokenType::TT_PLUS:
        break;
      case tokenization::TokenType::TT_STAR:
        break;
      case tokenization::TokenType::TT_EQUAL:
        break;
      case tokenization::TokenType::TT_IDENTIFIER:
        break;
      case tokenization::TokenType::TT_INT_LITERAL:
        break;
      case tokenization::TokenType::TT_SEMI:
        break;
      default:
        throw std::runtime_error("Unknown token");
      }

      logger.trace("Finished parsing token: {}",
                   tokenization::token_str.at(current_token.token_type));
      i++;
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
  peek(size_t current_index, const std::vector<tokenization::Token> &tokens,
       size_t lookahead = 0) const noexcept {
    if (current_index + lookahead < tokens.size()) {
      return tokens[current_index + lookahead];
    }
    return {};
  }

  const std::vector<tokenization::Token> m_tokens;
};

} // namespace pimento::ast