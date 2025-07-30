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

//! @brief Supported token types.
enum class TokenType : uint8_t {
  BEGIN = 0,
  BOOL,
  BREAK,
  CLASS,
  ELIF,
  ELSE,
  ENUM,
  EXIT,
  FALSE,
  FLOAT_NAN,
  FLOAT32_T_MAX,
  FLOAT32_T_MIN,
  FLOAT32_T,
  FLOAT64_T_MAX,
  FLOAT64_T_MIN,
  FLOAT64_T,
  FOR,
  FUNCTION,
  IF,
  IN,
  INT16_T_MAX,
  INT16_T_MIN,
  INT16_T,
  INT32_T_MAX,
  INT32_T_MIN,
  INT32_T,
  INT64_T_MAX,
  INT64_T_MIN,
  INT64_T,
  INT8_T_MAX,
  INT8_T_MIN,
  INT8_T,
  INTERFACE,
  LOGICAL_AND,
  LOGICAL_OR,
  MUTABLE,
  NEG_INF,
  POS_INF,
  PRIVATE,
  PUBLIC,
  RETURN,
  STRING,
  TRUE,
  UINT16_T_MAX,
  UINT16_T_MIN,
  UINT16_T,
  UINT32_T_MAX,
  UINT32_T_MIN,
  UINT32_T,
  UINT64_T_MAX,
  UINT64_T_MIN,
  UINT64_T,
  UINT8_T_MAX,
  UINT8_T_MIN,
  UINT8_T,
  WHILE,
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
  NE_OP,
  EQ_OP,
  ASSIGN,
  BIT_NOT_ASSIGN,
  BIT_NOT,
  AND_ASSIGN,
  AND,
  OR_ASSIGN,
  OR,
  TERNARY,
  DOT,
  COMMA,
  SEMI,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_SQUARE,
  RIGHT_SQUARE,
  LEFT_CURLY,
  RIGHT_CURLY,
  IDENT,
  TYPE_IDENT,
  INTEGER_CONST,
  NUMERIC_CONST,
  FLOAT_CONST,
  STRING_LITERAL,
  COMMENT,
  NUM_TOKENS
};

#if defined(_MSC_VER)
    #define PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#else
    #define PACKED_STRUCT(name) struct __attribute__((packed)) name
#endif

//! @brief 64-bit integers with the 32-bit, 16-bit, and 8-bit representations packed.
union NumericConst
{
  //! @brief Get the value as a double precision float.
  //! @return double The value as a double precision float.
  [[nodiscard]] inline double   as_float64() { return float64; }
  //! @brief Get the value as a single precision float.
  //! @return float The value as a single precision float.
  [[nodiscard]] inline float    as_float32() { return static_cast<float>(float64); }

  //! @brief Get the value as a double precision float assuming the value was stored as an integer.
  //! @return double The value as a double precision float.
  [[nodiscard]] inline double   as_float64_from_int() { return static_cast<double>(uint64); }
  //! @brief Get the value as a single precision float assuming the value was stored as an integer.
  //! @return float The value as a single precision float.
  [[nodiscard]] inline float    as_float32_from_int() { return static_cast<float>(uint64); }

  //! @brief Get the value as a 64-bit unsigned integer.
  //! @return uint64_t The value as a 64-bit unsigned integer.
  [[nodiscard]] inline uint64_t as_uint64()  { return uint64; }
  //! @brief Get the value as a 64-bit signed integer.
  //! @return uint64_t The value as a 64-bit signed integer.
  [[nodiscard]] inline int64_t  as_int64()   { return reinterpret_cast<int64_t&>(uint64); }

  //! @brief Get the value as a 32-bit unsigned integer.
  //! @return uint32_t The value as a 32-bit unsigned integer.
  [[nodiscard]] inline uint32_t as_uint32()  { return uint32_view.value; }
  //! @brief Get the value as a 32-bit signed integer.
  //! @return uint32_t The value as a 32-bit signed integer.
  [[nodiscard]] inline int32_t  as_int32()   { return reinterpret_cast<int32_t&>(uint32_view.value); }

  //! @brief Get the value as a 16-bit unsigned integer.
  //! @return uint16_t The value as a 16-bit unsigned integer.
  [[nodiscard]] inline uint16_t as_uint16()  { return uint16_view.value; }
  //! @brief Get the value as a 16-bit signed integer.
  //! @return uint16_t The value as a 16-bit signed integer.
  [[nodiscard]] inline int16_t  as_int16()   { return reinterpret_cast<int16_t&>(uint16_view.value);}

  //! @brief Get the value as an 8-bit unsigned integer.
  //! @return uint8_t The value as an 8-bit unsigned integer.
  [[nodiscard]] inline uint8_t  as_uint8()   { return uint8_view.value; }
  //! @brief Get the value as an 8-bit signed integer.
  //! @return uint8_t The value as an 8-bit signed integer.
  [[nodiscard]] inline int8_t   as_int8()    { return reinterpret_cast<int8_t&>(uint8_view.value); }

  // Explicit overloads to make use of C++ built-in sign extension
  void operator=(double                 value) { float64 = value; }
  void operator=(float                  value) { float64 = value; }
  void operator=(long long unsigned int value) { uint64  = value; }
  void operator=(long unsigned int      value) { uint64  = value; }
  void operator=(unsigned int           value) { uint64  = value; }
  void operator=(unsigned short         value) { uint64  = value; }
  void operator=(unsigned char          value) { uint64  = value; }
  void operator=(long long signed int   value) { uint64  = value; }
  void operator=(long signed int        value) { uint64  = value; }
  void operator=(signed int             value) { uint64  = value; }
  void operator=(signed short           value) { uint64  = value; }
  void operator=(signed char            value) { uint64  = value; }

private:
  uint64_t uint64;
  double   float64;

  PACKED_STRUCT(uint32Struct) {
    uint32_t value;
  } uint32_view;

  PACKED_STRUCT(uint16Struct) {
    uint16_t value;
  } uint16_view;

  PACKED_STRUCT(uint8Struct) {
    uint8_t value;
  } uint8_view;
};

//! @brief Parsed token.
struct Token {
  //! @brief The type of this token.
  TokenType token_type;
  //! @brief The literal string parsed to get the token.
  std::string lexeme;
  //! @brief The line and column number of the token.
  std::pair<unsigned, unsigned> source_span;
};

//! @brief Token for representing numeric constants.
struct NumConstToken : public Token {
  //! @brief The value of the floating point number.
  NumericConst value;
};

//! @brief Token for representing string values.
//! This could be string literals, identifiers, or type identifiers.
struct StringToken : public Token {
  std::string value;
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
        {TokenType::LOGICAL_AND,        "LOGICAL_AND"},
        {TokenType::BOOL,               "BOOL"},
        {TokenType::BREAK,              "BREAK"},
        {TokenType::CLASS,              "CLASS"},
        {TokenType::ELIF,               "ELIF"},
        {TokenType::ELSE,               "ELSE"},
        {TokenType::ENUM,               "ENUM"},
        {TokenType::EXIT,               "EXIT"},
        {TokenType::FALSE,              "FALSE"},
        {TokenType::FLOAT32,            "FLOAT32"},
        {TokenType::FLOAT64,            "FLOAT64"},
        {TokenType::FOR,                "FOR"},
        {TokenType::FUNCTION,           "FUNCTION"},
        {TokenType::IF,                 "IF"},
        {TokenType::INTERFACE,          "INTERFACE"},
        {TokenType::IN,                 "IN"},
        {TokenType::INT16,              "INT16"},
        {TokenType::INT32,              "INT32"},
        {TokenType::INT64,              "INT64"},
        {TokenType::INT8,               "INT8"},
        {TokenType::MUTABLE,            "MUTABLE"},
        {TokenType::LOGICAL_OR,         "LOGICAL_OR"},
        {TokenType::PRIVATE,            "PRIVATE"},
        {TokenType::PUBLIC,             "PUBLIC"},
        {TokenType::RETURN,             "RETURN"},
        {TokenType::STRING,             "STRING"},
        {TokenType::TRUE,               "TRUE"},
        {TokenType::UINT16,             "UINT16"},
        {TokenType::UINT32,             "UINT32"},
        {TokenType::UINT64,             "UINT64"},
        {TokenType::UINT8,              "UINT8"},
        {TokenType::WHILE,              "WHILE"},
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
        {TokenType::NE_OP,              "NE_OP"},
        {TokenType::EQ_OP,              "EQ_OP"},
        {TokenType::ASSIGN,             "ASSIGN"},
        {TokenType::BIT_NOT_ASSIGN,     "BIT_NOT_ASSIGN"},
        {TokenType::BIT_NOT,            "BIT_NOT"},
        {TokenType::AND_ASSIGN,         "AND_ASSIGN"},
        {TokenType::AND,                "AND"},
        {TokenType::OR_ASSIGN,          "OR_ASSIGN"},
        {TokenType::OR,                 "OR"},
        {TokenType::TERNARY,            "TERNARY"},
        {TokenType::DOT,                "DOT"},
        {TokenType::COMMA,              "COMMA"},
        {TokenType::SEMI,               "SEMI"},
        {TokenType::LEFT_PAREN,         "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN,        "RIGHT_PAREN"},
        {TokenType::LEFT_SQUARE,        "LEFT_SQUARE"},
        {TokenType::RIGHT_SQUARE,       "RIGHT_SQUARE"},
        {TokenType::LEFT_CURLY,         "LEFT_CURLY"},
        {TokenType::RIGHT_CURLY,        "RIGHT_CURLY"},
        {TokenType::IDENT,              "IDENT"},
        {TokenType::TYPE_IDENT,         "TYPE_IDENT"},
        {TokenType::INTEGER_CONST,      "INTEGER_CONST"},
        {TokenType::NUMERIC_CONST,      "NUMERIC_CONST"},
        {TokenType::FLOAT_CONST,        "FLOAT_CONST"},
        {TokenType::STRING_LITERAL,     "STRING_LITERAL"},
        {TokenType::COMMENT,            "COMMENT"}
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
          {"and",     TokenType::LOGICAL_AND},
          {"bool",    TokenType::BOOL},
          {"break",   TokenType::BREAK},
          {"class",   TokenType::CLASS},
          {"elif",    TokenType::ELIF},
          {"else",    TokenType::ELSE},
          {"enum",    TokenType::ENUM},
          {"exit",    TokenType::EXIT},
          {"false",   TokenType::FALSE},
          {"float32", TokenType::FLOAT32},
          {"float64", TokenType::FLOAT64},
          {"for",     TokenType::FOR},
          {"func",    TokenType::FUNCTION},
          {"if",      TokenType::IF},
          {"iface",   TokenType::INTERFACE},
          {"in",      TokenType::IN},
          {"int16",   TokenType::INT16},
          {"int32",   TokenType::INT32},
          {"int64",   TokenType::INT64},
          {"int8",    TokenType::INT8},
          {"mut",     TokenType::MUTABLE},
          {"or",      TokenType::LOGICAL_OR},
          {"private", TokenType::PRIVATE},
          {"public",  TokenType::PUBLIC},
          {"return",  TokenType::RETURN},
          {"string",  TokenType::STRING},
          {"true",    TokenType::TRUE},
          {"uint16",  TokenType::UINT16},
          {"uint32",  TokenType::UINT32},
          {"uint64",  TokenType::UINT64},
          {"uint8",   TokenType::UINT8},
          {"while",   TokenType::WHILE},
          {"^^=",     TokenType::EXPONENT_ASSIGN},
          {"^^",      TokenType::EXPONENT},
          {"^=",      TokenType::XOR_ASSIGN},
          {"^",       TokenType::XOR},
          {"%=",      TokenType::MOD_ASSIGN},
          {"%",       TokenType::MOD},
          {"*=",      TokenType::MUL_ASSIGN},
          {"*",       TokenType::MUL},
          {"//=",     TokenType::INTEGER_DIV_ASSIGN},
          {"//",      TokenType::INTEGER_DIV},
          {"/=",      TokenType::DIV_ASSIGN},
          {"/",       TokenType::DIV},
          {"++",      TokenType::INC},
          {"+=",      TokenType::ADD_ASSIGN},
          {"+",       TokenType::ADD},
          {"--",      TokenType::DEC},
          {"-=",      TokenType::SUB_ASSIGN},
          {"-",       TokenType::SUB},
          {"<<",      TokenType::SHIFT_LEFT},
          {"<=",      TokenType::LE_OP},
          {"<",       TokenType::LT_OP},
          {">>",      TokenType::SHIFT_RIGHT},
          {">=",      TokenType::GE_OP},
          {">",       TokenType::GT_OP},
          {"!=",      TokenType::NE_OP},
          {"==",      TokenType::EQ_OP},
          {"=",       TokenType::ASSIGN},
          {"~=",      TokenType::BIT_NOT_ASSIGN},
          {"~",       TokenType::BIT_NOT},
          {"&=",      TokenType::AND_ASSIGN},
          {"&",       TokenType::AND},
          {"|=",      TokenType::OR_ASSIGN},
          {"|",       TokenType::OR},
          {"?",       TokenType::TERNARY},
          {".",       TokenType::DOT},
          {",",       TokenType::COMMA},
          {";",       TokenType::SEMI},
          {"(",       TokenType::LEFT_PAREN},
          {")",       TokenType::RIGHT_PAREN},
          {"[",       TokenType::LEFT_SQUARE},
          {"]",       TokenType::RIGHT_SQUARE},
          {"{",       TokenType::LEFT_CURLY},
          {"}",       TokenType::RIGHT_CURLY},
          {"",        TokenType::IDENT},
          {"",        TokenType::TYPE_IDENT},
          {"",        TokenType::INTEGER_CONST},
          {"",        TokenType::NUMERIC_CONST},
          {"",        TokenType::FLOAT_CONST},
          {"",        TokenType::STRING_LITERAL},
          {"",        TokenType::COMMENT}
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