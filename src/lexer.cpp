#include <cctype>
#include <format>
#include <iostream>
#include <variant>

#include <pimento/errors.hpp>
#include <pimento/lexer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

Lexer::Lexer(std::istream &istream) : m_file_buffer(istream) {
  m_tokens.reserve(InputBuffer::BUFFER_SIZE);
  // tokenize();
}

[[nodiscard]] const std::vector<Token> &Lexer::tokens() const noexcept {
  return m_tokens;
}

void Lexer::tokenize() {
  auto &logger = utils::get_logger();
  size_t offset, line, column;

  while (!m_file_buffer.done()) {
    // Cache current offset, line number, and column number at start of parsing
    // current token
    if (m_token_buffer.str().length() == 0) {
      offset = m_file_buffer.get_offset();
      line = m_file_buffer.get_current_line();
      column = m_file_buffer.get_current_column();
    } else if (m_token_buffer.str().length() + 1 > MAX_TOKEN_LEN) {
      pimento::errors::raise(
          {pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
           m_file_buffer.get_current_line() + 1,
           m_file_buffer.get_current_column(),
           std::format("Max token length of {} characters exceeded.",
                       MAX_TOKEN_LEN)});
    }

    char current_char;
    if (auto current = m_file_buffer.consume()) {
      current_char = current.value();
    } else {
      break;
    }
    m_token_buffer << current_char;

    switch (m_token_buffer.str()[0]) {
    case '_':
      // TODO(lthomas): I don't like this... Lots of reused code, not very
      // clean.
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '_':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          if (auto next = m_file_buffer.peek()) {
            switch (next.value()) {
            case '_':
              pimento::errors::raise(
                  {pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                   m_file_buffer.get_current_line() + 1,
                   m_file_buffer.get_current_column(),
                   "Too many '_' at start of identifier. Max is 2."});
            default:
              if (!std::islower(next.value())) {
                pimento::errors::raise(
                    {pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                     m_file_buffer.get_current_line() + 1,
                     m_file_buffer.get_current_column(),
                     "Expected character matching [a-z_]."});
              }
              parse_ident(offset, line, column);
            }
          }
          break;
        default:
          if (!std::islower(next.value())) {
            pimento::errors::raise(
                {pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                 m_file_buffer.get_current_line() + 1,
                 m_file_buffer.get_current_column(),
                 "Expected character matching [a-z_]."});
          }
          parse_ident(offset, line, column);
          break;
        }
      } else if (m_token_buffer.str().back() == '_') {
        pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                m_file_buffer.get_current_line() + 1,
                                m_file_buffer.get_current_column(),
                                "Expected character after '_'"});
      }
      break;
      // clang-format off
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k':
    case 'l': case 'm': case 'n': case 'o': case 'p':
    case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':
      // clang-format on
      parse_ident(offset, line, column);
      break;
      // clang-format off
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K':
    case 'L': case 'M': case 'N': case 'O': case 'P':
    case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z': {
      // clang-format on
      // TODO(lthomas): parse_ident or keyword
      parse_type(offset, line, column);
      break;
    }
      // clang-format off
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
      // clang-format on
      parse_numeric_const(offset, line, column);
      break;
    }
      // clang-format off
    case '\n': case '\t': case '\v': case '\f': case ' ': {
      // clang-format on
      // m_file_buffer.advance();
      m_token_buffer.str("");
      m_token_buffer.clear();
      break;
    }
    case ':':
      create_token(TokenType::COLON, offset, line, column);
      break;
    case ',':
      create_token(TokenType::COMMA, offset, line, column);
      break;
    case '{':
      create_token(TokenType::LEFT_CURLY, offset, line, column);
      break;
    case '(':
      create_token(TokenType::LEFT_PAREN, offset, line, column);
      break;
    case '[':
      create_token(TokenType::LEFT_SQUARE, offset, line, column);
      break;
    case '}':
      create_token(TokenType::RIGHT_CURLY, offset, line, column);
      break;
    case ')':
      create_token(TokenType::RIGHT_PAREN, offset, line, column);
      break;
    case ']':
      create_token(TokenType::RIGHT_SQUARE, offset, line, column);
      break;
    case ';':
      create_token(TokenType::SEMI, offset, line, column);
      break;
    // &, &=
    case '&':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::AMP_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::AMP, offset, line, column);
        }
      }
      break;
    // ^, ^=, ^^, ^^=
    case '^':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '^':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          if (auto next = m_file_buffer.peek()) {
            switch (next.value()) {
            case '=':
              if (auto current = m_file_buffer.consume()) {
                m_token_buffer << current.value();
              } else {
                break;
              }
              create_token(TokenType::CARET_CARET_EQUAL, offset, line, column);
              break;
            default:
              create_token(TokenType::CARET_CARET, offset, line, column);
            }
          }
          break;
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::CARET_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::CARET, offset, line, column);
        }
      }
      break;
    // .
    case '.':
      create_token(TokenType::DOT, offset, line, column);
      break;
    // =, ==
    case '=':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::EQUAL_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::EQUAL, offset, line, column);
        }
      }
      break;
    // !
    // case '!':
    //   // TODO(lthomas): Not sure if this symbol is necessary
    //   if (auto current = m_file_buffer.consume()) {
    //     m_token_buffer << current.value();
    //   } else {
    //     break;
    //   }
    //   break;
    // /, //, /=, //=
    case '/':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '/':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          if (auto next = m_file_buffer.peek()) {
            switch (next.value()) {
            case '=':
              if (auto current = m_file_buffer.consume()) {
                m_token_buffer << current.value();
              } else {
                break;
              }
              create_token(TokenType::FSLASH_FSLASH_EQUAL, offset, line,
                           column);
              break;
            default:
              create_token(TokenType::FSLASH_FSLASH, offset, line, column);
            }
          }
          break;
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::FSLASH_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::FSLASH, offset, line, column);
        }
      }
      break;
    // <, <=, <<
    case '<':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '<':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::LANGLE_LANGLE, offset, line, column);
          break;
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::LANGLE_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::LANGLE, offset, line, column);
        }
      }
      break;
    // -, --, -=
    case '-':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '-':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::MINUS_MINUS, offset, line, column);
          break;
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::MINUS_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::MINUS, offset, line, column);
        }
      }
      break;
    // %, %=
    case '%':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::PERCENT_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::PERCENT, offset, line, column);
        }
      }
      break;
    // |, |=
    case '|':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::PIPE_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::PIPE, offset, line, column);
        }
      }
      break;
    // +, ++, +=
    case '+':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '+':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::PLUS_PLUS, offset, line, column);
          break;
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::PLUS_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::PLUS, offset, line, column);
        }
      }
      break;
    // ?
    case '?':
      create_token(TokenType::QUESTION, offset, line, column);
      break;
    // >, >=, >>
    case '>':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '>':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::RANGLE_RANGLE, offset, line, column);
          break;
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::RANGLE_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::RANGLE, offset, line, column);
        }
      }
      break;
    // *, *=
    case '*':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::STAR_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::STAR, offset, line, column);
        }
      }
      break;
    // ~, ~=
    case '~':
      if (auto next = m_file_buffer.peek()) {
        switch (next.value()) {
        case '=':
          if (auto current = m_file_buffer.consume()) {
            m_token_buffer << current.value();
          } else {
            break;
          }
          create_token(TokenType::TILDE_EQUAL, offset, line, column);
          break;
        default:
          create_token(TokenType::TILDE, offset, line, column);
        }
      }
      break;
    // Comments
    case '#': {
      std::optional<char> next;
      do {
        next = m_file_buffer.consume();
      } while (next.has_value() && next.value() != '\n');

      m_token_buffer.str("");
      m_token_buffer.clear();
      break;
    }
    // Unknown symbol
    default:
      pimento::errors::raise(
          {pimento::errors::ErrorType::SYMBOL_ERROR,
           m_file_buffer.get_current_line() + 1,
           m_file_buffer.get_current_column(),
           std::format("Unknown symbol '{}'.", current_char)});
    }
  }

  logger.trace("Finished final line: {} with {} columns",
               m_file_buffer.get_current_line(),
               m_file_buffer.get_current_column());
  logger.debug("Total chunks read: {}", m_file_buffer.get_total_chunks());
  logger.debug("Total bytes read: {}", m_file_buffer.get_total_bytes());

  if (logger.level() == spdlog::level::trace) {
    std::cout << "Tokens:" << std::endl;
    for (const auto &token : m_tokens) {
      std::cout << token << std::endl;
    }

    // Testing using offset and span for pulling from input stream
    std::cout << "Tokens from file" << std::endl;
    for (const auto &token : m_tokens) {
      std::cout << m_file_buffer.get(token.source_span.first,
                                     token.source_span.second)
                << std::endl;
    }
  }
}

void Lexer::create_token(TokenType type, size_t offset, size_t line,
                         size_t column) noexcept {
  std::string lexme{m_token_buffer.str()};
  m_tokens.emplace_back(type, lexme, std::make_pair(offset, lexme.length()),
                        std::make_pair(line, column));
  m_token_buffer.str("");
  m_token_buffer.clear();
}

void Lexer::create_ident_token(const std::string &value, size_t offset,
                               size_t line, size_t column) noexcept {
  std::string lexme{m_token_buffer.str()};
  // TODO(lthomas): Fix polymorphism. Vector of token pointers? std::variant?
  m_tokens.emplace_back(TokenType::IDENT, lexme,
                        std::make_pair(offset, lexme.length()),
                        std::make_pair(line, column));
  m_token_buffer.str("");
  m_token_buffer.clear();
}

void Lexer::create_type_token(const std::string &value, size_t offset,
                              size_t line, size_t column) noexcept {
  std::string lexme{m_token_buffer.str()};
  // TODO(lthomas): Fix polymorphism. Vector of token pointers? std::variant?
  m_tokens.emplace_back(TokenType::TYPE_IDENT, lexme,
                        std::make_pair(offset, lexme.length()),
                        std::make_pair(line, column));
  m_token_buffer.str("");
  m_token_buffer.clear();
}

void Lexer::parse_ident(size_t offset, size_t line, size_t column) {
  if (auto next = m_file_buffer.peek()) {
    do {
      m_token_buffer << m_file_buffer.consume().value();
      next = m_file_buffer.peek();
    } while (next.has_value() && is_ident_char(next.value()));

    next = m_file_buffer.peek();
    if (next.has_value()) {
      if (!std::isspace(next.value())) {
        pimento::errors::raise(
            {pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
             m_file_buffer.get_current_line() + 1,
             m_file_buffer.get_current_column(),
             std::format("Unexpected character: '{}'.", next.value())});
      }
    }

    create_ident_token(m_token_buffer.str(), offset, line, column);
  }
}

void Lexer::parse_type(size_t offset, size_t line, size_t column) {
  if (auto next = m_file_buffer.peek()) {
    do {
      m_token_buffer << m_file_buffer.consume().value();
      next = m_file_buffer.peek();
    } while (next.has_value() && is_type_char(next.value()));

    next = m_file_buffer.peek();
    if (next.has_value()) {
      if (!std::isspace(next.value())) {
        pimento::errors::raise(
            {pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
             m_file_buffer.get_current_line() + 1,
             m_file_buffer.get_current_column(),
             std::format("Unexpected character: '{}'.", next.value())});
      }
    }

    create_type_token(m_token_buffer.str(), offset, line, column);
  }
}

void Lexer::parse_numeric_const(size_t offset, size_t line, size_t column) {
  m_token_buffer.str("");
  m_token_buffer.clear();
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
