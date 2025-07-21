//! @file tokens.hpp
//! @brief Pimento Tokens
//! @author Logan Thomas

#pragma once

namespace pimento {

enum class TokenType {
  // Keywords
  TT_ELSE = 0,
  TT_EXIT,
  TT_IF,
  TT_ELIF,
  TT_LET,
  // Braces
  TT_LEFT_CURLY,
  TT_LEFT_PAREN,
  TT_RIGHT_CURLY,
  TT_RIGHT_PAREN,
  // Arithmetic operators
  TT_DOUBLE_CARET,
  TT_FORWARD_SLASH,
  TT_MINUS,
  TT_PERCENT,
  TT_PLUS,
  TT_STAR,
  // Additional operators
  TT_EQUAL,
  // Miscellaneous
  TT_IDENTIFIER,
  TT_INT_LITERAL,
  TT_SEMI,
  NUM_TOKENS
};

struct Token {
  TokenType token_type;
};

std::string token_to_str(const TokenType& token) {
  switch (token) {
    case TokenType::TT_ELSE:
      return "TT_ELSE";
    case TokenType::TT_EXIT:
      return "TT_EXIT";
    case TokenType::TT_IF:
      return "TT_IF";
    case TokenType::TT_ELIF:
      return "TT_IFELSE";
    case TokenType::TT_LET:
      return "TT_LET";
    case TokenType::TT_LEFT_CURLY:
      return "TT_LEFT_CURLY";
    case TokenType::TT_LEFT_PAREN:
      return "TT_LEFT_PAREN";
    case TokenType::TT_RIGHT_CURLY:
      return "TT_RIGHT_CURLY";
    case TokenType::TT_RIGHT_PAREN:
      return "TT_RIGHT_PAREN";
    case TokenType::TT_DOUBLE_CARET:
      return "TT_CARET";
    case TokenType::TT_FORWARD_SLASH:
      return "TT_FORWARD_SLASH";
    case TokenType::TT_MINUS:
      return "TT_MINUS";
    case TokenType::TT_PERCENT:
      return "TT_PERCENT";
    case TokenType::TT_PLUS:
      return "TT_PLUS";
    case TokenType::TT_STAR:
      return "TT_STAR";
    case TokenType::TT_EQUAL:
      return "TT_EQUAL";
    case TokenType::TT_IDENTIFIER:
      return "TT_IDENTIFIER";
    case TokenType::TT_INT_LITERAL:
      return "TT_INT_LITERAL";
    case TokenType::TT_SEMI:
      return "TT_SEMI";
    default:
      return "UNKNOWN TOKEN";
  }
}

}  // namespace pimento