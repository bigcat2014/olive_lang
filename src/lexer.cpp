#include <variant>

#include <pimento/lexer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

Lexer::Lexer(std::shared_ptr<std::istream> istream) : p_stream(istream) {
  m_tokens.reserve(BUFFER_SIZE);
  tokenize();
}

[[nodiscard]] const std::vector<Token> &Lexer::tokens() const noexcept {
  return m_tokens;
}

void Lexer::tokenize() {
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

[[nodiscard]] inline char Lexer::peek(size_t current_index,
                                      const char *const buffer, size_t size,
                                      size_t lookahead) const noexcept {
  if (current_index + lookahead < size) {
    return buffer[current_index + lookahead];
  }

  // Return invalid character.
  return '\0';
}

bool Lexer::try_parse_token(std::string &token_buffer,
                            const char next) noexcept {
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
        m_tokens.push_back(
            TokenFactory::create_token(TokenType::TT_IDENTIFIER, token_buffer));
        token_buffer.clear();
        return true;
      }
    }
  }

  return false;
}

void Lexer::TraceTokenVisitor::operator()(
    const BinOpProperties &properties) const noexcept {
  output << "; Binary operator with prec: "
         << std::to_string(properties.precedence) << " and associativity: "
         << TokenTypeUtil::get_associativity_str(properties.associativity);
}

void Lexer::TraceTokenVisitor::operator()(
    const IntLitProperties &properties) const noexcept {
  output << "; Int literal with value: " << properties.value;
}

void Lexer::TraceTokenVisitor::operator()(
    const IdentProperties &properties) const noexcept {
  output << "; Identifier: " << properties.identifier;
}

void Lexer::TraceTokenVisitor::operator()(
    const std::monostate &) const noexcept {
  // No-op
}

} // namespace pimento::tokenization
