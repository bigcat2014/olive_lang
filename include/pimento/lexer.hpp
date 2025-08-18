//! @file lexer.hpp
//! @brief Pimento lexer.
//! @author Logan Thomas

#pragma once

#include <array>
#include <istream>
#include <optional>
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

  //! @brief Tokenize the input stream.
  void tokenize();

private:
  //! @brief The size in bytes of the chunks to read from the input stream.
  constexpr static size_t BUFFER_SIZE = 4096;
  //! @brief The size in bytes of the maximum token length.
  constexpr static size_t MAX_TOKEN_LEN = 64;

private:

  //! @brief Peek at the next character in the buffer without consuming it.
  //! @param buffer The buffer from which to get the character.
  //! @return The next character in the buffer.
  [[nodiscard]] inline std::optional<char> peek(const std::array<char, BUFFER_SIZE> &buffer) noexcept;

  //! @brief Advance the lexer.
  inline void advance(const std::array<char, BUFFER_SIZE> &buffer) noexcept;

  //! @brief Attempt to consume the next character.
  //! @param buffer The buffer to consume from.
  //! @return The character that was consumed.
  [[nodiscard]] inline std::optional<char> try_consume(const std::array<char, BUFFER_SIZE> &buffer) noexcept;

  //! @brief Attempt to consume the next character if it matches the provided character.
  //! @param buffer The buffer to consume from.
  //! @param character The character to attempt to consume.
  //! @return The consumed character.
  inline char try_consume(const std::array<char, BUFFER_SIZE> &buffer, const char &character) noexcept;

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
  //! @brief The input stream to tokenize.
  std::istream &m_stream;
  //! @brief Total bytes read from the input stream.
  size_t m_total_bytes{0};
  //! @brief Total chunks of size BUFFER_SIZE read from the input stream.
  size_t m_total_chunks{0};
  //! @brief Current index in the buffer.
  size_t m_buffer_index{0};
  //! @brief The current line of the input stream.
  size_t m_current_line{0};
  //! @brief The current column of the current line of the input stream.
  size_t m_current_column{0};
  //! @brief The tokens parsed from the input stream.
  std::vector<Token> m_tokens;
};

} // namespace pimento::tokenization