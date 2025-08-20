//! @file lexer.hpp
//! @brief Pimento lexer.
//! @author Logan Thomas

#pragma once

#include <array>
#include <istream>
#include <optional>
#include <sstream>
#include <vector>

#include <pimento/input_buffer.hpp>
#include <pimento/tokens.hpp>

namespace pimento::tokenization {

class Lexer {
public:
  //! @brief Constructor for the Lexer
  //! @param istream The stream of characters to tokenize.
  explicit Lexer(std::istream &istream);

  //! @brief Getter for the vector of tokens lexed.
  //! @return The vector of lexed tokens.
  [[nodiscard]] const std::vector<Token> &tokens() const noexcept;

  //! @brief Tokenize the input stream.
  void tokenize();

private:
  //! @brief Create a token of the specified type.
  //! @param type The TokenType to create.
  //! @param offset The offset of the token in the input.
  //! @param line The line number of the start of the token.
  //! @param column The column number of the start of the token.
  void create_token(TokenType type, size_t offset, size_t line,
                    size_t column) noexcept;

  //! @brief Convert Scientific Notation to double precision float constant.
  //! @param mantissa_str The mantissa of the scientific number.
  //! @param exponent_str The exponent of the scientific number.
  //! @return The double precision floating point number represented by the
  //! scientific notation.
  [[nodiscard]] FloatConst
  double_from_scientific(std::string &mantissa_str,
                         const std::string &exponent_str);

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

} // namespace pimento::tokenization