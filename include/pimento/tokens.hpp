//! @file tokens.hpp
//! @brief Pimento Tokens.
//! @author Logan Thomas

#pragma once

#include <bit>
#include <cmath>
#include <cstdint>
#include <optional>
#include <ranges>
#include <string>
#include <variant>

#include <iostream>

#include <boost/bimap.hpp>

namespace pimento::tokenization {

//! @brief Supported token types.
enum class TokenType : uint8_t {
  BEGIN = 0,
  AMP_EQUAL,
  AMP,
  AND,
  BOOL,
  BREAK,
  CARET_CARET_EQUAL,
  CARET_CARET,
  CARET_EQUAL,
  CARET,
  CLASS,
  COLON,
  COMMA,
  COMMENT,
  DOT,
  ELIF,
  ELSE,
  ENUM,
  EQUAL_EQUAL,
  EQUAL,
  EXCLAIM_EQUAL,
  EXIT,
  FALSE,
  FLOAT_CONST,
  FLOAT_NAN,
  FLOAT32_T_MAX,
  FLOAT32_T_MIN,
  FLOAT32_T,
  FLOAT64_T_MAX,
  FLOAT64_T_MIN,
  FLOAT64_T,
  FOR,
  FSLASH_EQUAL,
  FSLASH_FSLASH_EQUAL,
  FSLASH_FSLASH,
  FSLASH,
  FUNCTION,
  IDENT,
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
  INTEGER_CONST,
  INTERFACE,
  LANGLE_EQUAL,
  LANGLE_LANGLE,
  LANGLE,
  LEFT_CURLY,
  LEFT_PAREN,
  LEFT_SQUARE,
  MINUS_EQUAL,
  MINUS_MINUS,
  MINUS,
  MUTABLE,
  NEG_INF,
  NOT,
  NUMERIC_CONST,
  OR,
  PERCENT_EQUAL,
  PERCENT,
  PIPE_EQUAL,
  PIPE,
  PLUS_EQUAL,
  PLUS_PLUS,
  PLUS,
  POS_INF,
  PRIVATE,
  PUBLIC,
  QUESTION,
  RANGLE_EQUAL,
  RANGLE_RANGLE,
  RANGLE,
  RETURN,
  RIGHT_CURLY,
  RIGHT_PAREN,
  RIGHT_SQUARE,
  SEMI,
  STAR_EQUAL,
  STAR,
  STRING_LITERAL,
  STRING,
  TILDE_EQUAL,
  TILDE,
  TRUE,
  TYPE_IDENT,
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
  NUM_TOKENS
};

// TODO(lthomas): Not yet IEEE-754 compliant.
//! @brief Float Constant Value.
//! @details Can be a double, float, or scientific notation.
class FloatConst {
public:
  enum class Precision { FLOAT32, FLOAT64 };

  //! @brief FloatConst Constructor for value represented in scientific
  //! notation.
  //! @param mantissa uint64_t The mantissa of the value.
  //! @param exponent int The exponent of the value.
  //! @param negative bool Whether or not the value is negative.
  FloatConst(uint64_t mantissa, int exponent, bool negative,
             Precision precision)
      : m_mantissa(mantissa), m_exponent(exponent), m_negative(negative),
        m_precision(precision) {}

  //! @brief NumericConst Constructor for double-precision floating point
  //! values.
  //! @param value double The value of the floating point number.
  explicit FloatConst(double value) : m_precision(Precision::FLOAT64) {
    m_negative = std::signbit(value);
    double abs_val = std::fabs(value);
    int exp;
    double frac = std::frexp(abs_val, &exp);
    m_mantissa = static_cast<uint64_t>(frac * (1ull << FLOAT64_MANTISSA_BITS));
    m_exponent = exp - FLOAT64_MANTISSA_BITS;
  }

  //! @brief NumericConst Constructor for single-precision floating point
  //! values.
  //! @param value float The value of the floating point number.
  explicit FloatConst(float value) : m_precision(Precision::FLOAT32) {
    m_negative = std::signbit(value);
    float abs_val = std::fabs(value);
    int exp;
    float frac = std::frexp(abs_val, &exp);
    m_mantissa = static_cast<uint64_t>(frac * (1ull << FLOAT32_MANTISSA_BITS));
    m_exponent = exp - FLOAT32_MANTISSA_BITS;
  }

  //! @brief Get the value as a double precision float.
  //! @return double The value as a double precision float.
  [[nodiscard]] inline double as_float64() const noexcept {
    double result;
    switch (m_precision) {
    case Precision::FLOAT64:
      result = std::ldexp(static_cast<double>(m_mantissa), m_exponent);
      result = m_negative ? -result : result;
      break;
    case Precision::FLOAT32:
      result = static_cast<double>(as_float32());
      break;
    }
    return result;
  }
  //! @brief Get the value as a single precision float.
  //! @return float The value as a single precision float.
  [[nodiscard]] inline float as_float32() const noexcept {
    float result;
    switch (m_precision) {
    case Precision::FLOAT64:
      result = static_cast<float>(as_float64());
      break;
    case Precision::FLOAT32:
      result = std::ldexp(static_cast<double>(m_mantissa), m_exponent);
      result = m_negative ? -result : result;
      break;
    }
    return result;
  }

private:
  //! @brief The mantissa of the floating point number.
  uint64_t m_mantissa;
  //! @brief The exponent of the floating point number.
  int m_exponent;
  //! @brief Whether or not the value is negative.
  bool m_negative;
  //! @brief The precision the value was stored as.
  Precision m_precision;

  //! @brief Number of bits in a 64-bit floating point value mantissa.
  static inline constexpr uint8_t FLOAT64_MANTISSA_BITS = 53;
  //! @brief Number of bits in a 32-bit floating point value mantissa.
  static inline constexpr uint8_t FLOAT32_MANTISSA_BITS = 24;
};

#if defined(_MSC_VER)
#define PACKED_STRUCT() __pragma(pack(push, 1)) struct __pragma(pack(pop))
#else
#define PACKED_STRUCT() struct __attribute__((packed))
#endif

//! @brief Constant value as raw bits.
//! @details Generally to be used with values specified in hex, octal, or
//! binary.
class RawBits {
public:
  //! @brief RawBits Constructor.
  //! @tparam T Integral type.
  //! @param value The value to use as the raw bits.
  template <typename T>
    requires std::is_integral_v<T>
  explicit RawBits(T value) {
    if constexpr (std::is_signed_v<T>) {
      int64 = static_cast<int64_t>(value);
    } else {
      uint64 = static_cast<uint64_t>(value);
    }
  }

  // clang-format off
  //! @brief Get the raw bits as unsigned 64-bit integer.
  //! @return uint64_t The raw bits as unsigned 64-bit integer.
  [[nodiscard]] inline uint64_t as_uint64() const noexcept { return uint64; }
  //! @brief Get the raw bits as signed 64-bit integer.
  //! @return int64_t The raw bits as signed 64-bit integer.
  [[nodiscard]] inline int64_t  as_int64()  const noexcept { return int64; }
  //! @brief Get the raw bits as unsigned 32-bit integer.
  //! @return uint32_t The raw bits as unsigned 32-bit integer.
  [[nodiscard]] inline uint32_t as_uint32() const noexcept { return std::bit_cast<uint32_t>(uint32_view.value); }
  //! @brief Get the raw bits as signed 32-bit integer.
  //! @return int32_t The raw bits as signed 32-bit integer.
  [[nodiscard]] inline uint32_t as_int32()  const noexcept { return std::bit_cast<uint32_t>(int32_view.value); }
  //! @brief Get the raw bits as unsigned 16-bit integer.
  //! @return uint16_t The raw bits as unsigned 16-bit integer.
  [[nodiscard]] inline uint16_t as_uint16() const noexcept { return std::bit_cast<uint16_t>(uint16_view.value); }
  //! @brief Get the raw bits as signed 16-bit integer.
  //! @return int16_t The raw bits as signed 16-bit integer.
  [[nodiscard]] inline uint16_t as_int16()  const noexcept { return std::bit_cast<uint16_t>(int16_view.value); }
  //! @brief Get the raw bits as unsigned 8-bit integer.
  //! @return uint8_t The raw bits as unsigned 8-bit integer.
  [[nodiscard]] inline uint8_t  as_uint8()  const noexcept { return std::bit_cast<uint8_t>(uint8_view.value); }
  //! @brief Get the raw bits as signed 8-bit integer.
  //! @return int8_t The raw bits as signed 8-bit integer.
  [[nodiscard]] inline uint8_t  as_int8()   const noexcept { return std::bit_cast<uint8_t>(int8_view.value); }
  // clang-format on

private:
  //! @brief Union of signed and unsigned 64, 32, 16, and 8 bit values to be
  //! used for type punning.
  union {
    //! @brief 64-bit unsigned value.
    uint64_t uint64;
    //! @brief 64-bit signed value.
    int64_t int64;

    //! @brief View of lower 32-bits of unsigned value.
    PACKED_STRUCT() { uint32_t value; }
    uint32_view;
    //! @brief View of lower 32-bits of signed value.
    PACKED_STRUCT() { int32_t value; }
    int32_view;

    //! @brief View of lower 16-bits of unsigned value.
    PACKED_STRUCT() { uint16_t value; }
    uint16_view;
    //! @brief View of lower 16-bits of signed value.
    PACKED_STRUCT() { int16_t value; }
    int16_view;

    //! @brief View of lower 8-bits of unsigned value.
    PACKED_STRUCT() { uint8_t value; }
    uint8_view;
    //! @brief View of lower 8-bits of signed value.
    PACKED_STRUCT() { int8_t value; }
    int8_view;
  };
};

//! @brief Integer Constant value.
//! @details Can be signed or unsigned.
class IntConst {
public:
  //! @brief IntConst Constructor.
  //! @tparam T Integral type.
  //! @param value The value of the integer.
  template <typename T>
    requires std::is_integral_v<T>
  explicit IntConst(T value) {
    if constexpr (std::is_signed_v<T>) {
      int64 = static_cast<int64_t>(value);
    } else {
      uint64 = static_cast<uint64_t>(value);
    }
  }

  // clang-format off
  //! @brief Get the value as unsigned 64-bit integer.
  //! @return uint64_t The value as unsigned 64-bit integer.
  [[nodiscard]] inline uint64_t as_uint64() const noexcept { return uint64; }
  //! @brief Get the value as signed 64-bit integer.
  //! @return int64_t The value as signed 64-bit integer.
  [[nodiscard]] inline int64_t  as_int64()  const noexcept { return int64; }
  //! @brief Get the value as unsigned 32-bit integer.
  //! @return uint32_t The value as unsigned 32-bit integer.
  [[nodiscard]] inline uint32_t as_uint32() const noexcept { return static_cast<uint32_t>(uint64); }
  //! @brief Get the value as signed 32-bit integer.
  //! @return int32_t The value as signed 32-bit integer.
  [[nodiscard]] inline uint32_t as_int32()  const noexcept { return static_cast<uint32_t>(int64); }
  //! @brief Get the value as unsigned 16-bit integer.
  //! @return uint16_t The value as unsigned 16-bit integer.
  [[nodiscard]] inline uint16_t as_uint16() const noexcept { return static_cast<uint16_t>(uint64); }
  //! @brief Get the value as signed 16-bit integer.
  //! @return int16_t The value as signed 16-bit integer.
  [[nodiscard]] inline uint16_t as_int16()  const noexcept { return static_cast<uint16_t>(int64); }
  //! @brief Get the value as unsigned 8-bit integer.
  //! @return uint8_t The value as unsigned 8-bit integer.
  [[nodiscard]] inline uint8_t  as_uint8()  const noexcept { return static_cast<uint8_t>(uint64); }
  //! @brief Get the value as signed 8-bit integer.
  //! @return int8_t The value as signed 8-bit integer.
  [[nodiscard]] inline uint8_t  as_int8()   const noexcept { return static_cast<uint8_t>(int64); }
  // clang-format on

private:
  union {
    uint64_t uint64;
    int64_t int64;
  };
};

//! @brief Numeric Constant value.
//! @details Can be floating point number, integer, or raw bits.
class NumericConst {
public:
  using Value = std::variant<IntConst, FloatConst, RawBits>;

  //! @brief NumericConst Constructor for integer value.
  //! @tparam T Integral type.
  //! @param value The value of the integer.
  template <typename T>
    requires std::is_integral_v<T>
  explicit NumericConst(T value) : m_value(IntConst(value)) {}

  // TODO(lthomas): Not sure if I should always be creating these as float64
  // precision, I think this has the potential for a double round error.
  //! @brief NumericConst Constructor for floating point value represented in
  //! scientific notation.
  //! @param mantissa uint64_t The mantissa of the floating point value.
  //! @param exponent int The exponent of the floating point value.
  //! @param negative bool Whether or not the value is negative.
  NumericConst(uint64_t mantissa, int exponent, bool negative = false)
      : m_value(FloatConst{mantissa, exponent, negative,
                           FloatConst::Precision::FLOAT64}) {}

  //! @brief NumericConst Constructor for value specified in raw bits.
  //! @param raw const RawBits& The raw bits from which to construct the value.
  explicit NumericConst(const RawBits &raw) : m_value(std::move(raw)) {}

  //! @brief NumericConst Constructor for value specified as FloatConst.
  //! @param raw const FloatConst& The floating point constant from which to
  //! construct the value.
  explicit NumericConst(const FloatConst &float_const)
      : m_value(std::move(float_const)) {}

  //! @brief NumericConst Constructor for double-precision floating point
  //! values.
  //! @param value double The value of the floating point number.
  explicit NumericConst(double value) : m_value(FloatConst(value)) {}

  //! @brief NumericConst Constructor for single-precision floating point
  //! values.
  //! @param value float The value of the floating point number.
  explicit NumericConst(float value) : m_value(FloatConst(value)) {}

  //! @brief Get the value as a double-precision (64-bit) floating point number.
  //! @return double The value as a double-precision (64-bit) floating point
  //! number.
  double as_float64() const {
    return std::visit(
        [](auto &&v) -> double {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return std::bit_cast<double>(v.as_uint64());
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return v.as_float64();
          } else {
            // std::cout << "IntConst" << std::endl;
            return static_cast<double>(v.as_int64());
          }
        },
        m_value);
  }

  //! @brief Get the value as a single-precision (32-bit) floating point number.
  //! @return float The value as a single-precision (32-bit) floating point
  //! number.
  float as_float32() const {
    return std::visit(
        [](auto &&v) -> float {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return std::bit_cast<float>(v.as_uint32());
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return v.as_float32();
          } else {
            // std::cout << "IntConst" << std::endl;
            return static_cast<float>(v.as_int64());
          }
        },
        m_value);
  }

  //! @brief Get the value as unsigned 64-bit integer.
  //! @return uint64_t The value as unsigned 64-bit integer.
  uint64_t as_uint64() const {
    return std::visit(
        [](auto &&v) -> uint64_t {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return v.as_uint64();
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return static_cast<uint64_t>(v.as_float64());
          } else {
            // std::cout << "IntConst" << std::endl;
            return v.as_uint64();
          }
        },
        m_value);
  }

  //! @brief Get the value as signed 64-bit integer.
  //! @return int64_t The value as signed 64-bit integer.
  int64_t as_int64() const {
    return std::visit(
        [](auto &&v) -> int64_t {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return v.as_int64();
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return static_cast<int64_t>(v.as_float64());
          } else {
            // std::cout << "IntConst" << std::endl;
            return v.as_int64();
          }
        },
        m_value);
  }

  //! @brief Get the value as unsigned 32-bit integer.
  //! @return uint32_t The value as unsigned 32-bit integer.
  uint32_t as_uint32() const {
    return std::visit(
        [](auto &&v) -> uint32_t {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return v.as_uint32();
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return static_cast<uint32_t>(v.as_float64());
          } else {
            // std::cout << "IntConst" << std::endl;
            return v.as_uint32();
          }
        },
        m_value);
  }

  //! @brief Get the value as signed 32-bit integer.
  //! @return int32_t The value as signed 32-bit integer.
  int32_t as_int32() const {
    return std::visit(
        [](auto &&v) -> int32_t {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return v.as_int32();
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return static_cast<int32_t>(v.as_float64());
          } else {
            // std::cout << "IntConst" << std::endl;
            return v.as_int32();
          }
        },
        m_value);
  }

  //! @brief Get the value as unsigned 16-bit integer.
  //! @return uint16_t The value as unsigned 16-bit integer.
  uint16_t as_uint16() const {
    return std::visit(
        [](auto &&v) -> uint16_t {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return v.as_uint16();
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return static_cast<uint16_t>(v.as_float64());
          } else {
            // std::cout << "IntConst" << std::endl;
            return v.as_uint16();
          }
        },
        m_value);
  }

  //! @brief Get the value as signed 16-bit integer.
  //! @return int16_t The value as signed 16-bit integer.
  int16_t as_int16() const {
    return std::visit(
        [](auto &&v) -> int16_t {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return v.as_int16();
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return static_cast<int16_t>(v.as_float64());
          } else {
            // std::cout << "IntConst" << std::endl;
            return v.as_int16();
          }
        },
        m_value);
  }

  //! @brief Get the value as unsigned 8-bit integer.
  //! @return uint8_t The value as unsigned 8-bit integer.
  uint8_t as_uint8() const {
    return std::visit(
        [](auto &&v) -> uint8_t {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return v.as_uint8();
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return static_cast<uint8_t>(v.as_float64());
          } else {
            // std::cout << "IntConst" << std::endl;
            return v.as_uint8();
          }
        },
        m_value);
  }

  //! @brief Get the value as signed 8-bit integer.
  //! @return int8_t The value as signed 8-bit integer.
  int8_t as_int8() const {
    return std::visit(
        [](auto &&v) -> int8_t {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, RawBits>) {
            // std::cout << "RawBits" << std::endl;
            return v.as_int8();
          } else if constexpr (std::is_same_v<T, FloatConst>) {
            // std::cout << "FloatConst" << std::endl;
            return static_cast<int8_t>(v.as_float64());
          } else {
            // std::cout << "IntConst" << std::endl;
            return v.as_int8();
          }
        },
        m_value);
  }

private:
  Value m_value;
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