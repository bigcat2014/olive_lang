#include <variant>

#include <pimento/lexer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

Lexer::Lexer(std::istream &istream) : m_stream(istream) {
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

  while (m_stream) {
    m_stream.read(file_buffer.data(), file_buffer.size());
    auto n = static_cast<size_t>(m_stream.gcount());
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

      // TODO(lthomas): Implement logging
      // if (logger.level() == spdlog::level::trace) {
      // }
    }

    logger.trace("Finished chunk {}", total_chunks++);
  }

  logger.debug("Total chunks read: {}", total_chunks);
  logger.debug("Total bytes read: {}", total);
}

[[nodiscard]] inline char Lexer::peek(size_t current_index,
                                      const char *const buffer, size_t size,
                                      size_t lookahead) noexcept {
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
