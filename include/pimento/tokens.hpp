//! @file tokens.hpp
//! @brief Pimento Tokens
//! @author Logan Thomas

#pragma once

#include <boost/bimap.hpp>
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

//! @brief Parsed token.
struct Token {
  //! @brief  @brief The type of this token.
  TokenType token_type;
  //! @brief The optional value associated with this token.
  std::optional<std::string> value;
};

//! @brief Static utility class for interacting with token types.
class TokenTypeUtil {
public:
  enum class Associativity { LEFT = 0, RIGHT };

public:
  TokenTypeUtil() = delete;

  //! @brief Get a human-readable string representing the token.
  //! @param token_type TokenType The token type for which to return the string.
  //! @return std::string The human-readable string representation of the token.
  [[nodiscard]] static inline std::string
  get_type_as_str(TokenType token_type) {
    try {
      return s_token_str.at(token_type);
    } catch (const std::out_of_range &) {
      return "UNKNOWN_TOKEN";
    }
  }

  //! @brief Get the token string that is parsed in order to get a particular
  //! token type.
  //! @param token_type TokenType The token type for which to return the string.
  //! @return std::string The string representation of the token for parsing.
  [[nodiscard]] static inline std::string get_token_str(TokenType token_type) {
    try {
      return s_token_lookup.right.at(token_type);
    } catch (const std::out_of_range &) {
      return "UNKNOWN_TOKEN";
    }
  }

  //! @brief Get the token type associated with a parsed string.
  //! @param token std::string The parsed token for which to get the token type.
  //! @return TokenType The token type associated with the provided token
  //! string.
  [[nodiscard]] static inline TokenType get_token_type(std::string token) {
    // Bubble up exception
    return s_token_lookup.left.at(token);
  }

  [[nodiscard]] static inline std::pair<int, Associativity>
  get_bin_expr_properties(TokenType token) {
    // Bubble up exception
    return s_bin_expr_properties.at(token);
  }

private:
  //! @brief Create a boost::bimap from an initializer list.
  //! @tparam L The type of the left side of the boost::bimap to initialize.
  //! @tparam R The type of the right side of the boost::bimap to initialize.
  //! @param list The initializer list from which to initialize the
  //! boost::bimap.
  //! @return boost::bimap<L, R> The boost::bimap initialized with the provided
  //! initializer list.
  template <typename L, typename R>
  [[nodiscard]] static inline boost::bimap<L, R> make_bimap(
      std::initializer_list<typename boost::bimap<L, R>::value_type> list) {
    return boost::bimap<L, R>(list.begin(), list.end());
  }

  // clang-format off
  //! @brief Map from token types to printable strings.
  static const inline std::unordered_map<TokenType, std::string> s_token_str{
      {TokenType::TT_ELSE,          "TT_ELSE"},
      {TokenType::TT_EXIT,          "TT_EXIT"},
      {TokenType::TT_IF,            "TT_IF"},
      {TokenType::TT_ELIF,          "TT_ELIF"},
      {TokenType::TT_LET,           "TT_LET"},
      {TokenType::TT_LEFT_CURLY,    "TT_LEFT_CURLY"},
      {TokenType::TT_LEFT_PAREN,    "TT_LEFT_PAREN"},
      {TokenType::TT_RIGHT_CURLY,   "TT_RIGHT_CURLY"},
      {TokenType::TT_RIGHT_PAREN,   "TT_RIGHT_PAREN"},
      {TokenType::TT_DOUBLE_CARET,  "TT_DOUBLE_CARET"},
      {TokenType::TT_FORWARD_SLASH, "TT_FORWARD_SLASH"},
      {TokenType::TT_MINUS,         "TT_MINUS"},
      {TokenType::TT_PERCENT,       "TT_PERCENT"},
      {TokenType::TT_PLUS,          "TT_PLUS"},
      {TokenType::TT_STAR,          "TT_STAR"},
      {TokenType::TT_EQUAL,         "TT_EQUAL"},
      {TokenType::TT_IDENTIFIER,    "TT_IDENTIFIER"},
      {TokenType::TT_INT_LITERAL,   "TT_INT_LITERAL"},
      {TokenType::TT_SEMI,          "TT_SEMI"}};
  // clang-format on

  // clang-format off
  //! @brief Bidirectional map between token strings and token types
  static const inline boost::bimap<std::string, TokenType> s_token_lookup =
      make_bimap<std::string, TokenType>({{"else", TokenType::TT_ELSE},
                                          {"exit", TokenType::TT_EXIT},
                                          {"if",   TokenType::TT_IF},
                                          {"elif", TokenType::TT_ELIF},
                                          {"let",  TokenType::TT_LET},
                                          {"{",    TokenType::TT_LEFT_CURLY},
                                          {"(",    TokenType::TT_LEFT_PAREN},
                                          {"}",    TokenType::TT_RIGHT_CURLY},
                                          {")",    TokenType::TT_RIGHT_PAREN},
                                          {"^^",   TokenType::TT_DOUBLE_CARET},
                                          {"/",    TokenType::TT_FORWARD_SLASH},
                                          {"-",    TokenType::TT_MINUS},
                                          {"%",    TokenType::TT_PERCENT},
                                          {"+",    TokenType::TT_PLUS},
                                          {"*",    TokenType::TT_STAR},
                                          {"=",    TokenType::TT_EQUAL},
                                          {";",    TokenType::TT_SEMI}});
  // clang-format on

  // clang-format off
  //! @brief Map from token types to printable strings.
  static const inline std::unordered_map<TokenType,
                                         std::pair<int, Associativity>>
      s_bin_expr_properties{
          {TokenType::TT_DOUBLE_CARET,  {2, Associativity::RIGHT}},
          {TokenType::TT_STAR,          {1, Associativity::LEFT}},
          {TokenType::TT_FORWARD_SLASH, {1, Associativity::LEFT}},
          {TokenType::TT_PERCENT,       {1, Associativity::LEFT}},
          {TokenType::TT_PLUS,          {0, Associativity::LEFT}},
          {TokenType::TT_MINUS,         {0, Associativity::LEFT}}};
  // clang-format on
};
} // namespace pimento::tokenization