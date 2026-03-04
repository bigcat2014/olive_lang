/// @file lexer.hpp
/// @brief Pimento lexer.
/// @author Logan Thomas

#pragma once

#include <istream>
#include <sstream>
#include <unordered_set>
#include <vector>

#include <pimento/input_buffer.hpp>
#include <pimento/tokens.hpp>

namespace pimento::tokenization {

/// @brief Class responsible for lexing input from a stream and producing a token vector for parsing.
class Lexer
{
public:
    /// @brief The starting buffer size for the tokens.
    static constexpr size_t BUFFER_SIZE = 4096;

    /// @brief Constructor for the Lexer
    /// @param istream The stream of characters to tokenize.
    explicit Lexer(std::istream* istream);

    /// @brief Getter for the vector of tokens lexed.
    /// @return The vector of lexed tokens.
    [[nodiscard]] const std::vector<Token>& tokens() const noexcept;

    /// @brief Tokenize the input stream.
    void tokenize();

private:
    /// @brief Holds the results of parsing the mantissa portion of a numeric literal.
    /// @details Returned by parseMantissa to describe which components were found during parsing, allowing the caller
    /// to determine the appropriate token type and validate that at least one digit was present.
    struct MantissaResult
    {
        /// @brief True if one or more digits were found before the decimal point.
        bool hasLeadingDigits;
        /// @brief True if a decimal point was found, promoting the literal to float territory.
        bool hasDot;
        /// @brief True if one or more digits were found after the decimal point. Only meaningful when hasDot is true.
        bool hasTrailingDigits;
    };

private:
    /// @brief Finalize a token and add it to the completed list of tokens.
    /// @details Finalize a token by setting the token type, lexeme, and span.
    /// @param token The token to finalize.
    /// @param type The type of the token.
    void addToken(Token& token, TokenType type) noexcept;

    /// @brief Parse an alphanumeric token from the input buffer.
    /// @details This token could be a keyword, an identifier, or a type identifier.
    /// @param token The token to build.
    void parseAlnumToken(Token& token) noexcept;

    /// @brief Parses the mantissa portion of a numeric literal from the input buffer.
    /// @details Consumes leading digits, an optional decimal point, and trailing digits from the input buffer, writing
    /// each consumed character into mTokenBuffer. The first character is assumed to have already been consumed by the
    /// caller and written into mTokenBuffer prior to this call.
    /// @return A MantissaResult containing flags indicating which components were found.
    MantissaResult parseMantissa() noexcept;

    /// @brief Parses the exponent portion of a numeric literal from the input buffer.
    /// @details If the next character in the input buffer is 'e' or 'E', consumes the exponent including its optional
    /// sign and required digit sequence, writing each consumed character into mTokenBuffer. If an exponent marker is
    /// found but no digits follow, raises a pimento INVALID_TOKEN_ERROR (no throw).
    /// @param token The token being constructed, used for error reporting.
    /// @return True if a valid exponent was found and consumed, false if no 'e'/'E' was present.
    bool parseExponent(const Token& token) noexcept;

    /// @brief Parse a numeric literal token from the input buffer.
    /// @param token The token to build.
    void parseNumericLiteral(Token& token) noexcept;

    /// @brief Parse a hexidecimal literal from the input buffer.
    /// @param token The token to build.
    void parseHex(Token& token) noexcept;

    /// @brief Parse an octal literal from the input buffer.
    /// @param token The token to build.
    void parseOctal(Token& token) noexcept;

    /// @brief Parse a binary literal from the input buffer.
    /// @param token The token to build.
    void parseBinary(Token& token) noexcept;

    /// @brief Finalizes a scientific or floating-point numeric literal token.
    /// @details Converts the contents of mTokenBuffer to a double using std::stod and stores the result in the token.
    /// If the conversion does not consume the full contents of mTokenBuffer, raises a pimento INVALID_TOKEN_ERROR
    /// indicating a malformed literal (no throw).
    /// @param token The token being constructed, used for error reporting and the final value.
    void finalizeFloat(Token& token) noexcept;

    /// @brief Parses a string token handling escape sequences.
    /// @param token The token being constructed, used for error reporting and the final value.
    void parseStringToken(Token& token) noexcept;

    /// @brief Parses a raw string token, ignoring escape sequences and parsing until the next '"'.
    /// @param token The token being constructed, used for error reporting and the final value.
    void parseRawStringToken(Token& token) noexcept;

    /// @brief Check if the specified character is valid for an identifier.
    /// @param value The character to check.
    /// @return True if the character is valid for an identifier, false otherwise.
    [[nodiscard]] static bool isTokenChar(char value) noexcept
    {
        return std::isalnum(static_cast<unsigned char>(value)) != 0 || value == '_';
    }

    /// @brief Check if the specified character is valid for a type identifier.
    /// @param value The character to check.
    /// @return True if the character is valid for a type identifier, false
    /// otherwise.
    [[nodiscard]] static bool isTypeChar(char value) noexcept
    {
        return std::isalnum(static_cast<unsigned char>(value)) != 0;
    }

    /// @brief Check if the specified character is the hex specifier.
    /// @param value The character to check.
    /// @return True if the character is the hex specifier, false otherwise.
    [[nodiscard]] static bool isHexSpecifier(char value) noexcept { return value == 'x' || value == 'X'; }

    /// @brief Check if the specified character is the octal specifier.
    /// @param value The character to check.
    /// @return True if the character is the octal specifier, false otherwise.
    [[nodiscard]] static bool isOctalSpecifier(char value) noexcept { return value == 'o' || value == 'O'; }

    /// @brief Check if the specified character is the binary specifier.
    /// @param value The character to check.
    /// @return True if the character is the binary specifier, false otherwise.
    [[nodiscard]] static bool isBinarySpecifier(char value) noexcept { return value == 'b' || value == 'B'; }

    /// @brief Check if the specified character is a valid octal digit.
    /// @param value The character to check.
    /// @return True if the character is a valid octal digit, false otherwise.
    [[nodiscard]] static bool isOctalDigit(char value) noexcept
    {
        static const std::unordered_set<char> ValidChars{'0', '1', '2', '3', '4', '5', '6', '7'};

        return ValidChars.contains(value);
    }

    /// @brief Check if the specified character is a valid binary digit.
    /// @param value The character to check.
    /// @return True if the character is a valid binary digit, false otherwise.
    [[nodiscard]] static bool isBinaryDigit(char value) noexcept { return value == '0' || value == '1'; }

    /// @brief Check if the specified character is the scientific number delimiter.
    /// @param value The character to check.
    /// @return True if the character is the scientific number delimiter, false otherwise.
    [[nodiscard]] static bool isScientificDelimiter(char value) noexcept { return value == 'e' || value == 'E'; }

    /// @brief Checks whether or not a string is a valid identifier.
    /// @details A valid identifier matches the regex _{0,2}[a-z][a-zA-Z0-9_]*
    /// @param lexeme The string to check.
    /// @return True if it is a valid identifier, false otherwise.
    [[nodiscard]] static bool isValidIdentifier(const std::string& lexeme) noexcept;

    /// @brief Checks whether or not a string is a valid type identifier.
    /// @details A valid type identifier matches the regex [A-Z][a-zA-Z0-9]*
    /// @param lexeme The string to check.
    /// @return True if it is a valid type identifier, false otherwise.
    [[nodiscard]] static bool isValidType(const std::string& lexeme) noexcept;

    /// @brief Convert Scientific Notation to double precision float literal.
    /// @param mantissaStr The mantissa of the scientific number.
    /// @param exponentStr The exponent of the scientific number.
    /// @return The double precision floating point number represented by the scientific notation.
    [[nodiscard]] static FloatLiteral doubleFromScientific(std::string& mantissaStr, const std::string& exponentStr);

private:
    /// @brief The base of hexidecimal numbers.
    static constexpr int HEX_BASE = 16;
    /// @brief Maximum number of hexidecimal digits that can appear in a single hex literal.
    static constexpr size_t MAX_HEX_DIGITS = 16;  // 64-bit numbers

    /// @brief The base of octal numbers.
    static constexpr int OCTAL_BASE = 8;
    /// @brief Maximum number of octal digits that can appear in a single octal literal.
    static constexpr size_t MAX_OCTAL_DIGITS = 22;  // 64-bit numbers

    /// @brief The base of binary numbers.
    static constexpr int BINARY_BASE = 2;
    /// @brief Maximum number of binary digits that can appear in a single binary literal.
    static constexpr size_t MAX_BINARY_DIGITS = 64;  // 64-bit numbers

    /// @brief The size in bytes of the maximum token length.
    static constexpr size_t MAX_TOKEN_LEN = 64;

    /// @brief The buffer of the input to tokenize.
    InputBuffer mInputBuffer;
    /// @brief The current characters representing the token.
    std::stringstream mTokenBuffer;
    /// @brief The tokens parsed from the input stream.
    std::vector<Token> mTokens;
};

}  // namespace pimento::tokenization