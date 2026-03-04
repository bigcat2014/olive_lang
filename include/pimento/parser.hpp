/// @file parser.hpp
/// @brief Pimento parser.
/// @author Logan Thomas

#pragma once

#include <istream>
#include <optional>

#include <pimento/ast.hpp>
#include <pimento/lexer.hpp>
#include <pimento/tokens.hpp>

namespace pimento::ast {

/// @brief Token parser.
class Parser
{
public:
    /// @brief Construct a new Parser object.
    /// @param istream The stream of characters to tokenize and parse.
    explicit Parser(std::istream* istream);

    /// @brief Getter for the top level program AST node.
    /// @return The top level program AST node.
    [[nodiscard]] const node::ProgNode& getProgram() const noexcept;

private:
    /// @brief Parse all tokens.
    void parse();

    /// @brief Peek at a token at an offset from the current token in the buffer.
    /// @param lookahead Optional lookahead distance to peek.
    /// @return The token at `lookahead` offset from the current index or std::nullopt if attempting to peek out of
    /// bounds.
    [[nodiscard]] inline std::optional<tokenization::Token> peek(size_t lookahead = 0) const noexcept;

    /// @brief Advance the parser.
    inline void advance() noexcept;

    /// @brief Attempt to consume the next token.
    /// @return The token that was consumed.
    [[nodiscard]] inline std::optional<tokenization::Token> tryConsume() noexcept;

    /// @brief Try to consume the next token if it is the provided TokenType.
    /// @details Try to consume the next token if it is a specific TokenType. If the token type matches, consume it,
    /// otherwise log an error and exit.
    /// @param tokenType The token type we are asserting is next.
    /// @return The token that has been consumed.
    inline tokenization::Token tryConsume(tokenization::TokenType tokenType) noexcept;

    /// @brief Parse a Statement into the AST.
    /// @return The Statement node of the AST.
    std::shared_ptr<node::StmtNode> parseStatement();

    /// @brief Parse an Expression into the AST.
    /// @param minPrecedence The minimum precedence used for determining operator precedence.
    /// @return The Expression node of the AST.
    std::shared_ptr<node::ExprNode> parseExpression(uint8_t minPrecedence = 0);

    /// @brief Parse a Scope into the AST.
    /// @return The Scope node of the AST.
    std::shared_ptr<node::ScopeNode> parseScope();

    /// @brief Parse an If Predicate into the AST.
    /// @return The If Predicate node of the AST.
    std::optional<std::shared_ptr<node::IfPredNode>> parseIfPred();

    /// @brief Parse a Term into the AST.
    /// @return The Term node of the AST.
    std::shared_ptr<node::TermNode> parseTerm();

private:
    /// @brief The lexer used by this parser.
    tokenization::Lexer mLexer;
    /// @brief The current parsing index.
    size_t mIndex{0};
    /// @brief The root node of the ast.
    node::ProgNode mProg;
};

}  // namespace pimento::ast