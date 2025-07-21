//! @file lexer.hpp
//! @brief Pimento lexer.
//! @author Logan Thomas

#pragma once

#include <array>
#include <cctype>
#include <istream>
#include <sstream>
#include <variant>

#include <pimento/tokens.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

class Lexer {
public:
  //! @brief Constructor for the Lexer
  //! @param istream std::shared_ptr<std::istream> The stream of characters to
  //! tokenize.
  Lexer(std::shared_ptr<std::istream> istream) : p_stream(istream) {
    m_tokens.reserve(BUFFER_SIZE);
    tokenize();
  }

  //! @brief Getter for the vector of tokens lexed.
  //! @return const std::vector<Token>& The vector of tokens lexed.
  [[nodiscard]] const std::vector<Token> &tokens() const noexcept {
    return m_tokens;
  }

private:
  //! @brief Tokenize the input stream.
  void tokenize() {
    auto &logger = utils::get_logger();

    size_t total = 0;
    size_t total_chunks = 0;
    std::array<char, BUFFER_SIZE> file_buffer;
    std::string token_buffer;
    token_buffer.reserve(MAX_TOKEN_LEN);

    while (*p_stream) {
      p_stream->read(file_buffer.data(), file_buffer.size());
      size_t n = static_cast<size_t>(p_stream->gcount());
      if (n <= 0) {
        break;
      }

      total += n;
      for (size_t i = 0; i < n; i++) {
        if (token_buffer.size() + 1 > token_buffer.capacity()) {
          logger.error("Max token length of {} characters exceeded.",
                       MAX_TOKEN_LEN);
          exit(EXIT_FAILURE);
        }

        // Skip whitespace
        if (std::isspace(file_buffer[i])) {
          token_buffer.clear();
          continue;
        }

        // Add next character to token buffer
        token_buffer.push_back(file_buffer[i]);

        // Attempt to parse token from token buffer
        const char next = peek(i, file_buffer.data(), n, 1);
        bool token_added = try_parse_token(token_buffer, next);

        if (logger.level() == spdlog::level::trace) {
          if (!m_tokens.empty() && token_added) {
            Token token = m_tokens.back();
            try {
              std::ostringstream trace_output;
              trace_output << "Got token `"
                           << TokenTypeUtil::get_type_as_str(token.token_type)
                           << "`";
              std::visit(TraceTokenVisitor{.output = trace_output},
                         token.properties);
              logger.trace(trace_output.str());
            } catch (const std::out_of_range &) {
              logger.trace("Invalid token");
            }
          }
        }
      }

      logger.trace("Finished chunk {}", total_chunks++);
    }

    logger.debug("Total chunks read: {}", total_chunks);
    logger.debug("Total bytes read: {}", total);
  }

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
                                 size_t lookahead = 0) const noexcept {
    if (current_index + lookahead < size) {
      return buffer[current_index + lookahead];
    }

    // Return invalid character.
    return 0;
  }

  //! @brief Attmpt to parse a token from the buffer.
  //! @param token_buffer std::string The buffer from which to attempt to parse
  //! a token.
  //! @param next const char The next character in the buffer.
  //! @return bool Whether or not we successfully parsed a full token.
  bool try_parse_token(std::string &token_buffer, const char next) noexcept {
    if (token_buffer.empty()) {
      return false;
    }

    try {
      TokenType token_type = TokenTypeUtil::get_token_type(token_buffer);
      Token token = TokenFactory::create_token(token_type);
      m_tokens.push_back(token);
      token_buffer.clear();
      return true;
    } catch (const std::out_of_range &) {
      // Int literal special case
      if (std::isdigit(token_buffer.back())) {
        if (!std::isdigit(next)) {
          uint64_t value = std::stoull(token_buffer);
          m_tokens.push_back(
              TokenFactory::create_token(TokenType::TT_INT_LITERAL, value));
          token_buffer.clear();
          return true;
        }
        // Identifier special case
      } else if (std::isalpha(token_buffer.back())) {
        if (!std::isalpha(next)) {
          m_tokens.push_back(TokenFactory::create_token(
              TokenType::TT_IDENTIFIER, token_buffer));
          token_buffer.clear();
          return true;
        }
      }
    }

    return false;
  }

  //! @brief Token visitor for logging trace output.
  struct TraceTokenVisitor {
    //! @brief Output string stream for building a trace string based on the
    //! token properties.
    std::ostringstream &output;

    void operator()(const BinOpProperties &properties) const noexcept {
      output << "; Binary operator with prec: "
             << std::to_string(properties.precedence) << " and associativity: "
             << TokenTypeUtil::get_associativity_str(properties.associativity);
    }

    void operator()(const IntLitProperties &properties) const noexcept {
      output << "; Int literal with value: " << properties.value;
    }

    void operator()(const IdentProperties &properties) const noexcept {
      output << "; Identifier: " << properties.identifier;
    }

    void operator()(const std::monostate) const noexcept {}
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