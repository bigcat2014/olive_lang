#pragma once

#define TEST_IDENTIFIER_TOKEN(IDENTIFIER_TOKEN, IDENTIFIER)                    \
  (__extension__({                                                             \
    __typeof__(IDENTIFIER_TOKEN) __identifier_token = (IDENTIFIER_TOKEN);      \
    __typeof__(IDENTIFIER) __identifier = (IDENTIFIER);                        \
    EXPECT_EQ(__identifier_token.token_type,                                   \
              pimento::tokenization::TokenType::IDENTIFIER);                   \
    ASSERT_TRUE(                                                               \
        std::holds_alternative<pimento::tokenization::IdentProperties>(        \
            __identifier_token.properties));                                   \
    EXPECT_EQ(std::get<pimento::tokenization::IdentProperties>(                \
                  __identifier_token.properties)                               \
                  .identifier,                                                 \
              __identifier);                                                   \
  }))

#define TEST_IDENTIFIER_TERM(IDENTIFIER_TERM, IDENTIFIER)                      \
  (__extension__({                                                             \
    __typeof__(IDENTIFIER_TERM) __identifier_term = (IDENTIFIER_TERM);         \
    __typeof__(IDENTIFIER) __identifier = (IDENTIFIER);                        \
    ASSERT_TRUE(std::holds_alternative<                                        \
                std::shared_ptr<pimento::ast::node::TermIdentNode>>(           \
        __identifier_term));                                                   \
    EXPECT_EQ(std::get<std::shared_ptr<pimento::ast::node::TermIdentNode>>(    \
                  __identifier_term)                                           \
                  ->identifier_token.token_type,                               \
              pimento::tokenization::TokenType::IDENTIFIER);                   \
    ASSERT_TRUE(                                                               \
        std::holds_alternative<pimento::tokenization::IdentProperties>(        \
            std::get<std::shared_ptr<pimento::ast::node::TermIdentNode>>(      \
                __identifier_term)                                             \
                ->identifier_token.properties));                               \
    EXPECT_EQ(                                                                 \
        std::get<pimento::tokenization::IdentProperties>(                      \
            std::get<std::shared_ptr<pimento::ast::node::TermIdentNode>>(      \
                __identifier_term)                                             \
                ->identifier_token.properties)                                 \
            .identifier,                                                       \
        __identifier);                                                         \
  }))

#define TEST_INT_LIT_TERM(INT_LIT_TERM, INT_LIT)                               \
  (__extension__({                                                             \
    __typeof__(INT_LIT_TERM) __int_lit_term = (INT_LIT_TERM);                  \
    __typeof__(INT_LIT) __int_lit = (INT_LIT);                                 \
    ASSERT_TRUE(std::holds_alternative<                                        \
                std::shared_ptr<pimento::ast::node::TermIntLitNode>>(          \
        __int_lit_term));                                                      \
    EXPECT_EQ(std::get<std::shared_ptr<pimento::ast::node::TermIntLitNode>>(   \
                  __int_lit_term)                                              \
                  ->int_lit_token.token_type,                                  \
              pimento::tokenization::TokenType::INT_LITERAL);                  \
    ASSERT_TRUE(                                                               \
        std::holds_alternative<pimento::tokenization::IntLitProperties>(       \
            std::get<std::shared_ptr<pimento::ast::node::TermIntLitNode>>(     \
                __int_lit_term)                                                \
                ->int_lit_token.properties));                                  \
    EXPECT_EQ(                                                                 \
        std::get<pimento::tokenization::IntLitProperties>(                     \
            std::get<std::shared_ptr<pimento::ast::node::TermIntLitNode>>(     \
                __int_lit_term)                                                \
                ->int_lit_token.properties)                                    \
            .value,                                                            \
        __int_lit);                                                            \
  }))
