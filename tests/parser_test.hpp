#pragma once

#define TEST_IDENTIFIER_TOKEN(IDENTIFIER_TOKEN, IDENTIFIER)                                                        \
    (__extension__({                                                                                               \
        __typeof__(IDENTIFIER_TOKEN) __identifierToken = (IDENTIFIER_TOKEN);                                       \
        __typeof__(IDENTIFIER) __identifier            = (IDENTIFIER);                                             \
        EXPECT_EQ(__identifierToken.tokenType, pimento::tokenization::TokenType::IDENTIFIER);                      \
        ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IdentProperties>(__identifierToken.properties)); \
        EXPECT_EQ(std::get<pimento::tokenization::IdentProperties>(__identifierToken.properties).identifier,       \
                  __identifier);                                                                                   \
    }))

#define TEST_IDENTIFIER_TERM(IDENTIFIER_TERM, IDENTIFIER)                                                              \
    (__extension__({                                                                                                   \
        __typeof__(IDENTIFIER_TERM) __identifierTerm = (IDENTIFIER_TERM);                                              \
        __typeof__(IDENTIFIER) __identifier          = (IDENTIFIER);                                                   \
        ASSERT_TRUE(std::holds_alternative<std::shared_ptr<pimento::ast::node::TermIdentNode>>(__identifierTerm));     \
        EXPECT_EQ(                                                                                                     \
            std::get<std::shared_ptr<pimento::ast::node::TermIdentNode>>(__identifierTerm)->identifierToken.tokenType, \
            pimento::tokenization::TokenType::IDENTIFIER);                                                             \
        ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IdentProperties>(                                    \
            std::get<std::shared_ptr<pimento::ast::node::TermIdentNode>>(__identifierTerm)                             \
                ->identifierToken.properties));                                                                        \
        EXPECT_EQ(std::get<pimento::tokenization::IdentProperties>(                                                    \
                      std::get<std::shared_ptr<pimento::ast::node::TermIdentNode>>(__identifierTerm)                   \
                          ->identifierToken.properties)                                                                \
                      .identifier,                                                                                     \
                  __identifier);                                                                                       \
    }))

#define TEST_INT_LIT_TERM(INT_LIT_TERM, INT_LIT)                                                                      \
    (__extension__({                                                                                                  \
        __typeof__(INT_LIT_TERM) __intLitTerm = (INT_LIT_TERM);                                                       \
        __typeof__(INT_LIT) __intLit          = (INT_LIT);                                                            \
        ASSERT_TRUE(std::holds_alternative<std::shared_ptr<pimento::ast::node::TermIntLitNode>>(__intLitTerm));       \
        EXPECT_EQ(std::get<std::shared_ptr<pimento::ast::node::TermIntLitNode>>(__intLitTerm)->intLitToken.tokenType, \
                  pimento::tokenization::TokenType::INT_LITERAL);                                                     \
        ASSERT_TRUE(std::holds_alternative<pimento::tokenization::IntLitProperties>(                                  \
            std::get<std::shared_ptr<pimento::ast::node::TermIntLitNode>>(__intLitTerm)->intLitToken.properties));    \
        EXPECT_EQ(                                                                                                    \
            std::get<pimento::tokenization::IntLitProperties>(                                                        \
                std::get<std::shared_ptr<pimento::ast::node::TermIntLitNode>>(__intLitTerm)->intLitToken.properties)  \
                .value,                                                                                               \
            __intLit);                                                                                                \
    }))
