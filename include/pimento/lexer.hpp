//! @file lexer.hpp
//! @brief Pimento lexer.
//! @author Logan Thomas

#pragma once

#include <array>
#include <istream>
#include <sstream>

#include <pimento/tokens.hpp>

namespace pimento::tokenization {

class Lexer {
public:
  //! @brief Constructor for the Lexer
  //! @param istream std::shared_ptr<std::istream> The stream of characters to
  //! tokenize.
  explicit Lexer(std::shared_ptr<std::istream> istream);

  //! @brief Getter for the vector of tokens lexed.
  //! @return const std::vector<Token>& The vector of tokens lexed.
  [[nodiscard]] const std::vector<Token> &tokens() const noexcept;

private:
  //! @brief Tokenize the input stream.
  void tokenize();

  //! @brief Peek at a character at an offset from the current character in the
  //! buffer.
  //!
  //! Peek at a character at an offset from the current character in the buffer.
  //! If attempting to peek out of bounds, return whitespace.
  //! @param current_index size_t The index of the current character in the
  //! buffer.
  //! @param buffer const char* const The buffer from which to get the
  //! character.
  //! @param size size_t The length of the buffer.
  //! @param lookahead size_t Optional lookahead distance to peek.
  //! @return char The character at `lookahead` offset from the current index or
  //! 0 if attempting to peek out of bounds.
  [[nodiscard]] inline char peek(size_t current_index, const char *const buffer,
                                 size_t size,
                                 size_t lookahead = 0) const noexcept;

  //! @brief Attmpt to parse a token from the buffer.
  //! @param token_buffer std::string The buffer from which to attempt to parse
  //! a token.
  //! @param next const char The next character in the buffer.
  //! @return bool Whether or not we successfully parsed a full token.
  bool try_parse_token(std::string &token_buffer, const char next) noexcept;

  //! @brief Token visitor for logging trace output.
  struct TraceTokenVisitor {
    //! @brief Output string stream for building a trace string based on the
    //! token properties.
    std::ostringstream &output;

    //! @brief Handle BinOpProperties variant.
    //! @param properties BinOpProperties The properties of the binary operator.
    void operator()(const BinOpProperties &properties) const noexcept;

    //! @brief Handle IntLitProperties variant.
    //! @param properties IntLitProperties The properties of the int literal.
    void operator()(const IntLitProperties &properties) const noexcept;

    //! @brief Handle IdentProperties variant.
    //! @param properties IdentProperties The properties of the identifier.
    void operator()(const IdentProperties &properties) const noexcept;

    //! @brief Handle std::monostate variant.
    //! @param Unused std::monostate
    void operator()(const std::monostate &) const noexcept;
  };

private:
  //! @brief The size in bytes of the chunks to read from the input stream.
  constexpr static size_t BUFFER_SIZE = 4096;
  //! @brief The size in bytes of the maximum token length.
  constexpr static size_t MAX_TOKEN_LEN = 64;
  //! @brief The input stream to tokenize.
  std::shared_ptr<std::istream> p_stream;
  //! @brief The tokens parsed from the input stream.
  std::vector<Token> m_tokens;
};

} // namespace pimento::tokenization