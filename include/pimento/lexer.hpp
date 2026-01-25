//! @file lexer.hpp
//! @brief Pimento lexer.
//! @author Logan Thomas

#pragma once

#include <array>
#include <istream>
#include <optional>
#include <sstream>
#include <unordered_set>
#include <vector>

#include <pimento/input_buffer.hpp>
#include <pimento/tokens.hpp>

namespace pimento::tokenization {

class Lexer
{
   public:
    //! @brief Constructor for the Lexer
    //! @param istream The stream of characters to tokenize.
    explicit Lexer(std::istream& istream);

    //! @brief Getter for the vector of tokens lexed.
    //! @return The vector of lexed tokens.
    [[nodiscard]] const std::vector<Token>& tokens() const noexcept;

    //! @brief Tokenize the input stream.
    void tokenize();

   private:
    //! @brief Create a token of the specified type.
    //! @param type The TokenType to create.
    //! @param offset The offset of the token in the input.
    //! @param line The line number of the start of the token.
    //! @param column The column number of the start of the token.
    void create_token(TokenType type, size_t offset, size_t line, size_t column) noexcept;

    //! @brief Create an Identifier token.
    //! @param offset The offset of the token in the input.
    //! @param line The line number of the start of the token.
    //! @param column The column number of the start of the token.
    void create_ident_token(const std::string& value, size_t offset, size_t line, size_t column) noexcept;

    //! @brief Create a Type Identifier token.
    //! @param offset The offset of the token in the input.
    //! @param line The line number of the start of the token.
    //! @param column The column number of the start of the token.
    void create_type_token(const std::string& value, size_t offset, size_t line, size_t column) noexcept;

    //! @brief Parse an identifier token from the input buffer.
    //! @param offset The offset of the start of the token for token creation.
    //! @param line The line number of the start of the token for token creation.
    //! @param column The column number of the start of the token for token
    //! creation.
    void parse_ident(size_t offset, size_t line, size_t column);

    //! @brief Parse a type identifier token from the input buffer.
    //! @param offset The offset of the start of the token for token creation.
    //! @param line The line number of the start of the token for token creation.
    //! @param column The column number of the start of the token for token
    //! creation.
    void parse_type(size_t offset, size_t line, size_t column);

    //! @brief Parse a numeric constant token from the input buffer.
    //! @param offset The offset of the start of the token for token creation.
    //! @param line The line number of the start of the token for token creation.
    //! @param column The column number of the start of the token for token
    //! creation.
    void parse_numeric_const(size_t offset, size_t line, size_t column);

    //! @brief Check if the specified character is valid for an identifier.
    //! @param value The character to check.
    //! @return True if the character is valid for an identifier, false otherwise.
    [[nodiscard]] inline bool is_ident_char(char value) noexcept
    {
        return std::islower(value) || std::isupper(value) || std::isdigit(value) || value == '_';
    }

    //! @brief Check if the specified character is valid for a type identifier.
    //! @param value The character to check.
    //! @return True if the character is valid for a type identifier, false
    //! otherwise.
    [[nodiscard]] inline bool is_type_char(char value) noexcept
    {
        return std::islower(value) || std::isupper(value) || std::isdigit(value);
    }

    //! @brief Check if the specified character is the hex specifier.
    //! @param value The character to check.
    //! @return True if the character is the hex specifier, false otherwise.
    [[nodiscard]] inline bool is_hex_specifier(char value) noexcept { return value == 'x' || value == 'X'; }

    //! @brief Check if the specified character is the octal specifier.
    //! @param value The character to check.
    //! @return True if the character is the octal specifier, false otherwise.
    [[nodiscard]] inline bool is_octal_specifier(char value) noexcept { return value == 'o' || value == 'O'; }

    //! @brief Check if the specified character is the binary specifier.
    //! @param value The character to check.
    //! @return True if the character is the binary specifier, false otherwise.
    [[nodiscard]] inline bool is_binary_specifier(char value) noexcept { return value == 'b' || value == 'B'; }

    //! @brief Check if the specified character is a valid octal digit.
    //! @param value The character to check.
    //! @return True if the character is a valid octal digit, false otherwise.
    [[nodiscard]] inline bool is_octal_digit(char value) noexcept
    {
        static const std::unordered_set<char> chars{'0', '1', '2', '3', '4', '5', '6', '7'};

        return chars.contains(value);
    }

    //! @brief Check if the specified character is a valid binary digit.
    //! @param value The character to check.
    //! @return True if the character is a valid binary digit, false otherwise.
    [[nodiscard]] inline bool is_binary_digit(char value) noexcept
    {
        static const std::unordered_set<char> chars{'0', '1'};

        return chars.contains(value);
    }

    //! @brief Convert Scientific Notation to double precision float constant.
    //! @param mantissa_str The mantissa of the scientific number.
    //! @param exponent_str The exponent of the scientific number.
    //! @return The double precision floating point number represented by the
    //! scientific notation.
    [[nodiscard]] FloatConst double_from_scientific(std::string& mantissa_str, const std::string& exponent_str);

   private:
    //! @brief The size in bytes of the maximum token length.
    constexpr static size_t MAX_TOKEN_LEN = 64;

    //! @brief The buffer of the input to tokenize.
    InputBuffer m_file_buffer;
    //! @brief The current characters representing the token.
    std::stringstream m_token_buffer;
    //! @brief The tokens parsed from the input stream.
    std::vector<Token> m_tokens;
};

}  // namespace pimento::tokenization