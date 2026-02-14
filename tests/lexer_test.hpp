#pragma once

#define TEST_MONOSTATE_TOKEN(TOKEN, TOKEN_TYPE)                                  \
    (__extension__({                                                             \
        __typeof__(TOKEN) __token          = (TOKEN);                            \
        __typeof__(TOKEN_TYPE) __tokenType = (TOKEN_TYPE);                       \
        EXPECT_EQ(__token.tokenType, __tokenType);                               \
        EXPECT_TRUE(std::holds_alternative<std::monostate>(__token.properties)); \
    }))

#define TEST_BINARY_OP_TOKEN(TOKEN, TOKEN_TYPE, PRECEDENCE, ASSOCIATIVITY)                                        \
    (__extension__({                                                                                              \
        __typeof__(TOKEN) __token                 = (TOKEN);                                                      \
        __typeof__(TOKEN_TYPE) __tokenType        = (TOKEN_TYPE);                                                 \
        __typeof__(PRECEDENCE) __precedence       = (PRECEDENCE);                                                 \
        __typeof__(ASSOCIATIVITY) __associativity = (ASSOCIATIVITY);                                              \
        EXPECT_EQ(__token.tokenType, __tokenType);                                                                \
        ASSERT_TRUE(std::holds_alternative<pimento::tokenization::BinOpProperties>(__token.properties));          \
        EXPECT_EQ(std::get<pimento::tokenization::BinOpProperties>(__token.properties).associativity,             \
                  __associativity);                                                                               \
        EXPECT_EQ(std::get<pimento::tokenization::BinOpProperties>(__token.properties).precedence, __precedence); \
    }))

#define TEST_IDENTIFIER_TOKEN(IDENTIFIER_TOKEN, IDENTIFIER)                                                        \
    (__extension__({                                                                                               \
        __typeof__(IDENTIFIER_TOKEN) __identifierToken = (IDENTIFIER_TOKEN);                                       \
        __typeof__(IDENTIFIER) __identifier            = (IDENTIFIER);                                             \
        EXPECT_EQ(__identifierToken.tokenType, pimento::tokenization::TokenType::TT_IDENTIFIER);                   \
        ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IdentProperties>(__identifierToken.properties)); \
        EXPECT_EQ(std::get<pimento::tokenization::IdentProperties>(__identifierToken.properties).identifier,       \
                  __identifier);                                                                                   \
    }))

#define TEST_INT_LIT_TOKEN(INT_LIT_TOKEN, INT_LIT)                                                              \
    (__extension__({                                                                                            \
        __typeof__(INT_LIT_TOKEN) __intLitToken = (INT_LIT_TOKEN);                                              \
        __typeof__(INT_LIT) __intLit            = (INT_LIT);                                                    \
        EXPECT_EQ(__intLitToken.tokenType, pimento::tokenization::TokenType::TT_INT_LITERAL);                   \
        ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IntLitProperties>(__intLitToken.properties)); \
        EXPECT_EQ(std::get<pimento::tokenization::IntLitProperties>(__intLitToken.properties).value, __intLit); \
    }))
