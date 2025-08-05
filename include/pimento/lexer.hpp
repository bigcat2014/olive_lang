//! @file lexer.hpp
//! @brief Pimento lexer.
//! @author Logan Thomas

#pragma once

#include <array>
#include <istream>
#include <sstream>
#include <vector>

#include <pimento/tokens.hpp>

namespace pimento::tokenization {

class Lexer {
public:
  //! @brief Constructor for the Lexer
  //! @param istream The stream of characters to tokenize.
  explicit Lexer(std::istream &istream);

  //! @brief Getter for the vector of tokens lexed.
  //! @return The vector of tokens lexed.
  [[nodiscard]] const std::vector<Token> &tokens() const noexcept;

private:
  //! @brief Tokenize the input stream.
  void tokenize();

  //! @brief Peek at a character at an offset from the current character in the
  //! buffer.
  //! @details Peek at a character at an offset from the current character in
  //! the buffer. If attempting to peek out of bounds, return whitespace.
  //! @param current_index The index of the current character in the buffer.
  //! @param buffer The buffer from which to get the character.
  //! @param size The length of the buffer.
  //! @param lookahead Optional lookahead distance to peek.
  //! @return The character at `lookahead` offset from the current index or 0 if
  //! attempting to peek out of bounds.
  [[nodiscard]] static inline char peek(size_t current_index,
                                        const char *const buffer, size_t size,
                                        size_t lookahead = 0) noexcept;

  //! @brief Attmpt to parse a token from the buffer.
  //! @param token_buffer The buffer from which to attempt to parse a token.
  //! @param next The next character in the buffer.
  //! @return Whether or not we successfully parsed a full token.
  bool try_parse_token(std::string &token_buffer, const char next) noexcept;

  //! @brief Convert Scientific Notation to double precision float constant.
  //! @param mantissa_str The mantissa of the scientific number.
  //! @param exponent_str The exponent of the scientific number.
  //! @return The double precision floating point number represented by the
  //! scientific notation.
  [[nodiscard]] FloatConst
  double_from_scientific(std::string &mantissa_str,
                         const std::string &exponent_str);

private:
  //! @brief The size in bytes of the chunks to read from the input stream.
  constexpr static size_t BUFFER_SIZE = 4096;
  //! @brief The size in bytes of the maximum token length.
  constexpr static size_t MAX_TOKEN_LEN = 64;
  //! @brief The input stream to tokenize.
  std::istream &m_stream;
  //! @brief The tokens parsed from the input stream.
  std::vector<Token> m_tokens;
};

} // namespace pimento::tokenization