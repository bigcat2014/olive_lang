#include <cctype>
#include <format>
#include <iostream>

#include <pimento/errors.hpp>
#include <pimento/lexer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

Lexer::Lexer(std::istream& istream)
    : mFileBuffer(istream)
{
    mTokens.reserve(InputBuffer::BUFFER_SIZE);
    // tokenize();
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

    while (!mFileBuffer.done()) {
        // Cache current offset, line number, and column number at start of parsing
        // current token
        if (mTokenBuffer.str().empty()) {
            offset = mFileBuffer.getOffset();
            line   = mFileBuffer.getCurrentLine();
            column = mFileBuffer.getCurrentColumn();
        }
        else if (mTokenBuffer.str().length() + 1 > MAX_TOKEN_LEN) {
            pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                    mFileBuffer.getCurrentLine() + 1,
                                    mFileBuffer.getCurrentColumn(),
                                    std::format("Max token length of {} characters exceeded.", MAX_TOKEN_LEN)});
        }

        char currentChar;
        if (auto current = mFileBuffer.consume()) {
            currentChar = current.value();
        }
        else {
            break;
        }
        mTokenBuffer << currentChar;

        switch (mTokenBuffer.str()[0]) {
            case '_':
                // TODO(lthomas): I don't like this... Lots of reused code, not very
                // clean.
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '_':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            if (auto next = mFileBuffer.peek()) {
                                switch (next.value()) {
                                    case '_':
                                        pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                                                mFileBuffer.getCurrentLine() + 1,
                                                                mFileBuffer.getCurrentColumn(),
                                                                "Too many '_' at start of identifier. Max is 2."});
                                        break;
                                    default:
                                        if (std::islower(next.value()) == 0) {
                                            pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                                                    mFileBuffer.getCurrentLine() + 1,
                                                                    mFileBuffer.getCurrentColumn(),
                                                                    "Expected character matching [a-z_]."});
                                        }
                                        parseIdent(offset, line, column);
                                }
                            }
                            break;
                        default:
                            if (std::islower(next.value()) == 0) {
                                pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                                        mFileBuffer.getCurrentLine() + 1,
                                                        mFileBuffer.getCurrentColumn(),
                                                        "Expected character matching [a-z_]."});
                            }
                            parseIdent(offset, line, column);
                            break;
                    }
                }
                else if (mTokenBuffer.str().back() == '_') {
                    pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                            mFileBuffer.getCurrentLine() + 1,
                                            mFileBuffer.getCurrentColumn(),
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
                parseIdent(offset, line, column);
                break;
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
                // mFileBuffer.advance();
                mTokenBuffer.str("");
                mTokenBuffer.clear();
                break;
            }
            case ':':
                createToken(TokenType::COLON, offset, line, column);
                break;
            case ',':
                createToken(TokenType::COMMA, offset, line, column);
                break;
            case '{':
                createToken(TokenType::LEFT_CURLY, offset, line, column);
                break;
            case '(':
                createToken(TokenType::LEFT_PAREN, offset, line, column);
                break;
            case '[':
                createToken(TokenType::LEFT_SQUARE, offset, line, column);
                break;
            case '}':
                createToken(TokenType::RIGHT_CURLY, offset, line, column);
                break;
            case ')':
                createToken(TokenType::RIGHT_PAREN, offset, line, column);
                break;
            case ']':
                createToken(TokenType::RIGHT_SQUARE, offset, line, column);
                break;
            case ';':
                createToken(TokenType::SEMI, offset, line, column);
                break;
            // &, &=
            case '&':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::AMP_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::AMP, offset, line, column);
                    }
                }
                break;
            // ^, ^=, ^^, ^^=
            case '^':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '^':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            if (auto next = mFileBuffer.peek()) {
                                switch (next.value()) {
                                    case '=':
                                        if (auto current = mFileBuffer.consume()) {
                                            mTokenBuffer << current.value();
                                        }
                                        else {
                                            break;
                                        }
                                        createToken(TokenType::CARET_CARET_EQUAL, offset, line, column);
                                        break;
                                    default:
                                        createToken(TokenType::CARET_CARET, offset, line, column);
                                }
                            }
                            break;
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::CARET_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::CARET, offset, line, column);
                    }
                }
                break;
            // .
            case '.':
                createToken(TokenType::DOT, offset, line, column);
                break;
            // =, ==
            case '=':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::EQUAL_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::EQUAL, offset, line, column);
                    }
                }
                break;
            // !
            // case '!':
            //   // TODO(lthomas): Not sure if this symbol is necessary
            //   if (auto current = mFileBuffer.consume()) {
            //     mTokenBuffer << current.value();
            //   } else {
            //     break;
            //   }
            //   break;
            // /, //, /=, //=
            case '/':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '/':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            if (auto next = mFileBuffer.peek()) {
                                switch (next.value()) {
                                    case '=':
                                        if (auto current = mFileBuffer.consume()) {
                                            mTokenBuffer << current.value();
                                        }
                                        else {
                                            break;
                                        }
                                        createToken(TokenType::FSLASH_FSLASH_EQUAL, offset, line, column);
                                        break;
                                    default:
                                        createToken(TokenType::FSLASH_FSLASH, offset, line, column);
                                }
                            }
                            break;
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::FSLASH_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::FSLASH, offset, line, column);
                    }
                }
                break;
            // <, <=, <<
            case '<':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '<':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::LANGLE_LANGLE, offset, line, column);
                            break;
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::LANGLE_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::LANGLE, offset, line, column);
                    }
                }
                break;
            // -, --, -=
            case '-':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '-':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::MINUS_MINUS, offset, line, column);
                            break;
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::MINUS_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::MINUS, offset, line, column);
                    }
                }
                break;
            // %, %=
            case '%':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::PERCENT_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::PERCENT, offset, line, column);
                    }
                }
                break;
            // |, |=
            case '|':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::PIPE_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::PIPE, offset, line, column);
                    }
                }
                break;
            // +, ++, +=
            case '+':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '+':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::PLUS_PLUS, offset, line, column);
                            break;
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::PLUS_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::PLUS, offset, line, column);
                    }
                }
                break;
            // ?
            case '?':
                createToken(TokenType::QUESTION, offset, line, column);
                break;
            // >, >=, >>
            case '>':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '>':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::RANGLE_RANGLE, offset, line, column);
                            break;
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::RANGLE_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::RANGLE, offset, line, column);
                    }
                }
                break;
            // *, *=
            case '*':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::STAR_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::STAR, offset, line, column);
                    }
                }
                break;
            // ~, ~=
            case '~':
                if (auto next = mFileBuffer.peek()) {
                    switch (next.value()) {
                        case '=':
                            if (auto current = mFileBuffer.consume()) {
                                mTokenBuffer << current.value();
                            }
                            else {
                                break;
                            }
                            createToken(TokenType::TILDE_EQUAL, offset, line, column);
                            break;
                        default:
                            createToken(TokenType::TILDE, offset, line, column);
                    }
                }
                break;
            // Comments
            case '#': {
                std::optional<char> next = mFileBuffer.consume();
                while (next.has_value() && next.value() != '\n') {
                    next = mFileBuffer.consume();
                }

                mTokenBuffer.str("");
                mTokenBuffer.clear();
                break;
            }
            // Unknown symbol
            default:
                pimento::errors::raise({pimento::errors::ErrorType::SYMBOL_ERROR,
                                        mFileBuffer.getCurrentLine() + 1,
                                        mFileBuffer.getCurrentColumn(),
                                        std::format("Unknown symbol '{}'.", currentChar)});
        }
    }

    logger.trace(
        "Finished final line: {} with {} columns", mFileBuffer.getCurrentLine(), mFileBuffer.getCurrentColumn());
    logger.debug("Total chunks read: {}", mFileBuffer.getTotalChunks());
    logger.debug("Total bytes read: {}", mFileBuffer.getTotalBytes());

    if (logger.level() == spdlog::level::trace) {
        std::cout << "Tokens:" << '\n';
        for (const auto& token : mTokens) {
            std::cout << token << '\n';
        }

        // Testing using offset and span for pulling from input stream
        std::cout << "Tokens from file" << '\n';
        for (const auto& token : mTokens) {
            std::cout << mFileBuffer.get(token.sourceSpan.first, token.sourceSpan.second) << '\n';
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

void Lexer::createIdentToken(const std::string& /*value*/, size_t offset, size_t line, size_t column) noexcept
{
    std::string const lexme{mTokenBuffer.str()};
    // TODO(lthomas): Fix polymorphism. Vector of token pointers? std::variant?
    mTokens.emplace_back(TokenType::IDENT, lexme, std::make_pair(offset, lexme.length()), std::make_pair(line, column));
    mTokenBuffer.str("");
    mTokenBuffer.clear();
}

void Lexer::createTypeToken(const std::string& /*value*/, size_t offset, size_t line, size_t column) noexcept
{
    std::string const lexme{mTokenBuffer.str()};
    // TODO(lthomas): Fix polymorphism. Vector of token pointers? std::variant?
    mTokens.emplace_back(
        TokenType::TYPE_IDENT, lexme, std::make_pair(offset, lexme.length()), std::make_pair(line, column));
    mTokenBuffer.str("");
    mTokenBuffer.clear();
}

void Lexer::parseIdent(size_t offset, size_t line, size_t column)
{
    if (auto next = mFileBuffer.peek()) {
        while (next.has_value() && isIdentChar(next.value())) {
            auto consumed = mFileBuffer.consume();
            if (!consumed.has_value()) {
                auto& logger = utils::getLogger();
                logger.error("Unexpected EOF in \"{}\"", __FUNCTION__);
                exit(EXIT_FAILURE);
            }
            mTokenBuffer << consumed.value();
            if (mTokenBuffer.str().length() + 1 > MAX_TOKEN_LEN) {
                pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                        line + 1,
                                        column,
                                        std::format("Max token length of {} characters exceeded.", MAX_TOKEN_LEN)});
            }
            next = mFileBuffer.peek();
        }

        next = mFileBuffer.peek();
        if (next.has_value()) {
            if (std::isspace(next.value()) == 0) {
                pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                        mFileBuffer.getCurrentLine() + 1,
                                        mFileBuffer.getCurrentColumn(),
                                        std::format("Unexpected character: '{}'.", next.value())});
            }
        }

        createIdentToken(mTokenBuffer.str(), offset, line, column);
    }
}

void Lexer::parseType(size_t offset, size_t line, size_t column)
{
    if (auto next = mFileBuffer.peek()) {
        while (next.has_value() && isTypeChar(next.value())) {
            auto consumed = mFileBuffer.consume();
            if (!consumed.has_value()) {
                auto& logger = utils::getLogger();
                logger.error("Unexpected EOF in \"{}\"", __FUNCTION__);
                exit(EXIT_FAILURE);
            }
            mTokenBuffer << consumed.value();
            if (mTokenBuffer.str().length() + 1 > MAX_TOKEN_LEN) {
                pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                        line + 1,
                                        column,
                                        std::format("Max token length of {} characters exceeded.", MAX_TOKEN_LEN)});
            }
            next = mFileBuffer.peek();
        }

        next = mFileBuffer.peek();
        if (next.has_value()) {
            if (std::isspace(next.value()) == 0) {
                pimento::errors::raise({pimento::errors::ErrorType::INVALID_TOKEN_ERROR,
                                        mFileBuffer.getCurrentLine() + 1,
                                        mFileBuffer.getCurrentColumn(),
                                        std::format("Unexpected character: '{}'.", next.value())});
            }
        }

        createTypeToken(mTokenBuffer.str(), offset, line, column);
    }
}

void Lexer::parseNumericConst(size_t /*offset*/, size_t /*line*/, size_t /*column*/)
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
