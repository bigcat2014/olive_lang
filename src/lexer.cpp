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

      switch (file_buffer[m_buffer_index]) {
        case '_':
          // TODO(lthomas): parse_ident(file_buffer.data());
          advance(file_buffer);
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          advance(file_buffer);
          // TODO(lthomas): parse_numeric_const(file_buffer.data());
          break;
        case '#': {
          std::optional<char> next = peek(file_buffer);
          while (next.has_value() && next.value() != '\n') {
            advance(file_buffer);
            next = peek(file_buffer);
          }
          try_consume(file_buffer, '\n');
          break;
        }
        default:
          advance(file_buffer);
      }

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

  logger.debug("Total chunks read: {}", m_total_chunks);
  logger.debug("Total bytes read: {}", m_total_bytes);
}

[[nodiscard]] inline std::optional<char> Lexer::peek(const std::array<char, BUFFER_SIZE> &buffer) noexcept {
  if (m_buffer_index + 1 < buffer.size()) {
    return buffer[m_buffer_index + 1];
  }

  return {};
}

inline void Lexer::advance(const std::array<char, BUFFER_SIZE> &buffer) noexcept {
  // End of chunk or EoF
  if (buffer.size() <= m_buffer_index) { return; }

  char current = buffer[m_buffer_index++];
  if (current == '\n') {
    ++m_current_line;
    m_current_column = 0;
  } else {
    ++m_current_column;
  }
}

[[nodiscard]] inline std::optional<char> Lexer::try_consume(const std::array<char, BUFFER_SIZE> &buffer) noexcept {
  if (auto ret = peek(buffer)) {
    advance(buffer);
    return ret;
  }
  return {};
}

inline char Lexer::try_consume(const std::array<char, BUFFER_SIZE> &buffer, const char &character) noexcept {
  auto next = try_consume(buffer);
  if (!next.has_value() || next.value() != character) {
    // TODO(lthomas): I don't know if I want to log and exit here
    auto &logger = utils::get_logger();
    logger.error("Expected {} at line: {} column: {}", character, m_current_line, m_current_column);
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
FloatConst
Lexer::double_from_scientific(std::string &mantissa_str,
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
