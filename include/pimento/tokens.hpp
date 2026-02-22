/// @file tokens.hpp
/// @brief Pimento Tokens.
/// @author Logan Thomas

#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

#include <bit>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <string>
#include <variant>

#include <magic_enum/magic_enum.hpp>

namespace pimento::tokenization {

/// @brief Supported token types.
enum class TokenType : uint8_t
{
    BEGIN = 0,
    TT_AMP_EQUAL,
    TT_AMP,
    TT_AND,
    TT_BOOL,
    TT_BREAK,
    TT_CARET_CARET_EQUAL,
    TT_CARET_CARET,
    TT_CARET_EQUAL,
    TT_CARET,
    TT_CLASS,
    TT_COLON,
    TT_COMMA,
    TT_COMMENT,
    TT_DOT,
    TT_ELIF,
    TT_ELSE,
    TT_ENUM,
    TT_EQUAL_EQUAL,
    TT_EQUAL,
    TT_EXCLAIM_EQUAL,
    TT_EXIT,
    TT_FALSE,
    TT_FLOAT_CONST,
    TT_FLOAT_NAN,
    TT_FLOAT32_T_MAX,
    TT_FLOAT32_T_MIN,
    TT_FLOAT32_T,
    TT_FLOAT64_T_MAX,
    TT_FLOAT64_T_MIN,
    TT_FLOAT64_T,
    TT_FOR,
    TT_FSLASH_EQUAL,
    TT_FSLASH_FSLASH_EQUAL,
    TT_FSLASH_FSLASH,
    TT_FSLASH,
    TT_FUNCTION,
    TT_IDENT,
    TT_IF,
    TT_IN,
    TT_INT16_T_MAX,
    TT_INT16_T_MIN,
    TT_INT16_T,
    TT_INT32_T_MAX,
    TT_INT32_T_MIN,
    TT_INT32_T,
    TT_INT64_T_MAX,
    TT_INT64_T_MIN,
    TT_INT64_T,
    TT_INT8_T_MAX,
    TT_INT8_T_MIN,
    TT_INT8_T,
    TT_INTEGER_CONST,
    TT_INTERFACE,
    TT_LANGLE_EQUAL,
    TT_LANGLE_LANGLE,
    TT_LANGLE,
    TT_LEFT_CURLY,
    TT_LEFT_PAREN,
    TT_LEFT_SQUARE,
    TT_MINUS_EQUAL,
    TT_MINUS_MINUS,
    TT_MINUS,
    TT_MUTABLE,
    TT_NEG_INF,
    TT_NOT,
    TT_NUMERIC_CONST,
    TT_OR,
    TT_PERCENT_EQUAL,
    TT_PERCENT,
    TT_PIPE_EQUAL,
    TT_PIPE,
    TT_PLUS_EQUAL,
    TT_PLUS_PLUS,
    TT_PLUS,
    TT_POS_INF,
    TT_PRIVATE,
    TT_PUBLIC,
    TT_QUESTION,
    TT_RANGLE_EQUAL,
    TT_RANGLE_RANGLE,
    TT_RANGLE,
    TT_RETURN,
    TT_RIGHT_CURLY,
    TT_RIGHT_PAREN,
    TT_RIGHT_SQUARE,
    TT_SEMI,
    TT_STAR_EQUAL,
    TT_STAR,
    TT_STRING_LITERAL,
    TT_STRING,
    TT_TILDE_EQUAL,
    TT_TILDE,
    TT_TRUE,
    TT_TYPE_IDENT,
    TT_UINT16_T_MAX,
    TT_UINT16_T_MIN,
    TT_UINT16_T,
    TT_UINT32_T_MAX,
    TT_UINT32_T_MIN,
    TT_UINT32_T,
    TT_UINT64_T_MAX,
    TT_UINT64_T_MIN,
    TT_UINT64_T,
    TT_UINT8_T_MAX,
    TT_UINT8_T_MIN,
    TT_UINT8_T,
    TT_WHILE,
    NUM_TOKENS
};

// TODO(lthomas): Not yet IEEE-754 compliant.
/// @brief Float Constant Value.
/// @details Can be a double, float, or scientific notation.
class FloatConst
{
public:
    /// @brief Enum representing the precision of the floating point value.
    enum class Precision : uint8_t
    {
        FLOAT32,
        FLOAT64
    };

    /// @brief Constructor for value represented in scientific notation.
    /// @param mantissa The mantissa of the value.
    /// @param exponent The exponent of the value.
    /// @param negative Whether or not the value is negative.
    /// @param precision The precision of the floating point value.
    FloatConst(uint64_t mantissa, int32_t exponent, bool negative, Precision precision)
        : mMantissa(mantissa)
        , mExponent(exponent)
        , mNegative(negative)
        , mPrecision(precision)
    {}

    /// @brief Constructor for double-precision floating point values.
    /// @param value The value of the floating point number.
    explicit FloatConst(double value)
        : mNegative(std::signbit(value))
        , mPrecision(Precision::FLOAT64)
    {
        const double absVal = std::fabs(value);
        int exp;
        const double frac = std::frexp(absVal, &exp);
        mMantissa         = static_cast<uint64_t>(frac * (1ULL << FLOAT64_MANTISSA_BITS));
        mExponent         = exp - FLOAT64_MANTISSA_BITS;
    }

    /// @brief Constructor for single-precision floating point values.
    /// @param value The value of the floating point number.
    explicit FloatConst(float value)
        : mNegative(std::signbit(value))
        , mPrecision(Precision::FLOAT32)
    {
        const float absVal = std::fabs(value);
        int exp;
        const float frac = std::frexp(absVal, &exp);
        mMantissa        = static_cast<uint64_t>(frac * (1ULL << FLOAT32_MANTISSA_BITS));
        mExponent        = exp - FLOAT32_MANTISSA_BITS;
    }

public:
    /// @brief Get the value as a double precision float.
    /// @return The value as a double precision float.
    [[nodiscard]] double asFloat64() const noexcept
    {
        double result;
        switch (mPrecision) {
            case Precision::FLOAT64:
                result = std::ldexp(static_cast<double>(mMantissa), mExponent);
                result = mNegative ? -result : result;
                break;
            case Precision::FLOAT32:
                result = static_cast<double>(asFloat32());
                break;
        }
        return result;
    }

    /// @brief Get the value as a single precision float.
    /// @return The value as a single precision float.
    [[nodiscard]] float asFloat32() const noexcept
    {
        float result;
        switch (mPrecision) {
            case Precision::FLOAT64:
                result = static_cast<float>(asFloat64());
                break;
            case Precision::FLOAT32:
                result = static_cast<float>(std::ldexp(static_cast<double>(mMantissa), mExponent));
                result = mNegative ? -result : result;
                break;
        }
        return result;
    }

private:
    /// @brief The mantissa of the floating point number.
    uint64_t mMantissa;
    /// @brief The exponent of the floating point number.
    int mExponent;
    /// @brief Whether or not the value is negative.
    bool mNegative;
    /// @brief The precision the value was stored as.
    Precision mPrecision;

    /// @brief Number of bits in a 64-bit floating point value mantissa.
    static constexpr uint8_t FLOAT64_MANTISSA_BITS = 53;
    /// @brief Number of bits in a 32-bit floating point value mantissa.
    static constexpr uint8_t FLOAT32_MANTISSA_BITS = 24;
};

/// @brief Template wrapper for packed struct members.
/// @tparam T The type to wrap in a packed struct.
template <typename T>
#ifdef _MSC_VER
#pragma pack(push, 1)
struct PackedView
{
    T value;
};
#pragma pack(pop)
#else
struct [[gnu::packed]] PackedView
{
    /// @brief The value stored in the packed view.
    T value;
};
#endif

/// @brief Constant value as raw bits.
/// @details Generally to be used with values specified in hex, octal, or binary.
class RawBits
{
public:
    /// @brief Constructor.
    /// @tparam T Integral type.
    /// @param value The value to use as the raw bits.
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
    /// @brief Get the raw bits as unsigned 64-bit integer.
    /// @return The raw bits as unsigned 64-bit integer.
    [[nodiscard]] uint64_t asUint64() const noexcept { return uint64; }

    /// @brief Get the raw bits as signed 64-bit integer.
    /// @return The raw bits as signed 64-bit integer.
    [[nodiscard]] int64_t asInt64() const noexcept { return int64; }

    /// @brief Get the raw bits as unsigned 32-bit integer.
    /// @return The raw bits as unsigned 32-bit integer.
    [[nodiscard]] uint32_t asUint32() const noexcept { return uint32View.value; }

    /// @brief Get the raw bits as signed 32-bit integer.
    /// @return The raw bits as signed 32-bit integer.
    [[nodiscard]] int32_t asInt32() const noexcept { return int32View.value; }

    /// @brief Get the raw bits as unsigned 16-bit integer.
    /// @return The raw bits as unsigned 16-bit integer.
    [[nodiscard]] uint16_t asUint16() const noexcept { return uint16View.value; }

    /// @brief Get the raw bits as signed 16-bit integer.
    /// @return The raw bits as signed 16-bit integer.
    [[nodiscard]] int16_t asInt16() const noexcept { return int16View.value; }

    /// @brief Get the raw bits as unsigned 8-bit integer.
    /// @return The raw bits as unsigned 8-bit integer.
    [[nodiscard]] uint8_t asUint8() const noexcept { return uint8View.value; }

    /// @brief Get the raw bits as signed 8-bit integer.
    /// @return The raw bits as signed 8-bit integer.
    [[nodiscard]] int8_t asInt8() const noexcept { return int8View.value; }

private:
    /// @brief Union of signed and unsigned 64, 32, 16, and 8 bit values to be
    /// used for type punning.
    union
    {
        /// @brief 64-bit unsigned value.
        uint64_t uint64;

        /// @brief 64-bit signed value.
        int64_t int64;

        /// @brief View of lower 32-bits of unsigned value.
        PackedView<uint32_t> uint32View;

        /// @brief View of lower 32-bits of signed value.
        PackedView<int32_t> int32View;

        /// @brief View of lower 16-bits of unsigned value.
        PackedView<uint16_t> uint16View;

        /// @brief View of lower 16-bits of signed value.
        PackedView<int16_t> int16View;

        /// @brief View of lower 8-bits of unsigned value.
        PackedView<uint8_t> uint8View;

        /// @brief View of lower 8-bits of signed value.
        PackedView<int8_t> int8View;
    };
};

/// @brief Integer Constant value.
/// @details Can be signed or unsigned.
class IntConst
{
public:
    /// @brief Constructor.
    /// @tparam T Integral type.
    /// @param value The value of the integer.
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
    /// @brief Get the value as unsigned 64-bit integer.
    /// @return The value as unsigned 64-bit integer.
    [[nodiscard]] uint64_t asUint64() const noexcept { return uint64; }

    /// @brief Get the value as signed 64-bit integer.
    /// @return The value as signed 64-bit integer.
    [[nodiscard]] int64_t asInt64() const noexcept { return int64; }

    /// @brief Get the value as unsigned 32-bit integer.
    /// @return The value as unsigned 32-bit integer.
    [[nodiscard]] uint32_t asUint32() const noexcept { return static_cast<uint32_t>(uint64); }

    /// @brief Get the value as signed 32-bit integer.
    /// @return The value as signed 32-bit integer.
    [[nodiscard]] int32_t asInt32() const noexcept { return static_cast<int32_t>(int64); }

    /// @brief Get the value as unsigned 16-bit integer.
    /// @return The value as unsigned 16-bit integer.
    [[nodiscard]] uint16_t asUint16() const noexcept { return static_cast<uint16_t>(uint64); }

    /// @brief Get the value as signed 16-bit integer.
    /// @return The value as signed 16-bit integer.
    [[nodiscard]] int16_t asInt16() const noexcept { return static_cast<int16_t>(int64); }

    /// @brief Get the value as unsigned 8-bit integer.
    /// @return The value as unsigned 8-bit integer.
    [[nodiscard]] uint8_t asUint8() const noexcept { return static_cast<uint8_t>(uint64); }

    /// @brief Get the value as signed 8-bit integer.
    /// @return The value as signed 8-bit integer.
    [[nodiscard]] int8_t asInt8() const noexcept { return static_cast<int8_t>(int64); }

private:
    /// @brief Union of signed and signed and unsigned 64-bit values to be used
    /// for type punning.
    union
    {
        /// @brief 64-bit unsigned value.
        uint64_t uint64;
        /// @brief 64-bit signed value.
        int64_t int64;
    };
};

/// @brief Numeric Constant value.
/// @details Can be floating point number, integer, or raw bits.
class NumericConst
{
public:
    /// @brief Alias for the variant of the numeric constant value.
    using Value = std::variant<IntConst, FloatConst, RawBits>;

    /// @brief Constructor for integer value.
    /// @tparam T Integral type.
    /// @param value The value of the integer.
    template <typename T>
        requires std::is_integral_v<T>
    explicit NumericConst(T value)
        : mValue(IntConst(value))
    {}

    // TODO(lthomas): Not sure if I should always be creating these as double-precision float, I think this has the
    // potential for a double round error.
    /// @brief Constructor for floating point value represented in scientific notation.
    /// @param mantissa The mantissa of the floating point value.
    /// @param exponent The exponent of the floating point value.
    /// @param negative Whether or not the value is negative.
    NumericConst(uint64_t mantissa, int exponent, bool negative = false)
        : mValue(FloatConst{mantissa, exponent, negative, FloatConst::Precision::FLOAT64})
    {}

    /// @brief Constructor for value specified in raw bits.
    /// @param raw The raw bits from which to construct the value.
    explicit NumericConst(const RawBits& raw)
        : mValue(raw)
    {}

    /// @brief Constructor for value specified as FloatConst.
    /// @param floatConst The floating point constant from which to construct the value.
    explicit NumericConst(const FloatConst& floatConst)
        : mValue(floatConst)
    {}

    /// @brief Constructor for double-precision floating point values.
    /// @param value The value of the floating point number.
    explicit NumericConst(double value)
        : mValue(FloatConst(value))
    {}

    /// @brief Constructor for single-precision floating point values.
    /// @param value The value of the floating point number.
    explicit NumericConst(float value)
        : mValue(FloatConst(value))
    {}

public:
    /// @brief Get the value as a double-precision (64-bit) floating point number.
    /// @return The value as a double-precision (64-bit) floating point number.
    [[nodiscard]] double asFloat64() const
    {
        return std::visit(
            [](auto&& element) -> double {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return std::bit_cast<double>(element.asUint64());
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return element.asFloat64();
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return static_cast<double>(element.asInt64());
                }
            },
            mValue);
    }

    /// @brief Get the value as a single-precision (32-bit) floating point number.
    /// @return The value as a single-precision (32-bit) floating point number.
    [[nodiscard]] float asFloat32() const
    {
        return std::visit(
            [](auto&& element) -> float {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, RawBits>) {
                    // std::cout << "RawBits" << std::endl;
                    return std::bit_cast<float>(element.asUint32());
                }
                else if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return element.asFloat32();
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return static_cast<float>(element.asInt64());
                }
            },
            mValue);
    }

    /// @brief Get the value as unsigned 64-bit integer.
    /// @return The value as unsigned 64-bit integer.
    [[nodiscard]] uint64_t asUint64() const
    {
        return std::visit(
            [](auto&& element) -> uint64_t {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<uint64_t>(element.asFloat64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return element.asUint64();
                }
            },
            mValue);
    }

    /// @brief Get the value as signed 64-bit integer.
    /// @return The value as signed 64-bit integer.
    [[nodiscard]] int64_t asInt64() const
    {
        return std::visit(
            [](auto&& element) -> int64_t {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<int64_t>(element.asFloat64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return element.asInt64();
                }
            },
            mValue);
    }

    /// @brief Get the value as unsigned 32-bit integer.
    /// @return The value as unsigned 32-bit integer.
    [[nodiscard]] uint32_t asUint32() const
    {
        return std::visit(
            [](auto&& element) -> uint32_t {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<uint32_t>(element.asFloat64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return element.asUint32();
                }
            },
            mValue);
    }

    /// @brief Get the value as signed 32-bit integer.
    /// @return The value as signed 32-bit integer.
    [[nodiscard]] int32_t asInt32() const
    {
        return std::visit(
            [](auto&& element) -> int32_t {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<int32_t>(element.asFloat64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return element.asInt32();
                }
            },
            mValue);
    }

    /// @brief Get the value as unsigned 16-bit integer.
    /// @return The value as unsigned 16-bit integer.
    [[nodiscard]] uint16_t asUint16() const
    {
        return std::visit(
            [](auto&& element) -> uint16_t {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<uint16_t>(element.asFloat64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return element.asUint16();
                }
            },
            mValue);
    }

    /// @brief Get the value as signed 16-bit integer.
    /// @return The value as signed 16-bit integer.
    [[nodiscard]] int16_t asInt16() const
    {
        return std::visit(
            [](auto&& element) -> int16_t {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<int16_t>(element.asFloat64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return element.asInt16();
                }
            },
            mValue);
    }

    /// @brief Get the value as unsigned 8-bit integer.
    /// @return The value as unsigned 8-bit integer.
    [[nodiscard]] uint8_t asUint8() const
    {
        return std::visit(
            [](auto&& element) -> uint8_t {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<uint8_t>(element.asFloat64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return element.asUint8();
                }
            },
            mValue);
    }

    /// @brief Get the value as signed 8-bit integer.
    /// @return The value as signed 8-bit integer.
    [[nodiscard]] int8_t asInt8() const
    {
        return std::visit(
            [](auto&& element) -> int8_t {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, FloatConst>) {
                    // std::cout << "FloatConst" << std::endl;
                    return static_cast<int8_t>(element.asFloat64());
                }
                else {
                    // std::cout << "IntConst" << std::endl;
                    return element.asInt8();
                }
            },
            mValue);
    }

private:
    /// @brief The numeric constant value.
    Value mValue;
};

/// @brief Parsed token.
struct Token
{
    /// @brief The type of this token.
    TokenType tokenType;
    /// @brief The literal string parsed to get the token.
    std::string lexeme;
    /// @brief The offset and span of the token.
    std::pair<uint64_t, uint64_t> sourceSpan;
    /// @brief The line and column number of the token.
    std::pair<uint64_t, uint64_t> location;

    /// @brief Output stream operator for Token.
    /// @param out The output stream to which to write the token.
    /// @param data The token to write to the output stream.
    /// @return The output stream to which the token was written.
    friend std::ostream& operator<<(std::ostream& out, const Token& data)
    {
        out << "Token:";
        out << "\n\tTokenType: " << magic_enum::enum_name(data.tokenType);
        out << "\n\tLexeme: \"" << data.lexeme << "\"";
        out << "\n\tOffset: " << data.sourceSpan.first;
        out << "\n\tSpan: " << data.sourceSpan.second;
        out << "\n\tLine: " << data.location.first;
        out << "\n\tColumn: " << data.location.second;
        return out;
    }
};

/// @brief Token for representing numeric constants.
struct NumConstToken : public Token
{
    /// @brief The value of the floating point number.
    NumericConst value;
};

/// @brief Token for representing string values.
/// @details This could be string literals, identifiers, or type identifiers.
struct StringToken : public Token
{
    /// @brief The string value stored, either string literal or identifier.
    std::string value;
};

}  // namespace pimento::tokenization

#ifdef __clang__
#pragma clang diagnostic pop
#endif