#pragma once

#define TEST_MONOSTATE_TOKEN(TOKEN, TOKEN_TYPE)                                \
  (__extension__({                                                             \
    __typeof__(TOKEN) __token = (TOKEN);                                       \
    __typeof__(TOKEN_TYPE) __token_type = (TOKEN_TYPE);                        \
    EXPECT_EQ(__token.token_type, __token_type);                               \
    EXPECT_TRUE(std::holds_alternative<std::monostate>(__token.properties));   \
  }))

#define TEST_BINARY_OP_TOKEN(TOKEN, TOKEN_TYPE, PRECEDENCE, ASSOCIATIVITY)     \
  (__extension__({                                                             \
    __typeof__(TOKEN) __token = (TOKEN);                                       \
    __typeof__(TOKEN_TYPE) __token_type = (TOKEN_TYPE);                        \
    __typeof__(PRECEDENCE) __precedence = (PRECEDENCE);                        \
    __typeof__(ASSOCIATIVITY) __associativity = (ASSOCIATIVITY);               \
    EXPECT_EQ(__token.token_type, __token_type);                               \
    ASSERT_TRUE(                                                               \
        std::holds_alternative<pimento::tokenization::BinOpProperties>(        \
            __token.properties));                                              \
    EXPECT_EQ(                                                                 \
        std::get<pimento::tokenization::BinOpProperties>(__token.properties)   \
            .associativity,                                                    \
        __associativity);                                                      \
    EXPECT_EQ(                                                                 \
        std::get<pimento::tokenization::BinOpProperties>(__token.properties)   \
            .precedence,                                                       \
        __precedence);                                                         \
  }))

#define TEST_IDENTIFIER_TOKEN(IDENTIFIER_TOKEN, IDENTIFIER)                    \
  (__extension__({                                                             \
    __typeof__(IDENTIFIER_TOKEN) __identifier_token = (IDENTIFIER_TOKEN);      \
    __typeof__(IDENTIFIER) __identifier = (IDENTIFIER);                        \
    EXPECT_EQ(__identifier_token.token_type,                                   \
              pimento::tokenization::TokenType::IDENTIFIER);                \
    ASSERT_TRUE(                                                               \
        std::holds_alternative<pimento::tokenization::IdentProperties>(        \
            __identifier_token.properties));                                   \
    EXPECT_EQ(std::get<pimento::tokenization::IdentProperties>(                \
                  __identifier_token.properties)                               \
                  .identifier,                                                 \
              __identifier);                                                   \
  }))

#define TEST_INT_LIT_TOKEN(INT_LIT_TOKEN, INT_LIT)                             \
  (__extension__({                                                             \
    __typeof__(INT_LIT_TOKEN) __int_lit_token = (INT_LIT_TOKEN);               \
    __typeof__(INT_LIT) __int_lit = (INT_LIT);                                 \
    EXPECT_EQ(__int_lit_token.token_type,                                      \
              pimento::tokenization::TokenType::INT_LITERAL);               \
    ASSERT_TRUE(                                                               \
        std::holds_alternative<pimento::tokenization::IntLitProperties>(       \
            __int_lit_token.properties));                                      \
    EXPECT_EQ(std::get<pimento::tokenization::IntLitProperties>(               \
                  __int_lit_token.properties)                                  \
                  .value,                                                      \
              __int_lit);                                                      \
  }))
