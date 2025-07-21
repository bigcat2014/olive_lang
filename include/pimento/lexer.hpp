//! @file lexer.hpp
//! @brief Pimento lexer
//! @author Logan Thomas

#pragma once

#include <array>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <pimento/tokens.hpp>
#include <pimento/utils.hpp>
#include <sstream>
#include <stdexcept>

namespace pimento::tokenization {

class Lexer {
 public:
  //! @brief Constructor for the Lexer
  //! @param path const std::filesystem::path& The path to the file to tokenize.
  Lexer(const std::filesystem::path& path) : m_path(path) {
    m_tokens.reserve(BUFFER_SIZE);
  }

  //! @brief Tokenize the input file.
  void tokenize() {
    auto& logger = pimento::utils::get_logger();
    std::fstream file(m_path, std::ios::in);
    if (!file) {
      throw std::runtime_error("cannot open " + m_path.string());
    }

    std::array<char, BUFFER_SIZE> file_buffer;

    size_t total = 0;
    size_t total_chunks = 0;
    std::string token_buffer;
    token_buffer.reserve(MAX_TOKEN_LEN);

    while (file) {
      file.read(file_buffer.data(), file_buffer.size());
      std::streamsize n = file.gcount();
      if (n <= 0) {
        break;
      }

      total += n;
      for (size_t i = 0; i < n; i++) {
        if (token_buffer.size() + 1 > token_buffer.capacity()) {
          std::ostringstream msg;
          msg << "Max token length of " << MAX_TOKEN_LEN
              << " characters exceeded.";
          throw std::runtime_error(msg.str());
        }

        // Skip whitespace
        if (std::isspace(file_buffer[i])) {
          token_buffer.clear();
          continue;
        }

        // Add next character to token buffer
        token_buffer.push_back(file_buffer[i]);

        // Attempt to parse token from token buffer
        const char next = peek(i, file_buffer.data(), n);
        bool token_added = try_parse_token(token_buffer, next);

        if (!m_tokens.empty() && token_added) {
          Token token = m_tokens.back();
          logger.debug("Token: `{}`; Value: `{}`",
                       token_to_str(token.token_type).name,
                       token.value.has_value() ? token.value.value() : "None");
        }
      }

      logger.debug("Finished chunk {}", total_chunks++);
    }

    logger.debug("Total chunks read: {}", total_chunks);
    logger.debug("Total bytes read: {}", total);
  }

 private:
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
  //! ` ` if attempting to peek out of bounds.
  inline char peek(size_t current_index, const char* const buffer, size_t size,
                   size_t lookahead = 1) const noexcept {
    return current_index + lookahead > size ? ' ' : buffer[current_index + 1];
  }

  //! @brief Attmpt to parse a token from the buffer.
  //! @param token_buffer std::string The buffer from which to attempt to parse
  //! a token.
  //! @param next const char The next character in the buffer.
  //! @return bool Whether or not we successfully parsed a full token.
  bool try_parse_token(std::string& token_buffer, const char next) noexcept {
    if (token_buffer.empty()) {
      return false;
    }

    for (const TokenType token_type :
         enum_range(TokenType::_BEGIN, TokenType::NUM_TOKENS)) {
      if (std::find(SPECIAL_TOKENS.begin(), SPECIAL_TOKENS.end(), token_type) !=
          SPECIAL_TOKENS.end()) {
        // Int literal special case
        if (std::isdigit(token_buffer.back())) {
          if (!std::isdigit(next)) {
            m_tokens.emplace_back(TokenType::TT_INT_LITERAL, token_buffer);
            token_buffer.clear();
            return true;
          }
          return false;
          // Identifier special case
        } else if (std::isalpha(token_buffer.back())) {
          if (!std::isalpha(next)) {
            m_tokens.emplace_back(TokenType::TT_IDENTIFIER, token_buffer);
            token_buffer.clear();
            return true;
          }
          return false;
        }
        continue;
      } else {
        TokenString token_str = token_to_str(token_type);
        if (std::strcmp(token_buffer.c_str(), token_str.token.c_str()) == 0) {
          m_tokens.emplace_back(token_type);
          token_buffer.clear();
          return true;
        }
      }
    }
    return false;
  }

  //! @brief The size in bytes of the chunks to read from the file.
  constexpr static size_t BUFFER_SIZE = 4096;
  //! @brief The size in bytes of the maximum token length.
  constexpr static size_t MAX_TOKEN_LEN = 64;
  //! @brief The path to the file to tokenize.
  const std::filesystem::path m_path;
  //! @brief The tokens parsed from the file.
  std::vector<Token> m_tokens;
};

}  // namespace pimento::tokenization