//! @file lexer_test.cpp
//! @brief Pimento lexer unit tests
//! @author Logan Thomas

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>

#include "lexer_test.hpp"
#include "test_settings.hpp"

#include <gtest/gtest.h>
#include <pimento/lexer.hpp>
#include <pimento/tokens.hpp>

using namespace pimento::tokenization;

class TestingFile {
public:
  TestingFile(const std::string_view &file_path_sv):
    m_input_file(std::string(file_path_sv))
  {
    if (!m_input_file) {
      throw std::runtime_error("cannot open " + std::string(file_path_sv));
    }
  }

  virtual ~TestingFile(){}

  bool advance() {
    m_token.clear();
    // TODO(lthomas): No conversion to bool if I directly return the result, but this works? Goofy.
    if (std::getline(m_input_file, m_token)) {
      return true;
    }
    return false;
  }

  std::string get_token() { return m_token; }

protected:
  std::ifstream m_input_file;
  std::string m_token;
};

std::vector<std::string> parse_csv_row(std::istringstream &input) {
  std::vector<std::string> parsed;
  std::string value;
  while (std::getline(input, value, ',')) {
    parsed.push_back(std::move(value));
  }

  return parsed;
}

TEST(Lexer, FloatPass) {
  TestingFile testing_file{FLOAT_PASS_PATH};

  while(testing_file.advance()) {
    std::istringstream test_stream{std::move(testing_file.get_token())};
    std::vector<std::string> parsed = parse_csv_row(test_stream);
    ASSERT_GE(parsed.size(), 2);

    std::istringstream input{std::move(parsed[0])};
    Lexer lexer{input};
    for (const auto &token : lexer.tokens()) {
      EXPECT_EQ(token.token_type, TokenType::NUMERIC_CONST);
      // TODO(lthomas): Placeholder code. Will need updating for token type
      EXPECT_EQ(token.value, std::stod(parsed[1]));
    }
  }
}

TEST(Lexer, IntegerPass) {
  TestingFile testing_file{INTEGER_PASS_PATH};

  while(testing_file.advance()) {
    std::istringstream test_stream{std::move(testing_file.get_token())};
    std::vector<std::string> parsed = parse_csv_row(test_stream);
    ASSERT_GE(parsed.size(), 2);

    std::istringstream input{std::move(parsed[0])};
    Lexer lexer{input};
    for (const auto &token : lexer.tokens()) {
      // TODO(lthomas): Placeholder code. Will need updating for token type
      EXPECT_EQ(token.token_type, TokenType::NUMERIC_CONST);
      EXPECT_EQ(token.value, std::stoll(parsed[1]));
    }
  }
}

TEST(Lexer, ScientificPass) {
  TestingFile testing_file{SCIENTIFIC_PASS_PATH};

  while(testing_file.advance()) {
    std::istringstream test_stream{std::move(testing_file.get_token())};
    std::vector<std::string> parsed = parse_csv_row(test_stream);
    ASSERT_GE(parsed.size(), 3);

    std::istringstream input{std::move(parsed[0])};
    Lexer lexer{input};
    for (const auto &token : lexer.tokens()) {
      // TODO(lthomas): Placeholder code. Will need updating for token type
      EXPECT_EQ(token.token_type, TokenType::NUMERIC_CONST);
      EXPECT_EQ(token.value, std::stoll(parsed[1]));
      EXPECT_EQ(token.value, std::stoll(parsed[2]));
    }
  }
}

TEST(Lexer, FloatFail) {
  TestingFile testing_file{FLOAT_FAIL_PATH};

  while(testing_file.advance()) {
    std::istringstream token_stream{testing_file.get_token()};
    Lexer lexer{token_stream};

    for (const auto &token : lexer.tokens()) {
      EXPECT_NE(token.token_type, TokenType::NUMERIC_CONST);
    }
  }
}

TEST(Lexer, IntegerFail) {
  TestingFile testing_file{INTEGER_FAIL_PATH};

  while(testing_file.advance()) {
    std::istringstream token_stream{testing_file.get_token()};
    Lexer lexer{token_stream};

    for (const auto &token : lexer.tokens()) {
      // TODO(lthomas): Will need updating for token type
      EXPECT_NE(token.token_type, TokenType::NUMERIC_CONST);
    }
  }
}

TEST(Lexer, ScientificFail) {
  TestingFile testing_file{SCIENTIFIC_FAIL_PATH};

  while(testing_file.advance()) {
    std::istringstream token_stream{testing_file.get_token()};
    Lexer lexer{token_stream};

    for (const auto &token : lexer.tokens()) {
      // TODO(lthomas): Will need updating for token type
      EXPECT_NE(token.token_type, TokenType::NUMERIC_CONST);
    }
  }
}

TEST(Lexer, EmptyStream) {
  std::istringstream iss;
  Lexer lexer(iss);

  EXPECT_TRUE(lexer.tokens().empty());
}

TEST(Lexer, ELSE) {
  std::istringstream iss{"else"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::ELSE);
}

TEST(Lexer, EXIT) {
  std::istringstream iss{"exit"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::EXIT);
}

TEST(Lexer, WHILE) {
  std::istringstream iss{"while"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::WHILE);
}

TEST(Lexer, IF) {
  std::istringstream iss{"if"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::IF);
}

TEST(Lexer, ELIF) {
  std::istringstream iss{"elif"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::ELIF);
}

// TEST(Lexer, LET) {
//   std::istringstream iss{"let"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::LET);
// }

TEST(Lexer, LEFT_CURLY) {
  std::istringstream iss{"{"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::LEFT_CURLY);
}

TEST(Lexer, LEFT_PAREN) {
  std::istringstream iss{"("};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::LEFT_PAREN);
}

TEST(Lexer, RIGHT_CURLY) {
  std::istringstream iss{"}"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::RIGHT_CURLY);
}

TEST(Lexer, RIGHT_PAREN) {
  std::istringstream iss{")"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::RIGHT_PAREN);
}

// TEST(Lexer, DOUBLE_CARET) {
//   std::istringstream iss{"^^"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::DOUBLE_CARET, 3,
//                        BinOpProperties::Associativity::RIGHT);
// }

// TEST(Lexer, PERCENT) {
//   std::istringstream iss{"%"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::PERCENT, 2,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, STAR) {
//   std::istringstream iss{"*"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::STAR, 2,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, FORWARD_SLASH) {
//   std::istringstream iss{"/"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::FORWARD_SLASH, 2,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, MINUS) {
//   std::istringstream iss{"-"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::MINUS, 1,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, PLUS) {
//   std::istringstream iss{"+"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::PLUS, 1,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, LT) {
//   std::istringstream iss{"<"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::LT, 0,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, GT) {
//   std::istringstream iss{">"};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_BINARY_OP_TOKEN(lexer.tokens()[0], TokenType::GT, 0,
//                        BinOpProperties::Associativity::LEFT);
// }

// TEST(Lexer, EQUAL) {
//   std::istringstream iss{"="};
//   Lexer lexer(iss);

//   ASSERT_EQ(lexer.tokens().size(), 1);
//   TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::EQUAL);
// }

TEST(Lexer, SEMI) {
  std::istringstream iss{";"};
  Lexer lexer(iss);

  ASSERT_EQ(lexer.tokens().size(), 1);
  TEST_MONOSTATE_TOKEN(lexer.tokens()[0], TokenType::SEMI);
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

TEST(Lexer, MAX_TOKEN_LENGTH) {
  std::istringstream iss{
      "MaxTokenLengthExceededMaxTokenLengthExceededMaxTokenLengthExceeded"};
  EXPECT_EXIT(pimento::tokenization::Lexer lexer(iss),
              testing::ExitedWithCode(EXIT_FAILURE),
              "Max token length .*exceeded.");
}