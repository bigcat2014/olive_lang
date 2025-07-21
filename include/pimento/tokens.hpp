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

//! @brief Map from token types to printable strings.
const std::unordered_map<TokenType, std::string> token_str{
  {TokenType::TT_ELSE, "TT_ELSE"},
  {TokenType::TT_EXIT, "TT_EXIT"},
  {TokenType::TT_IF, "TT_IF"},
  {TokenType::TT_ELIF, "TT_ELIF"},
  {TokenType::TT_LET, "TT_LET"},
  {TokenType::TT_LEFT_CURLY, "TT_LEFT_CURLY"},
  {TokenType::TT_LEFT_PAREN, "TT_LEFT_PAREN"},
  {TokenType::TT_RIGHT_CURLY, "TT_RIGHT_CURLY"},
  {TokenType::TT_RIGHT_PAREN, "TT_RIGHT_PAREN"},
  {TokenType::TT_DOUBLE_CARET, "TT_DOUBLE_CARET"},
  {TokenType::TT_FORWARD_SLASH, "TT_FORWARD_SLASH"},
  {TokenType::TT_MINUS, "TT_MINUS"},
  {TokenType::TT_PERCENT, "TT_PERCENT"},
  {TokenType::TT_PLUS, "TT_PLUS"},
  {TokenType::TT_STAR, "TT_STAR"},
  {TokenType::TT_EQUAL, "TT_EQUAL"},
  {TokenType::TT_IDENTIFIER, "TT_IDENTIFIER"},
  {TokenType::TT_INT_LITERAL, "TT_INT_LITERAL"},
  {TokenType::TT_SEMI, "TT_SEMI"},
};

//! @brief Token strings to token types
const std::unordered_map<std::string, TokenType> token_lookup{
  {"else", TokenType::TT_ELSE},
  {"exit", TokenType::TT_EXIT},
  {"if", TokenType::TT_IF},
  {"elif", TokenType::TT_ELIF},
  {"let", TokenType::TT_LET},
  {"{", TokenType::TT_LEFT_CURLY},
  {"(", TokenType::TT_LEFT_PAREN},
  {"}", TokenType::TT_RIGHT_CURLY},
  {")", TokenType::TT_RIGHT_PAREN},
  {"^^", TokenType::TT_DOUBLE_CARET},
  {"/", TokenType::TT_FORWARD_SLASH},
  {"-", TokenType::TT_MINUS},
  {"%", TokenType::TT_PERCENT},
  {"+", TokenType::TT_PLUS},
  {"*", TokenType::TT_STAR},
  {"=", TokenType::TT_EQUAL},
  {";", TokenType::TT_SEMI},
};

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