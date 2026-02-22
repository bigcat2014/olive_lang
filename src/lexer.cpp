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
    auto& logger = utils::getLogger();

    while (!mInputBuffer.done()) {
        // Cache current offset, line number, and column number at start of parsing current token
        Token token;
        if (mTokenBuffer.str().empty()) {
            token.offset = mInputBuffer.getOffset();
            token.line   = mInputBuffer.getCurrentLine();
            token.column = mInputBuffer.getCurrentColumn();
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
                                parseIdent(token);
                        }
                        break;
                    default:
                        if (std::islower(next) == 0) {
                            pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                                    mInputBuffer.getCurrentLine() + 1,
                                                    mInputBuffer.getCurrentColumn(),
                                                    "Expected character matching [a-z_]."});
                        }
                        parseIdent(token);
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
                parseIdent(token);
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
                parseType(token);
                break;
            }
                // clang-format off
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': {
                // clang-format on
                parseNumericConst(token);
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
                createToken(token, TokenType::TT_COLON);
                break;
            }
            case ',': {
                createToken(token, TokenType::TT_COMMA);
                break;
            }
            case '{': {
                createToken(token, TokenType::TT_LEFT_CURLY);
                break;
            }
            case '(': {
                createToken(token, TokenType::TT_LEFT_PAREN);
                break;
            }
            case '[': {
                createToken(token, TokenType::TT_LEFT_SQUARE);
                break;
            }
            case '}': {
                createToken(token, TokenType::TT_RIGHT_CURLY);
                break;
            }
            case ')': {
                createToken(token, TokenType::TT_RIGHT_PAREN);
                break;
            }
            case ']': {
                createToken(token, TokenType::TT_RIGHT_SQUARE);
                break;
            }
            case ';': {
                createToken(token, TokenType::TT_SEMI);
                break;
            }
            // &, &=
            case '&': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_AMP_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_AMP);
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
                                createToken(token, TokenType::TT_CARET_CARET_EQUAL);
                                break;
                            default:
                                createToken(token, TokenType::TT_CARET_CARET);
                        }
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_CARET_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_CARET);
                }
                break;
            }
            // .
            case '.': {
                createToken(token, TokenType::TT_DOT);
                break;
            }
            // =, ==
            case '=': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_EQUAL_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_EQUAL);
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
                                createToken(token, TokenType::TT_FSLASH_FSLASH_EQUAL);
                                break;
                            default:
                                createToken(token, TokenType::TT_FSLASH_FSLASH);
                        }
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_FSLASH_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_FSLASH);
                }
                break;
            }
            // <, <=, <<
            case '<': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '<':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_LANGLE_LANGLE);
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_LANGLE_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_LANGLE);
                }
                break;
            }
            // -, --, -=
            case '-': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '-':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_MINUS_MINUS);
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_MINUS_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_MINUS);
                }
                break;
            }
            // %, %=
            case '%': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_PERCENT_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_PERCENT);
                }
                break;
            }
            // |, |=
            case '|': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_PIPE_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_PIPE);
                }
                break;
            }
            // +, ++, +=
            case '+': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '+':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_PLUS_PLUS);
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_PLUS_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_PLUS);
                }
                break;
            }
            // ?
            case '?': {
                createToken(token, TokenType::TT_QUESTION);
                break;
            }
            // >, >=, >>
            case '>': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '>':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_RANGLE_RANGLE);
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_RANGLE_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_RANGLE);
                }
                break;
            }
            // *, *=
            case '*': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_STAR_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_STAR);
                }
                break;
            }
            // ~, ~=
            case '~': {
                const char next = mInputBuffer.peek();
                switch (next) {
                    case std::char_traits<char>::eof():
                        break;
                    case '=':
                        mTokenBuffer << mInputBuffer.consume();
                        createToken(token, TokenType::TT_TILDE_EQUAL);
                        break;
                    default:
                        createToken(token, TokenType::TT_TILDE);
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
            std::cout << mInputBuffer.get(token.offset, token.span) << '\n';
        }
    }
}

void Lexer::addToken(const Token& token)
{
    mTokens.emplace_back(token);
    mTokenBuffer.str("");
    mTokenBuffer.clear();
}

void Lexer::createToken(Token& token, TokenType type) noexcept
{
    const std::string lexeme{mTokenBuffer.str()};
    token.tokenType = type;
    token.lexeme    = lexeme;
    token.span      = lexeme.length();
    addToken(token);
}

void Lexer::createIdentToken(Token& token, const std::string& /*value*/) noexcept
{
    const std::string lexeme{mTokenBuffer.str()};
    token.tokenType = TokenType::TT_IDENT;
    token.lexeme    = lexeme;
    token.span      = lexeme.length();
    addToken(token);
}

void Lexer::createTypeToken(Token& token, const std::string& /*value*/) noexcept
{
    const std::string lexeme{mTokenBuffer.str()};
    token.tokenType = TokenType::TT_TYPE_IDENT;
    token.lexeme    = lexeme;
    token.span      = lexeme.length();
    addToken(token);
}

void Lexer::parseIdent(Token& token) noexcept
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
                                    token.line + 1,
                                    token.column,
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

    createIdentToken(token, mTokenBuffer.str());
}

void Lexer::parseType(Token& token) noexcept
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
                                    token.line + 1,
                                    token.column,
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

    createTypeToken(token, mTokenBuffer.str());
}

void Lexer::parseNumericConst(Token& /*token*/) noexcept
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
    const size_t dotPos  = mantissaStr.find('.');
    int32_t decimalShift = 0;
    if (dotPos != std::string::npos) {
        decimalShift = static_cast<int32_t>(mantissaStr.size()) - static_cast<int32_t>(dotPos) - 1;
        mantissaStr.erase(dotPos, 1);  // remove '.'
    }

    // Convert mantissa digits to integer
    uint64_t decimalMantissa = 0;
    for (const char character : mantissaStr) {
        if (character < '0' || character > '9') {
            throw std::invalid_argument("Invalid digit in float");
        }
        decimalMantissa = (decimalMantissa * 10) + static_cast<uint64_t>(character - '0');
    }

    // Effective base-10 exponent
    const int effectiveExp10 = exponent - decimalShift;

    // 4. Convert decimal mantissa and exponent to binary
    const long double value = static_cast<long double>(decimalMantissa) * std::pow(10.0L, effectiveExp10);

    // Decompose into mantissa + binary exponent
    int binExp;
    const long double frac = std::frexp(value, &binExp);                  // frac in [0.5, 1)
    mantissa               = static_cast<uint64_t>(frac * (1ULL << 53));  // 53-bit mantissa for double
    exponent               = binExp - 53;

    return {mantissa, exponent, negative, FloatConst::Precision::FLOAT64};
}

}  // namespace pimento::tokenization
