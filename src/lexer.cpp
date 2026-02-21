#include <cctype>
#include <format>
#include <iostream>

#include <pimento/errors.hpp>
#include <pimento/lexer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

Lexer::Lexer(std::istream* istream)
    : mInputBuffer(istream)
{
    mTokens.reserve(BUFFER_SIZE);
    tokenize();
}

[[nodiscard]] const std::vector<Token>& Lexer::tokens() const noexcept
{
    return mTokens;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void Lexer::tokenize()
{
    auto& logger  = utils::getLogger();
    size_t offset = 0;
    size_t line   = 0;
    size_t column = 0;

    while (!mInputBuffer.done()) {
        // Cache current offset, line number, and column number at start of parsing current token
        if (mTokenBuffer.str().empty()) {
            offset = mInputBuffer.getOffset();
            line   = mInputBuffer.getCurrentLine();
            column = mInputBuffer.getCurrentColumn();
        }
        else if (mTokenBuffer.str().length() + 1 > MAX_TOKEN_LEN) {
            pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                    mInputBuffer.getCurrentLine() + 1,
                                    mInputBuffer.getCurrentColumn(),
                                    std::format("Max token length of {} characters exceeded.", MAX_TOKEN_LEN)});
        }

        char currentChar = mInputBuffer.consume();
        if (currentChar == std::char_traits<char>::eof()) {
            break;
        }
        mTokenBuffer << currentChar;

        switch (mTokenBuffer.str()[0]) {
            case std::char_traits<char>::eof():
                break;
            case '_': {
                // TODO(lthomas): I don't like this... Lots of reused code, not very clean.
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        if (mTokenBuffer.str().back() == '_') {
                            pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                                    mInputBuffer.getCurrentLine() + 1,
                                                    mInputBuffer.getCurrentColumn(),
                                                    "Expected character after '_'"});
                        }
                        break;
                    case '_':
                        mTokenBuffer << mInputBuffer.consume();
                        next = mInputBuffer.peek();
                        switch (next) {
                            case std::char_traits<char>::eof():
                                break;
                            case '_':
                                pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                                        mInputBuffer.getCurrentLine() + 1,
                                                        mInputBuffer.getCurrentColumn(),
                                                        "Too many '_' at start of identifier. Max is 2."});
                                break;
                            default:
                                if (std::islower(next) == 0) {
                                    pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                                            mInputBuffer.getCurrentLine() + 1,
                                                            mInputBuffer.getCurrentColumn(),
                                                            "Expected character matching [a-z_]."});
                                }
                                parseIdent(offset, line, column);
                        }
                        break;
                    default:
                        if (std::islower(next) == 0) {
                            pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                                    mInputBuffer.getCurrentLine() + 1,
                                                    mInputBuffer.getCurrentColumn(),
                                                    "Expected character matching [a-z_]."});
                        }
                        parseIdent(offset, line, column);
                        break;
                }
                break;
            }
                // clang-format off
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'g': case 'h': case 'i': case 'j': case 'k':
            case 'l': case 'm': case 'n': case 'o': case 'p':
            case 'q': case 'r': case 's': case 't': case 'u':
            case 'v': case 'w': case 'x': case 'y': case 'z': {
                // clang-format on
                parseIdent(offset, line, column);
                break;
            }
                // clang-format off
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            case 'G': case 'H': case 'I': case 'J': case 'K':
            case 'L': case 'M': case 'N': case 'O': case 'P':
            case 'Q': case 'R': case 'S': case 'T': case 'U':
            case 'V': case 'W': case 'X': case 'Y': case 'Z': {
                // clang-format on
                // TODO(lthomas): parseIdent or keyword
                parseType(offset, line, column);
                break;
            }
                // clang-format off
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': {
                // clang-format on
                parseNumericConst(offset, line, column);
                break;
            }
                // clang-format off
            case '\n': case '\t': case '\v': case '\f': case ' ': {
                // clang-format on
                mTokenBuffer.str("");
                mTokenBuffer.clear();
                break;
            }
            case ':': {
                createToken(TokenType::TT_COLON, offset, line, column);
                break;
            }
            case ',': {
                createToken(TokenType::TT_COMMA, offset, line, column);
                break;
            }
            case '{': {
                createToken(TokenType::TT_LEFT_CURLY, offset, line, column);
                break;
            }
            case '(': {
                createToken(TokenType::TT_LEFT_PAREN, offset, line, column);
                break;
            }
            case '[': {
                createToken(TokenType::TT_LEFT_SQUARE, offset, line, column);
                break;
            }
            case '}': {
                createToken(TokenType::TT_RIGHT_CURLY, offset, line, column);
                break;
            }
            case ')': {
                createToken(TokenType::TT_RIGHT_PAREN, offset, line, column);
                break;
            }
            case ']': {
                createToken(TokenType::TT_RIGHT_SQUARE, offset, line, column);
                break;
            }
            case ';': {
                createToken(TokenType::TT_SEMI, offset, line, column);
                break;
            }
            // &, &=
            case '&': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_AMP_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_AMP, offset, line, column);
                }
                break;
            }
            // ^, ^=, ^^, ^^=
            case '^': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '^':
                        mTokenBuffer << mInputBuffer.consume();
                        next = mInputBuffer.peek();
                        switch (next) {
                            case std::char_traits<char>::eof():
                                break;
                            case '=':
                                mTokenBuffer << mInputBuffer.consume();
                                createToken(TokenType::TT_CARET_CARET_EQUAL, offset, line, column);
                                break;
                            default:
                                createToken(TokenType::TT_CARET_CARET, offset, line, column);
                        }
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_CARET_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_CARET, offset, line, column);
                }
                break;
            }
            // .
            case '.': {
                createToken(TokenType::TT_DOT, offset, line, column);
                break;
            }
            // =, ==
            case '=': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_EQUAL_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_EQUAL, offset, line, column);
                }
                break;
            }
            // !
            // case '!': {
            //     // TODO(lthomas): Not sure if this symbol is necessary
            //     if (auto current = mFileBuffer.consume()) {
            //         mTokenBuffer << current.value();
            //     }
            //     else {
            //         break;
            //     }
            //     break;
            // }
            // /, //, /=, //=
            case '/': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '/':
                        mTokenBuffer << mInputBuffer.consume();
                        next = mInputBuffer.peek();
                        switch (next) {
                            case std::char_traits<char>::eof():
                                break;
                            case '=':
                                mTokenBuffer << mInputBuffer.consume();
                                createToken(TokenType::TT_FSLASH_FSLASH_EQUAL, offset, line, column);
                                break;
                            default:
                                createToken(TokenType::TT_FSLASH_FSLASH, offset, line, column);
                        }
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_FSLASH_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_FSLASH, offset, line, column);
                }
                break;
            }
            // <, <=, <<
            case '<': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '<':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_LANGLE_LANGLE, offset, line, column);
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_LANGLE_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_LANGLE, offset, line, column);
                }
                break;
            }
            // -, --, -=
            case '-': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '-':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_MINUS_MINUS, offset, line, column);
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_MINUS_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_MINUS, offset, line, column);
                }
                break;
            }
            // %, %=
            case '%': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_PERCENT_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_PERCENT, offset, line, column);
                }
                break;
            }
            // |, |=
            case '|': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_PIPE_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_PIPE, offset, line, column);
                }
                break;
            }
            // +, ++, +=
            case '+': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '+':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_PLUS_PLUS, offset, line, column);
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_PLUS_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_PLUS, offset, line, column);
                }
                break;
            }
            // ?
            case '?': {
                createToken(TokenType::TT_QUESTION, offset, line, column);
                break;
            }
            // >, >=, >>
            case '>': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '>':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_RANGLE_RANGLE, offset, line, column);
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_RANGLE_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_RANGLE, offset, line, column);
                }
                break;
            }
            // *, *=
            case '*': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_STAR_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_STAR, offset, line, column);
                }
                break;
            }
            // ~, ~=
            case '~': {
                char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(TokenType::TT_TILDE_EQUAL, offset, line, column);
                        break;
                    default:
                        createToken(TokenType::TT_TILDE, offset, line, column);
                }
                break;
            }
            // Comments
            case '#': {
                char next = mInputBuffer.consume();
                while (next != '\n' && next != std::char_traits<char>::eof()) {
                    next = mInputBuffer.consume();
                }

                mTokenBuffer.str("");
                mTokenBuffer.clear();
                break;
            }
            // Unknown symbol
            default: {
                pimento::errors::raise({pimento::errors::ErrorType::SYMBOL_ERROR,
                                        mInputBuffer.getCurrentLine() + 1,
                                        mInputBuffer.getCurrentColumn(),
                                        std::format("Unknown symbol '{}'.", currentChar)});
            }
        }
    }

    logger.trace(
        "Finished final line: {} with {} columns", mInputBuffer.getCurrentLine(), mInputBuffer.getCurrentColumn());
    logger.debug("Total chunks read: {}", mInputBuffer.getTotalChunks());
    logger.debug("Total bytes read: {}", mInputBuffer.getTotalBytes());

    if (logger.level() == spdlog::level::trace) {
        std::cout << "Tokens:" << '\n';
        for (const auto& token : mTokens) {
            std::cout << token << '\n';
        }

        // Testing using offset and span for pulling from input stream
        std::cout << "Tokens from file" << '\n';
        for (const auto& token : mTokens) {
            std::cout << mInputBuffer.get(token.sourceSpan.first, token.sourceSpan.second) << '\n';
        }
    }
}

void Lexer::createToken(TokenType type, size_t offset, size_t line, size_t column) noexcept
{
    std::string const lexme{mTokenBuffer.str()};
    mTokens.emplace_back(type, lexme, std::make_pair(offset, lexme.length()), std::make_pair(line, column));
    mTokenBuffer.str("");
    mTokenBuffer.clear();
}

void Lexer::createIdentToken(const std::string& value, size_t offset, size_t line, size_t column) noexcept
{
    std::string const lexme{mTokenBuffer.str()};
    // TODO(lthomas): Fix polymorphism. Vector of token pointers? std::variant?
    mTokens.emplace_back(
        TokenType::TT_IDENT, lexme, std::make_pair(offset, lexme.length()), std::make_pair(line, column));
    mTokenBuffer.str("");
    mTokenBuffer.clear();
}

void Lexer::createTypeToken(const std::string& value, size_t offset, size_t line, size_t column) noexcept
{
    std::string const lexme{mTokenBuffer.str()};
    // TODO(lthomas): Fix polymorphism. Vector of token pointers? std::variant?
    mTokens.emplace_back(
        TokenType::TT_TYPE_IDENT, lexme, std::make_pair(offset, lexme.length()), std::make_pair(line, column));
    mTokenBuffer.str("");
    mTokenBuffer.clear();
}

void Lexer::parseIdent(size_t offset, size_t line, size_t column)
{
    char next = mInputBuffer.peek();

    if (next == std::char_traits<char>::eof()) {
        // TODO(lthomas): What to do here...
        return;
    }

    while (isIdentChar(next)) {
        mTokenBuffer << mInputBuffer.consume();
        if (mTokenBuffer.str().length() + 1 > MAX_TOKEN_LEN) {
            pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                    line + 1,
                                    column,
                                    std::format("Max token length of {} characters exceeded.", MAX_TOKEN_LEN)});
        }
        next = mInputBuffer.peek();
    }

    if (next == std::char_traits<char>::eof()) {
        // TODO(lthomas): What to do here...
        return;
    }

    if (std::isspace(next) == 0) {
        pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                mInputBuffer.getCurrentLine() + 1,
                                mInputBuffer.getCurrentColumn(),
                                std::format("Unexpected character: '{}'.", next)});
    }

    createIdentToken(mTokenBuffer.str(), offset, line, column);
}

void Lexer::parseType(size_t offset, size_t line, size_t column)
{
    char next = mInputBuffer.peek();

    if (next == std::char_traits<char>::eof()) {
        // TODO(lthomas): What to do here...
        return;
    }

    while (isTypeChar(next)) {
        mTokenBuffer << mInputBuffer.consume();
        if (mTokenBuffer.str().length() + 1 > MAX_TOKEN_LEN) {
            pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                    line + 1,
                                    column,
                                    std::format("Max token length of {} characters exceeded.", MAX_TOKEN_LEN)});
        }
        next = mInputBuffer.peek();
    }

    if (next == std::char_traits<char>::eof()) {
        // TODO(lthomas): What to do here...
        return;
    }

    if (std::isspace(next) == 0) {
        pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                mInputBuffer.getCurrentLine() + 1,
                                mInputBuffer.getCurrentColumn(),
                                std::format("Unexpected character: '{}'.", next)});
    }

    createTypeToken(mTokenBuffer.str(), offset, line, column);
}

void Lexer::parseNumericConst(size_t offset, size_t line, size_t column)
{
    mTokenBuffer.str("");
    mTokenBuffer.clear();
}

// TODO(lthomas): Not IEEE-754 compliant yet.
FloatConst Lexer::doubleFromScientific(std::string& mantissaStr, const std::string& exponentStr)
{
    uint64_t mantissa;
    int32_t exponent;
    bool negative = false;
    size_t pos    = 0;

    // 1. Parse sign
    if (mantissaStr[pos] == '-') {
        negative = true;
        pos++;
    }
    else if (mantissaStr[pos] == '+') {
        pos++;
    }

    exponent = std::stoi(exponentStr);

    // 3. Normalize mantissa (remove decimal point)
    size_t const dotPos  = mantissaStr.find('.');
    int32_t decimalShift = 0;
    if (dotPos != std::string::npos) {
        decimalShift = static_cast<int32_t>(mantissaStr.size()) - static_cast<int32_t>(dotPos) - 1;
        mantissaStr.erase(dotPos, 1);  // remove '.'
    }

    // Convert mantissa digits to integer
    uint64_t decimalMantissa = 0;
    for (char const character : mantissaStr) {
        if (character < '0' || character > '9') {
            throw std::invalid_argument("Invalid digit in float");
        }
        decimalMantissa = (decimalMantissa * 10) + static_cast<uint64_t>(character - '0');
    }

    // Effective base-10 exponent
    int const effectiveExp10 = exponent - decimalShift;

    // 4. Convert decimal mantissa and exponent to binary
    long double const value = static_cast<long double>(decimalMantissa) * std::pow(10.0L, effectiveExp10);

    // Decompose into mantissa + binary exponent
    int binExp;
    long double const frac = std::frexp(value, &binExp);                  // frac in [0.5, 1)
    mantissa               = static_cast<uint64_t>(frac * (1ULL << 53));  // 53-bit mantissa for double
    exponent               = binExp - 53;

    return {mantissa, exponent, negative, FloatConst::Precision::FLOAT64};
}

}  // namespace pimento::tokenization
