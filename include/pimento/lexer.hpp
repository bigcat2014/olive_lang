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
    /// @brief Add a token to the completed list of tokens.
    /// @param token The token to add.
    void addToken(const Token& token);

    /// @brief Create a token of the specified type.
    /// @param token The token to build.
    /// @param type The type of the token.
    void createToken(Token& token, TokenType type) noexcept;

    /// @brief Create an Identifier token.
    /// @param token The token to build.
    /// @param value The identifier string.
    void createIdentToken(Token& token, const std::string& value) noexcept;

    /// @brief Create a Type Identifier token.
    /// @param token The token to build.
    /// @param value The type string.
    void createTypeToken(Token& token, const std::string& value) noexcept;

    /// @brief Parse an identifier token from the input buffer.
    /// @param token The token to build.
    void parseIdent(Token& token) noexcept;

    /// @brief Parse a type identifier token from the input buffer.
    /// @param token The token to build.
    void parseType(Token& token) noexcept;

    /// @brief Parse a numeric constant token from the input buffer.
    /// @param token The token to build.
    void parseNumericConst(Token& token) noexcept;

    /// @brief Check if the specified character is valid for an identifier.
    /// @param value The character to check.
    /// @return True if the character is valid for an identifier, false otherwise.
    [[nodiscard]] static bool isIdentChar(char value) noexcept
    {
        return (std::islower(value) != 0) || (std::isupper(value) != 0) || (std::isdigit(value) != 0) || value == '_';
    }

    /// @brief Check if the specified character is valid for a type identifier.
    /// @param value The character to check.
    /// @return True if the character is valid for a type identifier, false
    /// otherwise.
    [[nodiscard]] static bool isTypeChar(char value) noexcept
    {
        return (std::islower(value) != 0) || (std::isupper(value) != 0) || (std::isdigit(value) != 0);
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
        static const std::unordered_set<char> Chars{'0', '1', '2', '3', '4', '5', '6', '7'};

        return Chars.contains(value);
    }

    /// @brief Check if the specified character is a valid binary digit.
    /// @param value The character to check.
    /// @return True if the character is a valid binary digit, false otherwise.
    [[nodiscard]] static bool isBinaryDigit(char value) noexcept
    {
        static const std::unordered_set<char> Chars{'0', '1'};

        return Chars.contains(value);
    }

    /// @brief Convert Scientific Notation to double precision float constant.
    /// @param mantissa_str The mantissa of the scientific number.
    /// @param exponent_str The exponent of the scientific number.
    /// @return The double precision floating point number represented by the scientific notation.
    [[nodiscard]] static FloatConst doubleFromScientific(std::string& mantissaStr, const std::string& exponentStr);

private:
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