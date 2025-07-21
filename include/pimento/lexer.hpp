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

namespace pimento {

class Lexer {
 public:
  Lexer(const std::filesystem::path& path) : m_path(path) {
    m_tokens.reserve(BUFFER_SIZE);
  }

  //! @brief Tokenize the input file.
  void tokenize() {
    auto& logger = utils::get_logger();
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

        if (std::isspace(file_buffer[i])) {
          token_buffer.clear();
          continue;
        }

        token_buffer.push_back(file_buffer[i]);

        // TODO(lthomas): I hate these strcmp calls, maybe there's a better way?
        // I also need to save specific data, like identifiers and int literals,
        // and I could probably clean this up by wrapping some of this logic
        // into a function
        if (std::strcmp(token_buffer.c_str(), "exit") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_EXIT));
          m_tokens.emplace_back(TokenType::TT_EXIT);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "if") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_IF));
          m_tokens.emplace_back(TokenType::TT_IF);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "elif") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_ELIF));
          m_tokens.emplace_back(TokenType::TT_ELIF);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "else") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_ELSE));
          m_tokens.emplace_back(TokenType::TT_ELSE);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "let") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_LET));
          m_tokens.emplace_back(TokenType::TT_LET);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "(") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_LEFT_PAREN));
          m_tokens.emplace_back(TokenType::TT_LEFT_PAREN);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), ")") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_RIGHT_PAREN));
          m_tokens.emplace_back(TokenType::TT_RIGHT_PAREN);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "{") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_LEFT_CURLY));
          m_tokens.emplace_back(TokenType::TT_LEFT_CURLY);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "}") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_RIGHT_CURLY));
          m_tokens.emplace_back(TokenType::TT_RIGHT_CURLY);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "^^") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_DOUBLE_CARET));
          m_tokens.emplace_back(TokenType::TT_DOUBLE_CARET);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "%") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_PERCENT));
          m_tokens.emplace_back(TokenType::TT_PERCENT);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "*") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_STAR));
          m_tokens.emplace_back(TokenType::TT_STAR);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "/") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_FORWARD_SLASH));
          m_tokens.emplace_back(TokenType::TT_FORWARD_SLASH);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "+") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_PLUS));
          m_tokens.emplace_back(TokenType::TT_PLUS);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "-") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_MINUS));
          m_tokens.emplace_back(TokenType::TT_MINUS);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), ";") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_SEMI));
          m_tokens.emplace_back(TokenType::TT_SEMI);
          token_buffer.clear();
        } else if (std::strcmp(token_buffer.c_str(), "=") == 0) {
          logger.debug("Token `{}`", token_to_str(TokenType::TT_EQUAL));
          m_tokens.emplace_back(TokenType::TT_EQUAL);
          token_buffer.clear();
        } else if (std::isdigit(file_buffer[i])) {
          if (i + 1 >= n || !std::isdigit(file_buffer[i + 1])) {
            logger.debug("Token `{}`", token_to_str(TokenType::TT_INT_LITERAL));
            m_tokens.emplace_back(TokenType::TT_INT_LITERAL);
            token_buffer.clear();
          }
        } else if (std::isalpha(file_buffer[i])) {
          if (i + 1 >= n || !std::isalpha(file_buffer[i + 1])) {
            logger.debug("Token `{}`", token_to_str(TokenType::TT_IDENTIFIER));
            m_tokens.emplace_back(TokenType::TT_IDENTIFIER);
            token_buffer.clear();
          }
        }
      }
      logger.debug("Finished chunk {}", total_chunks++);
    }

    logger.debug("Total chunks read: {}", total_chunks);
    logger.debug("Total bytes read: {}", total);
  }

 private:
  //! @brief The size in bytes of the chunks to read from the file.
  constexpr static size_t BUFFER_SIZE = 4096;
  //! @brief The size in bytes of the maximum token length.
  constexpr static size_t MAX_TOKEN_LEN = 64;
  //! @brief The path to the file to tokenize.
  const std::filesystem::path m_path;
  //! @brief The tokens parsed from the file.
  std::vector<Token> m_tokens;
};

}  // namespace pimento