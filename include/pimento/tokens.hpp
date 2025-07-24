//! @file tokens.hpp
//! @brief Pimento Tokens.
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
//! @brief Supported token types.
enum class TokenType : uint8_t {
  BEGIN = 0,
  EXIT,
  IF,
  ELIF,
  ELSE,
  WHILE,
  FOR,
  IN,
  BREAK,
  UINT8,
  UINT16,
  UINT32,
  UINT64,
  INT8,
  INT16,
  INT32,
  INT64,
  FLOAT,
  STRING,
  BOOL,
  TRUE,
  FALSE,
  FUNCTION,
  CLASS,
  ENUM,
  INTERFACE,
  MUTABLE,
  LEFT_CURLY,
  LEFT_PAREN,
  LEFT_SQUARE,
  RIGHT_CURLY,
  RIGHT_PAREN,
  RIGHT_SQUARE,
  SEMI,
  EXPONENT_ASSIGN,
  EXPONENT,
  XOR_ASSIGN,
  XOR,
  MOD_ASSIGN,
  MOD,
  MUL_ASSIGN,
  MUL,
  INTEGER_DIV_ASSIGN,
  INTEGER_DIV,
  DIV_ASSIGN,
  DIV,
  INC,
  ADD_ASSIGN,
  ADD,
  DEC,
  SUB_ASSIGN,
  SUB,
  SHIFT_LEFT,
  LE_OP,
  LT_OP,
  SHIFT_RIGHT,
  GE_OP,
  GT_OP,
  EQ_OP,
  ASSIGN,
  LOGICAL_AND,
  AND_ASSIGN,
  AND,
  LOGICAL_OR,
  OR_ASSIGN,
  OR,
  DOT,
  COMMENT,
  IDENTIFIER,
  NUMERIC_CONST,
  STRING_LITERAL,
  NUM_TOKENS
};

//! @brief Properties of a Binary Operator token.
struct BinOpProperties {
  //! @brief Associativity of binary operators.
  enum class Associativity : uint8_t { LEFT = 0, RIGHT };

  uint8_t precedence;
  Associativity associativity;
};

//! @brief Properties of an Int Literal token.
struct IntLitProperties {
  // TODO(lthomas): Only supports 64 bit ints for now.
  uint64_t value;
};

//! @brief Properties of an Idenfier token.
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

  //! @brief Get the token string that needs to be parsed in order to get a
  //! particular token type.
  //! @param token_type TokenType The token type for which to return the string
  //! representation.
  //! @return std::string The string representation of the token for parsing.
  [[nodiscard]] static inline std::string get_token_str(TokenType token_type) {
    try {
      return get_token_bimap().right.at(token_type);
    } catch (const std::out_of_range &) {
      return "UNKNOWN_TOKEN";
    }
  }

  //! @brief Get the human-readable string representing the associativity.
  //! @param assoc BinOpProperties::Associativity The Associativity for which to
  //! return the string.
  //! @return std::string The human-readable string representation of the token.
  [[nodiscard]] static inline std::string
  get_associativity_str(BinOpProperties::Associativity assoc) {
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
  [[nodiscard]] static inline TokenType
  get_token_type(const std::string &token) {
    // Bubble up exceptions
    return get_token_bimap().left.at(token);
  }

  //! @brief Get the binary operator properties of a particular token type.
  //! @param token The token type for which to get the binary operator
  //! properties.
  //! @return std::pair<uint8_t, BinOpProperties::Associativity> The precedence
  //! and associativity of the specified binary operator.
  [[nodiscard]] static inline std::pair<uint8_t, BinOpProperties::Associativity>
  get_bin_expr_properties(TokenType token) {
    // Bubble up exceptions
    return get_bin_expr_properties_map().at(token);
  }

private:
  //! @brief Create a boost::bimap from an initializer list.
  //! @tparam L The type of the left side of the boost::bimap to initialize.
  //! @tparam R The type of the right side of the boost::bimap to initialize.
  //! @param list std::initializer_list<typename boost::bimap<L, R>::value_type>
  //! The initializer list from which to initialize the boost::bimap.
  //! @return boost::bimap<L, R> The boost::bimap initialized with the provided
  //! initializer list.
  template <typename L, typename R>
  [[nodiscard]] static inline boost::bimap<L, R> make_bimap(
      std::initializer_list<typename boost::bimap<L, R>::value_type> list) {
    return boost::bimap<L, R>(list.begin(), list.end());
  }

private:
  using AssociativityMap =
      std::unordered_map<BinOpProperties::Associativity, std::string>;
  using TokenStrMap = std::unordered_map<TokenType, std::string>;
  using TokenBimap = boost::bimap<std::string, TokenType>;
  using BinExprMap =
      std::unordered_map<TokenType,
                         std::pair<uint8_t, BinOpProperties::Associativity>>;

  //! @brief Get the map from associtivity types to human-readable strings.
  [[nodiscard]] static AssociativityMap get_associativity_str_map() noexcept {
    // clang-format off
    static const AssociativityMap associativity_str{
        {BinOpProperties::Associativity::LEFT,  "LEFT"},
        {BinOpProperties::Associativity::RIGHT, "RIGHT"}
    };
    // clang-format on

    return associativity_str;
  }

  //! @brief Get the map from token types to human-readable strings.
  [[nodiscard]] static TokenStrMap get_token_str_map() noexcept {
    // clang-format off
    static const TokenStrMap token_str{
        {TokenType::EXIT,               "EXIT"},
        {TokenType::IF,                 "IF"},
        {TokenType::ELIF,               "ELIF"},
        {TokenType::ELSE,               "ELSE"},
        {TokenType::WHILE,              "WHILE"},
        {TokenType::FOR,                "FOR"},
        {TokenType::IN,                 "IN"},
        {TokenType::BREAK,              "BREAK"},
        {TokenType::UINT8,              "UINT8"},
        {TokenType::UINT16,             "UINT16"},
        {TokenType::UINT32,             "UINT32"},
        {TokenType::UINT64,             "UINT64"},
        {TokenType::INT8,               "INT8"},
        {TokenType::INT16,              "INT16"},
        {TokenType::INT32,              "INT32"},
        {TokenType::INT64,              "INT64"},
        {TokenType::FLOAT,              "FLOAT"},
        {TokenType::STRING,             "STRING"},
        {TokenType::BOOL,               "BOOL"},
        {TokenType::TRUE,               "TRUE"},
        {TokenType::FALSE,              "FALSE"},
        {TokenType::FUNCTION,           "FUNCTION"},
        {TokenType::CLASS,              "CLASS"},
        {TokenType::ENUM,               "ENUM"},
        {TokenType::INTERFACE,          "INTERFACE"},
        {TokenType::MUTABLE,            "MUTABLE"},
        {TokenType::LEFT_CURLY,         "LEFT_CURLY"},
        {TokenType::LEFT_PAREN,         "LEFT_PAREN"},
        {TokenType::LEFT_SQUARE,        "LEFT_SQUARE"},
        {TokenType::RIGHT_CURLY,        "RIGHT_CURLY"},
        {TokenType::RIGHT_PAREN,        "RIGHT_PAREN"},
        {TokenType::RIGHT_SQUARE,       "RIGHT_SQUARE"},
        {TokenType::SEMI,               "SEMI"},
        {TokenType::EXPONENT_ASSIGN,    "EXPONENT_ASSIGN"},
        {TokenType::EXPONENT,           "EXPONENT"},
        {TokenType::XOR_ASSIGN,         "XOR_ASSIGN"},
        {TokenType::XOR,                "XOR"},
        {TokenType::MOD_ASSIGN,         "MOD_ASSIGN"},
        {TokenType::MOD,                "MOD"},
        {TokenType::MUL_ASSIGN,         "MUL_ASSIGN"},
        {TokenType::MUL,                "MUL"},
        {TokenType::INTEGER_DIV_ASSIGN, "INTEGER_DIV_ASSIGN"},
        {TokenType::INTEGER_DIV,        "INTEGER_DIV"},
        {TokenType::DIV_ASSIGN,         "DIV_ASSIGN"},
        {TokenType::DIV,                "DIV"},
        {TokenType::INC,                "INC"},
        {TokenType::ADD_ASSIGN,         "ADD_ASSIGN"},
        {TokenType::ADD,                "ADD"},
        {TokenType::DEC,                "DEC"},
        {TokenType::SUB_ASSIGN,         "SUB_ASSIGN"},
        {TokenType::SUB,                "SUB"},
        {TokenType::SHIFT_LEFT,         "SHIFT_LEFT"},
        {TokenType::LE_OP,              "LE_OP"},
        {TokenType::LT_OP,              "LT_OP"},
        {TokenType::SHIFT_RIGHT,        "SHIFT_RIGHT"},
        {TokenType::GE_OP,              "GE_OP"},
        {TokenType::GT_OP,              "GT_OP"},
        {TokenType::EQ_OP,              "EQ_OP"},
        {TokenType::ASSIGN,             "ASSIGN"},
        {TokenType::LOGICAL_AND,        "LOGICAL_AND"},
        {TokenType::AND_ASSIGN,         "AND_ASSIGN"},
        {TokenType::AND,                "AND"},
        {TokenType::LOGICAL_OR,         "LOGICAL_OR"},
        {TokenType::OR_ASSIGN,          "OR_ASSIGN"},
        {TokenType::OR,                 "OR"},
        {TokenType::DOT,                "DOT"},
        {TokenType::COMMENT,            "COMMENT"},
        {TokenType::IDENTIFIER,         "IDENTIFIER"},
        {TokenType::NUMERIC_CONST,      "NUMERIC_CONST"},
        {TokenType::STRING_LITERAL,     "STRING_LITERAL"},
    };
    // clang-format on

    return token_str;
  }

  //! @brief Get the bidirectional map between token string representations and
  //! token types
  [[nodiscard]] static TokenBimap get_token_bimap() noexcept {
    // clang-format off
    static const TokenBimap token_lookup =
        make_bimap<std::string, TokenType>({
          {"exit",                                TokenType::EXIT},
          {"if",                                  TokenType::IF},
          {"elif",                                TokenType::ELIF},
          {"else",                                TokenType::ELSE},
          {"while",                               TokenType::WHILE},
          {"for",                                 TokenType::FOR},
          {"in",                                  TokenType::IN},
          {"break",                               TokenType::BREAK},
          {"uint8",                               TokenType::UINT8},
          {"uint16",                              TokenType::UINT16},
          {"uint32",                              TokenType::UINT32},
          {"uint64",                              TokenType::UINT64},
          {"int8",                                TokenType::INT8},
          {"int16",                               TokenType::INT16},
          {"int32",                               TokenType::INT32},
          {"int64",                               TokenType::INT64},
          {"float",                               TokenType::FLOAT},
          {"string",                              TokenType::STRING},
          {"bool",                                TokenType::BOOL},
          {"true",                                TokenType::TRUE},
          {"false",                               TokenType::FALSE},
          {"func",                                TokenType::FUNCTION},
          {"class",                               TokenType::CLASS},
          {"enum",                                TokenType::ENUM},
          {"iface",                               TokenType::INTERFACE},
          {"mut",                                 TokenType::MUTABLE},
          {"{",                                   TokenType::LEFT_CURLY},
          {"(",                                   TokenType::LEFT_PAREN},
          {"[",                                   TokenType::LEFT_SQUARE},
          {"}",                                   TokenType::RIGHT_CURLY},
          {")",                                   TokenType::RIGHT_PAREN},
          {"]",                                   TokenType::RIGHT_SQUARE},
          {";",                                   TokenType::SEMI},
          {"^^=",                                 TokenType::EXPONENT_ASSIGN},
          {"^^",                                  TokenType::EXPONENT},
          {"^=",                                  TokenType::XOR_ASSIGN},
          {"^",                                   TokenType::XOR},
          {"%=",                                  TokenType::MOD_ASSIGN},
          {"%",                                   TokenType::MOD},
          {"*=",                                  TokenType::MUL_ASSIGN},
          {"*",                                   TokenType::MUL},
          {"//=",                                 TokenType::INTEGER_DIV_ASSIGN},
          {"//",                                  TokenType::INTEGER_DIV},
          {"/=",                                  TokenType::DIV_ASSIGN},
          {"/",                                   TokenType::DIV},
          {"++",                                  TokenType::INC},
          {"+=",                                  TokenType::ADD_ASSIGN},
          {"+",                                   TokenType::ADD},
          {"--",                                  TokenType::DEC},
          {"-=",                                  TokenType::SUB_ASSIGN},
          {"-",                                   TokenType::SUB},
          {"<<",                                  TokenType::SHIFT_LEFT},
          {"<=",                                  TokenType::LE_OP},
          {"<",                                   TokenType::LT_OP},
          {">>",                                  TokenType::SHIFT_RIGHT},
          {">=",                                  TokenType::GE_OP},
          {">",                                   TokenType::GT_OP},
          {"==",                                  TokenType::EQ_OP},
          {"=",                                   TokenType::ASSIGN},
          {"&&",                                  TokenType::LOGICAL_AND},
          {"&=",                                  TokenType::AND_ASSIGN},
          {"&",                                   TokenType::AND},
          {"||",                                  TokenType::LOGICAL_OR},
          {"|=",                                  TokenType::OR_ASSIGN},
          {"|",                                   TokenType::OR},
          {".",                                   TokenType::DOT},
          // TODO(lthomas): These can't be regexes here
          {"#.*\n",                               TokenType::COMMENT},
          {"[a-zA-Z_][a-zA-Z0-9_]*",              TokenType::IDENTIFIER},
          {"[+-]?[0-9]+",                         TokenType::NUMERIC_CONST},
          {"0[xX][a-fA-F0-9]+",                   TokenType::NUMERIC_CONST},
          {"0[oO][0-7]+",                         TokenType::NUMERIC_CONST},
          {"0[bB][01]+",                          TokenType::NUMERIC_CONST},
          {"[+-]?[0-9]+[Ee][+-]?[0-9]+",          TokenType::NUMERIC_CONST},
          {"[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)", TokenType::NUMERIC_CONST},
          {"\".*\"",                              TokenType::STRING_LITERAL},
          });
    // clang-format on

    return token_lookup;
  }

  //! @brief Get the map from Binary Operator tokens to their respective
  //! properties.
  [[nodiscard]] static BinExprMap get_bin_expr_properties_map() noexcept {
    // clang-format off
    static const BinExprMap
        bin_expr_properties{
            {TokenType::EXPONENT, {3, BinOpProperties::Associativity::RIGHT}},
            {TokenType::MUL,      {2, BinOpProperties::Associativity::LEFT}},
            {TokenType::DIV,      {2, BinOpProperties::Associativity::LEFT}},
            {TokenType::MOD,      {2, BinOpProperties::Associativity::LEFT}},
            {TokenType::ADD,      {1, BinOpProperties::Associativity::LEFT}},
            {TokenType::SUB,      {1, BinOpProperties::Associativity::LEFT}},
            {TokenType::LT_OP,    {0, BinOpProperties::Associativity::LEFT}},
            {TokenType::GT_OP,    {0, BinOpProperties::Associativity::LEFT}},
        };
    // clang-format on

    return bin_expr_properties;
  }
};

// TODO(lthomas): Potentially use template specialization for static functions
// here?
//! @brief Factory class for creating tokens.
class TokenFactory {
public:
  //! @brief Create a token.
  //! @param token_type The token type the created token should be.
  //! @return Token The created token.
  [[nodiscard]] static inline Token
  create_token(TokenType token_type) noexcept {
    try {
      std::pair<uint8_t, BinOpProperties::Associativity> bin_op_properties =
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

  //! @brief Create a token.
  //! @param token_type TokenType The token type the created token should be.
  //! @param value uint64_t The value to store in the Int Literal token.
  //! @return Token The created token.
  [[nodiscard]] static inline Token create_token(TokenType token_type,
                                                 uint64_t value) noexcept {
    return Token{.token_type = token_type,
                 .properties = IntLitProperties{.value = value}};
  }

  //! @brief Create a token.
  //! @param token_type TokenType The token type the created token should be.
  //! @param identifier std::string The identifier to store in the Identifier
  //! token.
  //! @return Token The created token.
  [[nodiscard]] static inline Token
  create_token(TokenType token_type, const std::string &identifier) noexcept {
    return Token{.token_type = token_type,
                 .properties = IdentProperties{.identifier = identifier}};
  }

private:
};
} // namespace pimento::tokenization