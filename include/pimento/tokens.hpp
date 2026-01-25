//! @file tokens.hpp
//! @brief Pimento Tokens.
//! @author Logan Thomas

#pragma once

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

#include <bit>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

namespace pimento::tokenization {

//! @brief Supported token types.
enum class TokenType : uint8_t
{
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
class FloatConst
{
   public:
    enum class Precision
    {
        FLOAT32,
        FLOAT64
    };

    //! @brief Constructor for value represented in scientific notation.
    //! @param mantissa The mantissa of the value.
    //! @param exponent The exponent of the value.
    //! @param negative Whether or not the value is negative.
    FloatConst(uint64_t mantissa, int exponent, bool negative, Precision precision)
        : m_mantissa(mantissa)
        , m_exponent(exponent)
        , m_negative(negative)
        , m_precision(precision)
    {}

    //! @brief Constructor for double-precision floating point values.
    //! @param value The value of the floating point number.
    explicit FloatConst(double value)
        : m_precision(Precision::FLOAT64)
    {
        m_negative     = std::signbit(value);
        double abs_val = std::fabs(value);
        int exp;
        double frac = std::frexp(abs_val, &exp);
        m_mantissa  = static_cast<uint64_t>(frac * (1ull << FLOAT64_MANTISSA_BITS));
        m_exponent  = exp - FLOAT64_MANTISSA_BITS;
    }

    //! @brief Constructor for single-precision floating point values.
    //! @param value The value of the floating point number.
    explicit FloatConst(float value)
        : m_precision(Precision::FLOAT32)
    {
        m_negative    = std::signbit(value);
        float abs_val = std::fabs(value);
        int exp;
        float frac = std::frexp(abs_val, &exp);
        m_mantissa = static_cast<uint64_t>(frac * (1ull << FLOAT32_MANTISSA_BITS));
        m_exponent = exp - FLOAT32_MANTISSA_BITS;
    }

   public:
    //! @brief Get the value as a double precision float.
    //! @return The value as a double precision float.
    [[nodiscard]] inline double as_float64() const noexcept
    {
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
    //! @return The value as a single precision float.
    [[nodiscard]] inline float as_float32() const noexcept
    {
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
class RawBits
{
   public:
    //! @brief Constructor.
    //! @tparam T Integral type.
    //! @param value The value to use as the raw bits.
    template <typename T>
        requires std::is_integral_v<T>
    explicit RawBits(T value)
    {
        if constexpr (std::is_signed_v<T>) {
            int64 = static_cast<int64_t>(value);
        }
        else {
            uint64 = static_cast<uint64_t>(value);
        }
    }

   public:
    // clang-format off
  //! @brief Get the raw bits as unsigned 64-bit integer.
  //! @return The raw bits as unsigned 64-bit integer.
  [[nodiscard]] inline uint64_t as_uint64() const noexcept { return uint64; }
  //! @brief Get the raw bits as signed 64-bit integer.
  //! @return The raw bits as signed 64-bit integer.
  [[nodiscard]] inline int64_t  as_int64()  const noexcept { return int64; }
  //! @brief Get the raw bits as unsigned 32-bit integer.
  //! @return The raw bits as unsigned 32-bit integer.
  [[nodiscard]] inline uint32_t as_uint32() const noexcept { return uint32_view.value; }
  //! @brief Get the raw bits as signed 32-bit integer.
  //! @return The raw bits as signed 32-bit integer.
  [[nodiscard]] inline int32_t as_int32()  const noexcept { return int32_view.value; }
  //! @brief Get the raw bits as unsigned 16-bit integer.
  //! @return The raw bits as unsigned 16-bit integer.
  [[nodiscard]] inline uint16_t as_uint16() const noexcept { return uint16_view.value; }
  //! @brief Get the raw bits as signed 16-bit integer.
  //! @return The raw bits as signed 16-bit integer.
  [[nodiscard]] inline int16_t as_int16()  const noexcept { return int16_view.value; }
  //! @brief Get the raw bits as unsigned 8-bit integer.
  //! @return The raw bits as unsigned 8-bit integer.
  [[nodiscard]] inline uint8_t  as_uint8()  const noexcept { return uint8_view.value; }
  //! @brief Get the raw bits as signed 8-bit integer.
  //! @return The raw bits as signed 8-bit integer.
  [[nodiscard]] inline int8_t  as_int8()   const noexcept { return int8_view.value; }

    // clang-format on

   private:
    //! @brief Union of signed and unsigned 64, 32, 16, and 8 bit values to be
    //! used for type punning.
    union
    {
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
class IntConst
{
   public:
    //! @brief Constructor.
    //! @tparam T Integral type.
    //! @param value The value of the integer.
    template <typename T>
        requires std::is_integral_v<T>
    explicit IntConst(T value)
    {
        if constexpr (std::is_signed_v<T>) {
            int64 = static_cast<int64_t>(value);
        }
        else {
            uint64 = static_cast<uint64_t>(value);
        }
    }

   public:
    // clang-format off
  //! @brief Get the value as unsigned 64-bit integer.
  //! @return The value as unsigned 64-bit integer.
  [[nodiscard]] inline uint64_t as_uint64() const noexcept { return uint64; }
  //! @brief Get the value as signed 64-bit integer.
  //! @return The value as signed 64-bit integer.
  [[nodiscard]] inline int64_t  as_int64()  const noexcept { return int64; }
  //! @brief Get the value as unsigned 32-bit integer.
  //! @return The value as unsigned 32-bit integer.
  [[nodiscard]] inline uint32_t as_uint32() const noexcept { return static_cast<uint32_t>(uint64); }
  //! @brief Get the value as signed 32-bit integer.
  //! @return The value as signed 32-bit integer.
  [[nodiscard]] inline int32_t as_int32()  const noexcept { return static_cast<int32_t>(int64); }
  //! @brief Get the value as unsigned 16-bit integer.
  //! @return The value as unsigned 16-bit integer.
  [[nodiscard]] inline uint16_t as_uint16() const noexcept { return static_cast<uint16_t>(uint64); }
  //! @brief Get the value as signed 16-bit integer.
  //! @return The value as signed 16-bit integer.
  [[nodiscard]] inline int16_t as_int16()  const noexcept { return static_cast<int16_t>(int64); }
  //! @brief Get the value as unsigned 8-bit integer.
  //! @return The value as unsigned 8-bit integer.
  [[nodiscard]] inline uint8_t  as_uint8()  const noexcept { return static_cast<uint8_t>(uint64); }
  //! @brief Get the value as signed 8-bit integer.
  //! @return The value as signed 8-bit integer.
  [[nodiscard]] inline int8_t  as_int8()   const noexcept { return static_cast<int8_t>(int64); }

    // clang-format on

   private:
    //! @brief Union of signed and signed and unsigned 64-bit values to be used
    //! for type punning.
    union
    {
        //! @brief 64-bit unsigned value.
        uint64_t uint64;
        //! @brief 64-bit signed value.
        int64_t int64;
    };
};

//! @brief Numeric Constant value.
//! @details Can be floating point number, integer, or raw bits.
class NumericConst
{
   public:
    using Value = std::variant<IntConst, FloatConst, RawBits>;

    //! @brief Constructor for integer value.
    //! @tparam T Integral type.
    //! @param value The value of the integer.
    template <typename T>
        requires std::is_integral_v<T>
    explicit NumericConst(T value)
        : m_value(IntConst(value))
    {}

    // TODO(lthomas): Not sure if I should always be creating these as
    // double-precision float, I think this has the potential for a double round
    // error.
    //! @brief Constructor for floating point value represented in scientific
    //! notation.
    //! @param mantissa The mantissa of the floating point value.
    //! @param exponent The exponent of the floating point value.
    //! @param negative Whether or not the value is negative.
    NumericConst(uint64_t mantissa, int exponent, bool negative = false)
        : m_value(FloatConst{mantissa, exponent, negative, FloatConst::Precision::FLOAT64})
    {}

    //! @brief Constructor for value specified in raw bits.
    //! @param raw The raw bits from which to construct the value.
    explicit NumericConst(const RawBits& raw)
        : m_value(std::move(raw))
    {}

    //! @brief Constructor for value specified as FloatConst.
    //! @param raw The floating point constant from which to construct the value.
    explicit NumericConst(const FloatConst& float_const)
        : m_value(std::move(float_const))
    {}

    //! @brief Constructor for double-precision floating point values.
    //! @param value The value of the floating point number.
    explicit NumericConst(double value)
        : m_value(FloatConst(value))
    {}

    //! @brief Constructor for single-precision floating point values.
    //! @param value The value of the floating point number.
    explicit NumericConst(float value)
        : m_value(FloatConst(value))
    {}

   public:
    //! @brief Get the value as a double-precision (64-bit) floating point number.
    //! @return The value as a double-precision (64-bit) floating point number.
    double as_float64() const
    {
        return std::visit(
            [](auto&& v) -> double {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return std::bit_cast<double>(v.as_uint64());
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return v.as_float64();
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return static_cast<double>(v.as_int64());
                }
            },
            m_value);
    }

    //! @brief Get the value as a single-precision (32-bit) floating point number.
    //! @return The value as a single-precision (32-bit) floating point number.
    float as_float32() const
    {
        return std::visit(
            [](auto&& v) -> float {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return std::bit_cast<float>(v.as_uint32());
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return v.as_float32();
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return static_cast<float>(v.as_int64());
                }
            },
            m_value);
    }

    //! @brief Get the value as unsigned 64-bit integer.
    //! @return The value as unsigned 64-bit integer.
    uint64_t as_uint64() const
    {
        return std::visit(
            [](auto&& v) -> uint64_t {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return v.as_uint64();
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<uint64_t>(v.as_float64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return v.as_uint64();
                }
            },
            m_value);
    }

    //! @brief Get the value as signed 64-bit integer.
    //! @return The value as signed 64-bit integer.
    int64_t as_int64() const
    {
        return std::visit(
            [](auto&& v) -> int64_t {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return v.as_int64();
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<int64_t>(v.as_float64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return v.as_int64();
                }
            },
            m_value);
    }

    //! @brief Get the value as unsigned 32-bit integer.
    //! @return The value as unsigned 32-bit integer.
    uint32_t as_uint32() const
    {
        return std::visit(
            [](auto&& v) -> uint32_t {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return v.as_uint32();
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<uint32_t>(v.as_float64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return v.as_uint32();
                }
            },
            m_value);
    }

    //! @brief Get the value as signed 32-bit integer.
    //! @return The value as signed 32-bit integer.
    int32_t as_int32() const
    {
        return std::visit(
            [](auto&& v) -> int32_t {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return v.as_int32();
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<int32_t>(v.as_float64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return v.as_int32();
                }
            },
            m_value);
    }

    //! @brief Get the value as unsigned 16-bit integer.
    //! @return The value as unsigned 16-bit integer.
    uint16_t as_uint16() const
    {
        return std::visit(
            [](auto&& v) -> uint16_t {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return v.as_uint16();
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<uint16_t>(v.as_float64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return v.as_uint16();
                }
            },
            m_value);
    }

    //! @brief Get the value as signed 16-bit integer.
    //! @return The value as signed 16-bit integer.
    int16_t as_int16() const
    {
        return std::visit(
            [](auto&& v) -> int16_t {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return v.as_int16();
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<int16_t>(v.as_float64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return v.as_int16();
                }
            },
            m_value);
    }

    //! @brief Get the value as unsigned 8-bit integer.
    //! @return The value as unsigned 8-bit integer.
    uint8_t as_uint8() const
    {
        return std::visit(
            [](auto&& v) -> uint8_t {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return v.as_uint8();
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<uint8_t>(v.as_float64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return v.as_uint8();
                }
            },
            m_value);
    }

    //! @brief Get the value as signed 8-bit integer.
    //! @return The value as signed 8-bit integer.
    int8_t as_int8() const
    {
        return std::visit(
            [](auto&& v) -> int8_t {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return v.as_int8();
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<int8_t>(v.as_float64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return v.as_int8();
                }
            },
            m_value);
    }

   private:
    //! @brief The numeric constant value.
    Value m_value;
};

//! @brief Static utility class for interacting with token types.
class TokenTypeUtil
{
   public:
    TokenTypeUtil() = delete;

    //! @brief Get a human-readable string representing the token.
    //! @param token_type The token type for which to return the string.
    //! @return The human-readable string representation of the token.
    [[nodiscard]] static inline std::string get_type_as_str(TokenType token_type)
    {
        try {
            return get_token_str_map().at(token_type);
        } catch (const std::out_of_range&) {
            return "UNKNOWN_TOKEN";
        }
    }

   private:
    using TokenStrMap = std::unordered_map<TokenType, std::string>;

    //! @brief Get the map from token types to human-readable strings.
    [[nodiscard]] static TokenStrMap get_token_str_map() noexcept
    {
        // clang-format off
    static const TokenStrMap token_str{
      {TokenType::AMP_EQUAL,           "AMP_EQUAL"},
      {TokenType::AMP,                 "AMP"},
      {TokenType::AND,                 "AND"},
      {TokenType::BOOL,                "BOOL"},
      {TokenType::BREAK,               "BREAK"},
      {TokenType::CARET_CARET_EQUAL,   "CARET_CARET_EQUAL"},
      {TokenType::CARET_CARET,         "CARET_CARET"},
      {TokenType::CARET_EQUAL,         "CARET_EQUAL"},
      {TokenType::CARET,               "CARET"},
      {TokenType::CLASS,               "CLASS"},
      {TokenType::COLON,               "COLON"},
      {TokenType::COMMA,               "COMMA"},
      {TokenType::COMMENT,             "COMMENT"},
      {TokenType::DOT,                 "DOT"},
      {TokenType::ELIF,                "ELIF"},
      {TokenType::ELSE,                "ELSE"},
      {TokenType::ENUM,                "ENUM"},
      {TokenType::EQUAL_EQUAL,         "EQUAL_EQUAL"},
      {TokenType::EQUAL,               "EQUAL"},
      {TokenType::EXCLAIM_EQUAL,       "EXCLAIM_EQUAL"},
      {TokenType::EXIT,                "EXIT"},
      {TokenType::FALSE,               "FALSE"},
      {TokenType::FLOAT_CONST,         "FLOAT_CONST"},
      {TokenType::FLOAT_NAN,           "FLOAT_NAN"},
      {TokenType::FLOAT32_T_MAX,       "FLOAT32_T_MAX"},
      {TokenType::FLOAT32_T_MIN,       "FLOAT32_T_MIN"},
      {TokenType::FLOAT32_T,           "FLOAT32_T"},
      {TokenType::FLOAT64_T_MAX,       "FLOAT64_T_MAX"},
      {TokenType::FLOAT64_T_MIN,       "FLOAT64_T_MIN"},
      {TokenType::FLOAT64_T,           "FLOAT64_T"},
      {TokenType::FOR,                 "FOR"},
      {TokenType::FSLASH_EQUAL,        "FSLASH_EQUAL"},
      {TokenType::FSLASH_FSLASH_EQUAL, "FSLASH_FSLASH_EQUAL"},
      {TokenType::FSLASH_FSLASH,       "FSLASH_FSLASH"},
      {TokenType::FSLASH,              "FSLASH"},
      {TokenType::FUNCTION,            "FUNCTION"},
      {TokenType::IDENT,               "IDENT"},
      {TokenType::IF,                  "IF"},
      {TokenType::IN,                  "IN"},
      {TokenType::INT16_T_MAX,         "INT16_T_MAX"},
      {TokenType::INT16_T_MIN,         "INT16_T_MIN"},
      {TokenType::INT16_T,             "INT16_T"},
      {TokenType::INT32_T_MAX,         "INT32_T_MAX"},
      {TokenType::INT32_T_MIN,         "INT32_T_MIN"},
      {TokenType::INT32_T,             "INT32_T"},
      {TokenType::INT64_T_MAX,         "INT64_T_MAX"},
      {TokenType::INT64_T_MIN,         "INT64_T_MIN"},
      {TokenType::INT64_T,             "INT64_T"},
      {TokenType::INT8_T_MAX,          "INT8_T_MAX"},
      {TokenType::INT8_T_MIN,          "INT8_T_MIN"},
      {TokenType::INT8_T,              "INT8_T"},
      {TokenType::INTEGER_CONST,       "INTEGER_CONST"},
      {TokenType::INTERFACE,           "INTERFACE"},
      {TokenType::LANGLE_EQUAL,        "LANGLE_EQUAL"},
      {TokenType::LANGLE_LANGLE,       "LANGLE_LANGLE"},
      {TokenType::LANGLE,              "LANGLE"},
      {TokenType::LEFT_CURLY,          "LEFT_CURLY"},
      {TokenType::LEFT_PAREN,          "LEFT_PAREN"},
      {TokenType::LEFT_SQUARE,         "LEFT_SQUARE"},
      {TokenType::MINUS_EQUAL,         "MINUS_EQUAL"},
      {TokenType::MINUS_MINUS,         "MINUS_MINUS"},
      {TokenType::MINUS,               "MINUS"},
      {TokenType::MUTABLE,             "MUTABLE"},
      {TokenType::NEG_INF,             "NEG_INF"},
      {TokenType::NOT,                 "NOT"},
      {TokenType::NUMERIC_CONST,       "NUMERIC_CONST"},
      {TokenType::OR,                  "OR"},
      {TokenType::PERCENT_EQUAL,       "PERCENT_EQUAL"},
      {TokenType::PERCENT,             "PERCENT"},
      {TokenType::PIPE_EQUAL,          "PIPE_EQUAL"},
      {TokenType::PIPE,                "PIPE"},
      {TokenType::PLUS_EQUAL,          "PLUS_EQUAL"},
      {TokenType::PLUS_PLUS,           "PLUS_PLUS"},
      {TokenType::PLUS,                "PLUS"},
      {TokenType::POS_INF,             "POS_INF"},
      {TokenType::PRIVATE,             "PRIVATE"},
      {TokenType::PUBLIC,              "PUBLIC"},
      {TokenType::QUESTION,            "QUESTION"},
      {TokenType::RANGLE_EQUAL,        "RANGLE_EQUAL"},
      {TokenType::RANGLE_RANGLE,       "RANGLE_RANGLE"},
      {TokenType::RANGLE,              "RANGLE"},
      {TokenType::RETURN,              "RETURN"},
      {TokenType::RIGHT_CURLY,         "RIGHT_CURLY"},
      {TokenType::RIGHT_PAREN,         "RIGHT_PAREN"},
      {TokenType::RIGHT_SQUARE,        "RIGHT_SQUARE"},
      {TokenType::SEMI,                "SEMI"},
      {TokenType::STAR_EQUAL,          "STAR_EQUAL"},
      {TokenType::STAR,                "STAR"},
      {TokenType::STRING_LITERAL,      "STRING_LITERAL"},
      {TokenType::STRING,              "STRING"},
      {TokenType::TILDE_EQUAL,         "TILDE_EQUAL"},
      {TokenType::TILDE,               "TILDE"},
      {TokenType::TRUE,                "TRUE"},
      {TokenType::TYPE_IDENT,          "TYPE_IDENT"},
      {TokenType::UINT16_T_MAX,        "UINT16_T_MAX"},
      {TokenType::UINT16_T_MIN,        "UINT16_T_MIN"},
      {TokenType::UINT16_T,            "UINT16_T"},
      {TokenType::UINT32_T_MAX,        "UINT32_T_MAX"},
      {TokenType::UINT32_T_MIN,        "UINT32_T_MIN"},
      {TokenType::UINT32_T,            "UINT32_T"},
      {TokenType::UINT64_T_MAX,        "UINT64_T_MAX"},
      {TokenType::UINT64_T_MIN,        "UINT64_T_MIN"},
      {TokenType::UINT64_T,            "UINT64_T"},
      {TokenType::UINT8_T_MAX,         "UINT8_T_MAX"},
      {TokenType::UINT8_T_MIN,         "UINT8_T_MIN"},
      {TokenType::UINT8_T,             "UINT8_T"},
      {TokenType::WHILE,               "WHILE"}
    };
        // clang-format on

        return token_str;
    }
};

//! @brief Parsed token.
struct Token
{
    //! @brief The type of this token.
    TokenType token_type;
    //! @brief The literal string parsed to get the token.
    std::string lexeme;
    //! @brief The offset and span of the token.
    std::pair<uint64_t, uint64_t> source_span;
    //! @brief The line and column number of the token.
    std::pair<uint64_t, uint64_t> location;

    friend inline std::ostream& operator<<(std::ostream& out, Token const& data)
    {
        out << "Token:";
        out << "\n\tTokenType: " << TokenTypeUtil::get_type_as_str(data.token_type);
        out << "\n\tLexme: \"" << data.lexeme << "\"";
        out << "\n\tOffset: " << data.source_span.first;
        out << "\n\tSpan: " << data.source_span.second;
        out << "\n\tLine: " << data.location.first;
        out << "\n\tColumn: " << data.location.second;
        return out;
    }
};

//! @brief Token for representing numeric constants.
struct NumConstToken : public Token
{
    //! @brief The value of the floating point number.
    NumericConst value;
};

//! @brief Token for representing string values.
//! This could be string literals, identifiers, or type identifiers.
struct StringToken : public Token
{
    //! @brief The string value stored, either string literal or identifier.
    std::string value;
};

}  // namespace pimento::tokenization

#if defined(__clang__)
#pragma clang diagnostic pop
#endif