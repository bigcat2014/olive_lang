#include <variant>

#include <pimento/lexer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

Lexer::Lexer(std::istream &istream) : m_stream(istream) {
  m_tokens.reserve(BUFFER_SIZE);
  // tokenize();
}

[[nodiscard]] const std::vector<Token> &Lexer::tokens() const noexcept {
  return m_tokens;
}

void Lexer::tokenize() {
  auto &logger = utils::get_logger();

  std::array<char, BUFFER_SIZE> file_buffer;
  std::string token_buffer;
  token_buffer.reserve(MAX_TOKEN_LEN);

  while (m_stream) {
    m_stream.read(file_buffer.data(), file_buffer.size());
    auto n = static_cast<size_t>(m_stream.gcount());
    if (n <= 0) {
      break;
    }

    m_total_bytes += n;
    m_buffer_index = 0;
    while (m_buffer_index < n) {
      if (token_buffer.size() + 1 > MAX_TOKEN_LEN) {
        logger.error("Max token length of {} characters exceeded.",
                     MAX_TOKEN_LEN);
        exit(EXIT_FAILURE);
      }

      // clang-format off
      // TODO(lthomas): Some line/column number calculations are off
      switch (file_buffer[m_buffer_index]) {
      case '_':
        // TODO(lthomas): parse_ident(file_buffer.data());
        advance(file_buffer, n);
        m_token_buffer.str(std::string());
        m_token_buffer.clear();
        break;
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      case 'g': case 'h': case 'i': case 'j': case 'k':
      case 'l': case 'm': case 'n': case 'o': case 'p':
      case 'q': case 'r': case 's': case 't': case 'u':
      case 'v': case 'w': case 'x': case 'y': case 'z':
        // TODO(lthomas): parse_ident or keyword
        advance(file_buffer, n);
        m_token_buffer.str(std::string());
        m_token_buffer.clear();
        break;
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      case 'G': case 'H': case 'I': case 'J': case 'K':
      case 'L': case 'M': case 'N': case 'O': case 'P':
      case 'Q': case 'R': case 'S': case 'T': case 'U':
      case 'V': case 'W': case 'X': case 'Y': case 'Z':
        // TODO(lthomas): parse_ident or keyword
        advance(file_buffer, n);
        m_token_buffer.str(std::string());
        m_token_buffer.clear();
        break;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        // TODO(lthomas): parse_numeric_const(file_buffer.data());
        advance(file_buffer, n);
        m_token_buffer.str(std::string());
        m_token_buffer.clear();
        break;
      case '\n': case '\t': case '\v': case '\f': case ' ':
        advance(file_buffer, n);
        m_token_buffer.str(std::string());
        m_token_buffer.clear();
        break;
      case ':':
        create_token(file_buffer, n, TokenType::COLON);
        break;
      case ',':
        create_token(file_buffer, n, TokenType::COMMA);
        break;
      case '{':
        create_token(file_buffer, n, TokenType::LEFT_CURLY);
        break;
      case '(':
        create_token(file_buffer, n, TokenType::LEFT_PAREN);
        break;
      case '[':
        create_token(file_buffer, n, TokenType::LEFT_SQUARE);
        break;
      case '}':
        create_token(file_buffer, n, TokenType::RIGHT_CURLY);
        break;
      case ')':
        create_token(file_buffer, n, TokenType::RIGHT_PAREN);
        break;
      case ']':
        create_token(file_buffer, n, TokenType::RIGHT_SQUARE);
        break;
      case ';':
        create_token(file_buffer, n, TokenType::SEMI);
        break;
      // &, &=
      case '&':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::AMP_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::AMP);
          }
        }
        break;
      // ^, ^=, ^^, ^^=
      case '^':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value()) {
          case '^':
            advance(file_buffer, n);
            if (auto next = peek(file_buffer, n)) {
              switch (next.value()) {
              case '=':
                advance(file_buffer, n);
                create_token(file_buffer, n, TokenType::CARET_CARET_EQUAL);
                break;
              default:
                create_token(file_buffer, n, TokenType::CARET_CARET);
              }
            }
            break;
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::CARET_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::CARET);
          }
        }
        break;
      // .
      case '.':
        create_token(file_buffer, n, TokenType::DOT);
        break;
      // =, ==
      case '=':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::EQUAL_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::EQUAL);
          }
        }
        break;
      
      case '!':
        // TODO(lthomas): Not sure if this symbol is necessary
        advance(file_buffer, n);
        break;
      // /, //, /=, //=
      case '/':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value()) {
          case '/':
            advance(file_buffer, n);
            if (auto next = peek(file_buffer, n)) {
              switch (next.value()) {
              case '=':
                advance(file_buffer, n);
                create_token(file_buffer, n, TokenType::FSLASH_FSLASH_EQUAL);
                break;
              default:
                create_token(file_buffer, n, TokenType::FSLASH_FSLASH);
              }
            }
            break;
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::FSLASH_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::FSLASH);
          }
        }
        break;
      // <, <=, <<
      case '<':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
          case '<':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::LANGLE_LANGLE);
            break;
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::LANGLE_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::LANGLE);
          }
        }
        break;
      // -, --, -=
      case '-':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
          case '-':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::MINUS_MINUS);
            break;
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::MINUS_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::MINUS);
          }
        }
        break;
      // %, %=
      case '%':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::PERCENT_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::PERCENT);
          }
        }
        break;
      // |, |=
      case '|':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::PIPE_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::PIPE);
          }
        }
        break;
      // +, ++, +=
      case '+':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
          case '+':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::PLUS_PLUS);
            break;
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::PLUS_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::PLUS);
          }
        }
        break;
      // ?
      case '?':
        create_token(file_buffer, n, TokenType::QUESTION);
        break;
      // >, >=, >>
      case '>':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
          case '>':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::RANGLE_RANGLE);
            break;
          case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::RANGLE_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::RANGLE);
          }
        }
        break;
      // *, *=
      case '*':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
            case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::STAR_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::STAR);
          }
        }
        break;
      // ~, ~=
      case '~':
        if (auto next = peek(file_buffer, n)) {
          switch (next.value())
          {
            case '=':
            advance(file_buffer, n);
            create_token(file_buffer, n, TokenType::TILDE_EQUAL);
            break;
          default:
            create_token(file_buffer, n, TokenType::TILDE);
          }
        }
        break;
      // Comments
      case '#': {
        std::optional<char> next = peek(file_buffer, n);
        while (next.has_value() && next.value() != '\n') {
          advance(file_buffer, n);
          next = peek(file_buffer, n);
        }
        advance(file_buffer, n);
        m_token_buffer.str(std::string());
        m_token_buffer.clear();
        break;
      }
      // Unknown symbol
      default:
        logger.error("Unknown symbol {}", file_buffer[m_buffer_index]);
        advance(file_buffer, n);
        m_token_buffer.str(std::string());
        m_token_buffer.clear();
      }
      // clang-format on

      // Add next character to token buffer
      // token_buffer.push_back(file_buffer[m_buffer_index]);

      // Attempt to parse token from token buffer
      // const char next = peek(m_buffer_index, file_buffer.data(), n, 1);
      // bool token_added = try_parse_token(token_buffer, next);

      // TODO(lthomas): Implement logging
      // if (logger.level() == spdlog::level::trace) {
      // }
      // ++m_buffer_index;
    }

    logger.trace("Finished chunk {}", m_total_chunks++);
  }

  logger.trace("Finished final line: {} with {} columns", m_current_line,
               m_current_column);
  logger.debug("Total chunks read: {}", m_total_chunks);
  logger.debug("Total bytes read: {}", m_total_bytes);
}

void Lexer::create_token(const std::array<char, BUFFER_SIZE> &buffer, size_t size, TokenType type) noexcept {
  advance(buffer, size);
  m_tokens.emplace_back(type, m_token_buffer.str(),
      std::make_pair(m_current_line - 1, m_current_column - 1));
  m_token_buffer.str(std::string());
  m_token_buffer.clear();
}

[[nodiscard]] inline std::optional<char>
Lexer::peek(const std::array<char, BUFFER_SIZE> &buffer, size_t size) noexcept {
  if (m_buffer_index + 1 < size) {
    return buffer[m_buffer_index + 1];
  }

  return {};
}

inline bool Lexer::advance(const std::array<char, BUFFER_SIZE> &buffer,
                           size_t size) noexcept {
  // End of chunk or EoF
  if (m_buffer_index >= size) {
    return false;
  }

  auto &logger = utils::get_logger();

  char current = buffer[m_buffer_index++];
  if (current == '\n') {
    logger.trace("Finished line: {} with {} columns", m_current_line,
                 m_current_column);
    ++m_current_line;
    m_current_column = 0;
  } else {
    m_token_buffer << current;
    ++m_current_column;
  }
  return true;
}

[[nodiscard]] inline std::optional<char>
Lexer::try_consume(const std::array<char, BUFFER_SIZE> &buffer,
                   size_t size) noexcept {
  if (auto ret = peek(buffer, size)) {
    advance(buffer, size);
    return ret;
  }
  return {};
}

inline char Lexer::try_consume(const std::array<char, BUFFER_SIZE> &buffer,
                               size_t size, const char &character) noexcept {
  auto next = try_consume(buffer, size);
  if (!next.has_value() || next.value() != character) {
    // TODO(lthomas): I don't know if I want to log and exit here
    auto &logger = utils::get_logger();
    logger.error("Expected {} at line: {} column: {}", character,
                 m_current_line, m_current_column);
    exit(EXIT_FAILURE);
  }
  return next.value();
}

bool Lexer::try_parse_token(std::string &token_buffer,
                            const char next) noexcept {
  if (token_buffer.empty()) {
    return false;
  }

  // TODO(lthomas): Implement token parsing.

  return false;
}

// TODO(lthomas): Not IEEE-754 compliant yet.
FloatConst Lexer::double_from_scientific(std::string &mantissa_str,
                                         const std::string &exponent_str) {
  uint64_t mantissa;
  int exponent;
  bool negative = false;
  size_t pos = 0;

  // 1. Parse sign
  if (mantissa_str[pos] == '-') {
    negative = true;
    pos++;
  } else if (mantissa_str[pos] == '+') {
    pos++;
  }

  exponent = std::stoi(exponent_str);

  // 3. Normalize mantissa (remove decimal point)
  size_t dot_pos = mantissa_str.find('.');
  int decimal_shift = 0;
  if (dot_pos != std::string::npos) {
    decimal_shift = mantissa_str.size() - dot_pos - 1;
    mantissa_str.erase(dot_pos, 1); // remove '.'
  }

  // Convert mantissa digits to integer
  uint64_t decimal_mantissa = 0;
  for (char c : mantissa_str) {
    if (c < '0' || c > '9')
      throw std::invalid_argument("Invalid digit in float");
    decimal_mantissa = decimal_mantissa * 10 + (c - '0');
  }

  // Effective base-10 exponent
  int effective_exp_10 = exponent - decimal_shift;

  // 4. Convert decimal mantissa and exponent to binary
  long double value = static_cast<long double>(decimal_mantissa) *
                      std::pow(10.0L, effective_exp_10);

  // Decompose into mantissa + binary exponent
  int bin_exp;
  long double frac = std::frexp(value, &bin_exp); // frac in [0.5, 1)
  mantissa =
      static_cast<uint64_t>(frac * (1ull << 53)); // 53-bit mantissa for double
  exponent = bin_exp - 53;

  return FloatConst(mantissa, exponent, negative,
                    FloatConst::Precision::FLOAT64);
}

} // namespace pimento::tokenization
