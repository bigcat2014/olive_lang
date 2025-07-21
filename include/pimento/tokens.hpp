//! @file tokens.hpp
//! @brief Pimento Tokens
//! @author Logan Thomas

#pragma once

#include <optional>
#include <ranges>
#include <string>

namespace pimento::tokenization {

// TODO(lthomas): This tokenization method breaks down when there are tokens
// that are just double versions of another token; i.e. `==` and `=`. May need
// to parse these as their single version and handle doubles in the parser.
//! @brief The supported token types.
enum class TokenType {
  _BEGIN = 0,
  // Keywords
  TT_ELSE,
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

//! @brief Array of tokens that need special cases for parsing.
constexpr std::array<TokenType, 2> SPECIAL_TOKENS = {TokenType::TT_INT_LITERAL,
                                                     TokenType::TT_IDENTIFIER};

//! @brief Parsed token.
struct Token {
  //! @brief  @brief The type of this token.
  const TokenType token_type;
  //! @brief The optional value associated with this token.
  const std::optional<std::string> value;
};

//! @brief Token string conversions.
struct TokenString {
  //! @brief Human readable name of the token type for debug printing.
  const std::string name;
  //! @brief String version of the token to match against when tokenizing.
  const std::string token;
};

//! @brief Create a TokenString from a particular token type.
//! @param token const TokenType& The token type to generate the token string
//! for.
//! @return TokenString The string representations of the token type.
TokenString token_to_str(const TokenType& token) {
  // TODO(lthomas): Should just be a lookup table instead of regenerating
  // TokenString every time the function is called.
  switch (token) {
    case TokenType::TT_ELSE:
      return {.name = "TT_ELSE", .token = "else"};
    case TokenType::TT_EXIT:
      return {.name = "TT_EXIT", .token = "exit"};
    case TokenType::TT_IF:
      return {.name = "TT_IF", .token = "if"};
    case TokenType::TT_ELIF:
      return {.name = "TT_ELIF", .token = "elif"};
    case TokenType::TT_LET:
      return {.name = "TT_LET", .token = "let"};
    case TokenType::TT_LEFT_CURLY:
      return {.name = "TT_LEFT_CURLY", .token = "{"};
    case TokenType::TT_LEFT_PAREN:
      return {.name = "TT_LEFT_PAREN", .token = "("};
    case TokenType::TT_RIGHT_CURLY:
      return {.name = "TT_RIGHT_CURLY", .token = "}"};
    case TokenType::TT_RIGHT_PAREN:
      return {.name = "TT_RIGHT_PAREN", .token = ")"};
    case TokenType::TT_DOUBLE_CARET:
      return {.name = "TT_DOUBLE_CARET", .token = "^^"};
    case TokenType::TT_FORWARD_SLASH:
      return {.name = "TT_FORWARD_SLASH", .token = "/"};
    case TokenType::TT_MINUS:
      return {.name = "TT_MINUS", .token = "-"};
    case TokenType::TT_PERCENT:
      return {.name = "TT_PERCENT", .token = "%"};
    case TokenType::TT_PLUS:
      return {.name = "TT_PLUS", .token = "+"};
    case TokenType::TT_STAR:
      return {.name = "TT_STAR", .token = "*"};
    case TokenType::TT_EQUAL:
      return {.name = "TT_EQUAL", .token = "="};
    case TokenType::TT_IDENTIFIER:
      return {.name = "TT_IDENTIFIER", .token = ""};
    case TokenType::TT_INT_LITERAL:
      return {.name = "TT_INT_LITERAL", .token = ""};
    case TokenType::TT_SEMI:
      return {.name = "TT_SEMI", .token = ";"};
    default:
      return {.name = "UNKNOWN TOKEN", .token = ""};
  }
}

//! @brief Create an iterable view of the elements in the TokenType enum class;
//! exclusive.
//! @param first TokenType The starting token in the enum class.
//! @param last
//! @return Iterable view of elements in TokenType enum class.
constexpr inline auto enum_range(TokenType first, TokenType last) {
  auto enum_range =
      std::views::iota(
          static_cast<std::underlying_type_t<decltype(first)>>(first) + 1,
          static_cast<std::underlying_type_t<decltype(last)>>(last)) |
      std::views::transform(
          [](auto enum_val) { return (decltype(first))enum_val; });

  return enum_range;
};

}  // namespace pimento::tokenization