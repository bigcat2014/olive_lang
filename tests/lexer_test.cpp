/// @file lexer_test.cpp
/// @brief Pimento lexer unit tests
/// @author Logan Thomas

#include <sstream>
#include <string>
#include <string_view>

#include "lexer_test.hpp"
#include "test_settings.hpp"

#include <gtest/gtest.h>
#include <pimento/file_stream.hpp>
#include <pimento/lexer.hpp>
#include <pimento/tokens.hpp>

using namespace pimento::tokenization;

class TestingFile
{
public:
    explicit TestingFile(const std::string_view& filePath)
        : mStream(makeInputStreamFromPath(filePath))
    {
        if (!mStream) {
            throw std::runtime_error("cannot open " + std::string(filePath));
        }
    }

    virtual ~TestingFile() = default;

    [[nodiscard]] std::string getToken() const noexcept { return mLine; }

private:
    InputStream mStream;
    std::string mLine;
};

class LexerFixture : public ::testing::TestWithParam<std::pair<std::string_view, std::function<bool()>>>
{
protected:
    std::vector<Token> mTokens;

    void initializeFromFile(const std::string_view& filePath)
    {
        InputStream stream = makeInputStreamFromPath(filePath);
        Lexer lexer(stream.getStream());
        mTokens = lexer.tokens();
    }
};

std::vector<std::string> parseCSVRow(std::istringstream& input)
{
    std::vector<std::string> parsed;
    std::string value;
    while (std::getline(input, value, ',')) {
        parsed.emplace_back(value);
    }

    return parsed;
}

TEST_F(LexerFixture, FloatPass)
{
    initializeFromFile(FLOAT_PASS_PATH);

    for (const auto& token : mTokens) {
        ASSERT_EQ(token.tokenType, TokenType::TT_NUMERIC_CONST);
        // TODO(lthomas): Placeholder code. Will need updating for token type
        EXPECT_EQ(token.value, std::stof(token));
    }
}

TEST(Lexer, IntegerPass)
{
    initializeFromFile(INTEGER_PASS_PATH);

    for (const auto& token : mTokens) {
        ASSERT_EQ(token.tokenType, TokenType::TT_NUMERIC_CONST);
        // TODO(lthomas): Placeholder code. Will need updating for token type
        EXPECT_EQ(token.value, std::stoll(token));
    }
}

TEST(Lexer, ScientificPass)
{
    initializeFromFile(SCIENTIFIC_PASS_PATH);

    for (const auto& token : mTokens) {
        ASSERT_EQ(token.tokenType, TokenType::TT_NUMERIC_CONST);
        // TODO(lthomas): Placeholder code. Will need updating for token type
        EXPECT_EQ(token.value, std::stoll(parsed[1]));
        EXPECT_EQ(token.value, std::stoll(parsed[2]));
    }
}

TEST(Lexer, FloatFail)
{
    TestingFile testingFile{FLOAT_FAIL_PATH};

    while (testingFile.advance()) {
        std::istringstream testStream{testingFile.getToken()};
        Lexer lexer{testStream};

        for (const auto& token : lexer.tokens()) {
            EXPECT_NE(token.tokenType, TokenType::TT_NUMERIC_CONST);
        }
    }
}

TEST(Lexer, IntegerFail)
{
    TestingFile testingFile{INTEGER_FAIL_PATH};

    while (testingFile.advance()) {
        std::istringstream testStream{testingFile.getToken()};
        Lexer lexer{testStream};

        for (const auto& token : lexer.tokens()) {
            // TODO(lthomas): Will need updating for token type
            EXPECT_NE(token.tokenType, TokenType::TT_NUMERIC_CONST);
        }
    }
}

TEST(Lexer, ScientificFail)
{
    TestingFile testingFile{SCIENTIFIC_FAIL_PATH};

    while (testingFile.advance()) {
        std::istringstream testStream{testingFile.getToken()};
        Lexer lexer{testStream};

        for (const auto& token : lexer.tokens()) {
            // TODO(lthomas): Will need updating for token type
            EXPECT_NE(token.tokenType, TokenType::TT_NUMERIC_CONST);
        }
    }
}

TEST(Lexer, EmptyStream)
{
    std::istringstream iss;
    Lexer lexer(iss);

    EXPECT_TRUE(lexer.tokens().empty());
}

TEST(Lexer, ELSE)
{
    std::istringstream iss{"else"};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_ELSE);
}

TEST(Lexer, EXIT)
{
    std::istringstream iss{"exit"};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_EXIT);
}

TEST(Lexer, WHILE)
{
    std::istringstream iss{"while"};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_WHILE);
}

TEST(Lexer, IF)
{
    std::istringstream iss{"if"};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_IF);
}

TEST(Lexer, ELIF)
{
    std::istringstream iss{"elif"};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_ELIF);
}

// TEST(Lexer, LET) {
//   std::istringstream iss{"let"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_LET);
// }

TEST(Lexer, LEFT_CURLY)
{
    std::istringstream iss{"{"};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_LEFT_CURLY);
}

TEST(Lexer, LEFT_PAREN)
{
    std::istringstream iss{"("};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_LEFT_PAREN);
}

TEST(Lexer, RIGHT_CURLY)
{
    std::istringstream iss{"}"};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_RIGHT_CURLY);
}

TEST(Lexer, RIGHT_PAREN)
{
    std::istringstream iss{")"};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_RIGHT_PAREN);
}

// TEST(Lexer, DOUBLE_CARET) {
//   std::istringstream iss{"^^"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_DOUBLE_CARET, 3,
//                        BinOpProperties::Associativity::RIGHT);
// }

// TEST(Lexer, PERCENT) {
//   std::istringstream iss{"%"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_PERCENT, 2,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, STAR) {
//   std::istringstream iss{"*"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_STAR, 2,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, FORWARD_SLASH) {
//   std::istringstream iss{"/"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_FORWARD_SLASH, 2,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, MINUS) {
//   std::istringstream iss{"-"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_MINUS, 1,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, PLUS) {
//   std::istringstream iss{"+"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_PLUS, 1,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, LT) {
//   std::istringstream iss{"<"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_LT, 0,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, GT) {
//   std::istringstream iss{">"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::TT_GT, 0,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, EQUAL) {
//   std::istringstream iss{"="};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_EQUAL);
// }

TEST(Lexer, SEMI)
{
    std::istringstream iss{";"};
    Lexer lexer(iss);

    ASSERT_EQ(lexer.tokens().size(), 1);
    // TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::TT_SEMI);
}

// TEST(Lexer, INT_LITERAL) {
//   std::istringstream iss{"123456"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_INT_LIT_TOKEN(lexer.tokens()[0], std::stoull(iss.str()));
// }

// TEST(Lexer, IDENTIFIER) {
//   std::istringstream iss{"qwerty"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_IDENTIFIER_TOKEN(lexer.tokens()[0], iss.str());
// }

TEST(Lexer, MAX_TOKEN_LENGTH)
{
    std::istringstream iss{"MaxTokenLengthExceededMaxTokenLengthExceededMaxTokenLengthExceeded"};
    EXPECT_EXIT(
        pimento::tokenization::Lexer lexer(iss), testing::ExitedWithCode(EXIT_FAILURE), "Max token length .*exceeded.");
}