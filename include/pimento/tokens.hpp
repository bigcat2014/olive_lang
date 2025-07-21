//! @file tokens.hpp
//! @brief Pimento Tokens
//! @author Logan Thomas

#pragma once

#include <optional>
#include <ranges>
#include <string>
#include <variant>

#include <boost/bimap.hpp>

namespace pimento::tokenization {

// TODO(lthomas): This tokenization method breaks down when there are tokens
// that are just double versions of another token; i.e. `==` and `=`. May need
// to parse these as their single version and handle doubles in the parser.
//! @brief The supported token types.
enum class TokenType {
  BEGIN = 0,
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
  // Binary operators
  TT_DOUBLE_CARET,
  TT_FORWARD_SLASH,
  TT_MINUS,
  TT_PERCENT,
  TT_PLUS,
  TT_STAR,
  TT_LT,
  TT_GT,
  // Additional operators
  TT_EQUAL,
  // Miscellaneous
  TT_IDENTIFIER,
  TT_INT_LITERAL,
  TT_SEMI,
  NUM_TOKENS
};

enum class Associativity { LEFT = 0, RIGHT };

struct BinOpProperties {
  uint8_t precedence;
  Associativity associativity;
};

struct IntLitProperties {
  uint64_t value;
};

struct IdentProperties {
  std::string identifier;
};

//! @brief Parsed token.
struct Token {
  //! @brief  @brief The type of this token.
  TokenType token_type;

  //! @brief The properties associated with this token.
  std::variant<BinOpProperties, IntLitProperties, IdentProperties,
               std::monostate>
      properties;
};

//! @brief Static utility class for interacting with token types.
class TokenTypeUtil {
public:
  TokenTypeUtil() = delete;

  //! @brief Get a human-readable string representing the token.
  //! @param token_type TokenType The token type for which to return the string.
  //! @return std::string The human-readable string representation of the token.
  [[nodiscard]] static inline std::string
  get_type_as_str(TokenType token_type) {
    try {
      return get_token_str_map().at(token_type);
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
      return get_token_bimap().right.at(token_type);
    } catch (const std::out_of_range &) {
      return "UNKNOWN_TOKEN";
    }
  }

  [[nodiscard]] static inline std::string
  get_associativity_str(Associativity assoc) {
    try {
      return get_associativity_str_map().at(assoc);
    } catch (const std::out_of_range &) {
      return "UNKNOWN_ASSOCIATIVITY";
    }
  }

  //! @brief Get the token type associated with a parsed string.
  //! @param token std::string The parsed token for which to get the token type.
  //! @return TokenType The token type associated with the provided token
  //! string.
  [[nodiscard]] static inline TokenType get_token_type(std::string token) {
    // Bubble up exception
    return get_token_bimap().left.at(token);
  }

  [[nodiscard]] static inline std::pair<uint8_t, Associativity>
  get_bin_expr_properties(TokenType token) {
    // Bubble up exception
    return get_bin_expr_properties_map().at(token);
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

private:
  using AssociativityMap = std::unordered_map<Associativity, std::string>;
  using TokenStrMap = std::unordered_map<TokenType, std::string>;
  using TokenBimap = boost::bimap<std::string, TokenType>;
  using BinExprMap =
      std::unordered_map<TokenType, std::pair<uint8_t, Associativity>>;

  //! @brief Map from token types to printable strings.
  [[nodiscard]] static AssociativityMap get_associativity_str_map() noexcept {
    // clang-format off
    static const AssociativityMap associativity_str{
        {Associativity::LEFT,  "LEFT"},
        {Associativity::RIGHT, "RIGHT"}
    };
    // clang-format on

    return associativity_str;
  }

  //! @brief Map from token types to printable strings.
  [[nodiscard]] static TokenStrMap get_token_str_map() noexcept {
    // clang-format off
    static const TokenStrMap token_str{
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
        {TokenType::TT_LT,            "TT_LT"},
        {TokenType::TT_GT,            "TT_GT"},
        {TokenType::TT_EQUAL,         "TT_EQUAL"},
        {TokenType::TT_IDENTIFIER,    "TT_IDENTIFIER"},
        {TokenType::TT_INT_LITERAL,   "TT_INT_LITERAL"},
        {TokenType::TT_SEMI,          "TT_SEMI"},
    };
    // clang-format on

    return token_str;
  }

  //! @brief Bidirectional map between token strings and token types
  [[nodiscard]] static TokenBimap get_token_bimap() noexcept {
    // clang-format off
    static const TokenBimap token_lookup =
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
                                            {"<",    TokenType::TT_LT},
                                            {">",    TokenType::TT_GT},
                                            {"=",    TokenType::TT_EQUAL},
                                            {";",    TokenType::TT_SEMI},
          });
    // clang-format on

    return token_lookup;
  }

  //! @brief Map from token types to printable strings.
  [[nodiscard]] static BinExprMap get_bin_expr_properties_map() noexcept {
    // clang-format off
    static const BinExprMap
        bin_expr_properties{
            {TokenType::TT_DOUBLE_CARET,  {3, Associativity::RIGHT}},
            {TokenType::TT_STAR,          {2, Associativity::LEFT}},
            {TokenType::TT_FORWARD_SLASH, {2, Associativity::LEFT}},
            {TokenType::TT_PERCENT,       {2, Associativity::LEFT}},
            {TokenType::TT_PLUS,          {1, Associativity::LEFT}},
            {TokenType::TT_MINUS,         {1, Associativity::LEFT}},
            {TokenType::TT_LT,            {0, Associativity::LEFT}},
            {TokenType::TT_GT,            {0, Associativity::LEFT}},
        };
    // clang-format on

    return bin_expr_properties;
  }
};

class TokenFactory {
public:
  [[nodiscard]] static inline Token
  create_token(TokenType token_type) noexcept {
    try {
      std::pair<uint8_t, Associativity> bin_op_properties =
          TokenTypeUtil::get_bin_expr_properties(token_type);
      // Token is a binary operator, should include binary operator properties
      return Token{.token_type = token_type,
                   .properties = BinOpProperties{
                       .precedence = bin_op_properties.first,
                       .associativity = bin_op_properties.second}};
    } catch (const std::out_of_range &) {
      // No token properties
      return Token{.token_type = token_type, .properties = std::monostate()};
    }
  }

  [[nodiscard]] static inline Token create_token(TokenType token_type,
                                                 uint64_t value) noexcept {
    return Token{.token_type = token_type,
                 .properties = IntLitProperties{.value = value}};
  }

  [[nodiscard]] static inline Token
  create_token(TokenType token_type, std::string identifier) noexcept {
    return Token{.token_type = token_type,
                 .properties = IdentProperties{.identifier = identifier}};
  }

private:
};
} // namespace pimento::tokenization