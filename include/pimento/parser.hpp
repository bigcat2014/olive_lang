//! @file parser.hpp
//! @brief Pimento parser.
//! @author Logan Thomas

#pragma once

#include <istream>
#include <optional>
#include <vector>

#include <pimento/ast.hpp>
#include <pimento/lexer.hpp>
#include <pimento/tokens.hpp>

namespace pimento::ast {

//! @brief Token parser.
class Parser {
public:
  //! @brief Construct a new Parser object.
  //! @param istream std::istream& The stream of characters to tokenize and
  //! parse parse.
  explicit Parser(std::istream &istream);

  //! @brief Getter for the top level program AST node.
  //! @return const node::ProgNode& The top level program AST node.
  [[nodiscard]] const node::ProgNode &get_program() const noexcept;

private:
  //! @brief Parse all tokens.
  void parse();

  //! @brief Peek at a token at an offset from the current token in the buffer.
  //! @param lookahead size_t Optional lookahead distance to peek.
  //! @return std::optional<tokenization::Token> The token at `lookahead` offset
  //! from the current index or {} if attempting to peek out of bounds.
  [[nodiscard]] inline std::optional<tokenization::Token>
  peek(size_t lookahead = 0) const noexcept;

  //! @brief Consume the current token.
  inline void consume() noexcept;

  //! @brief Attempt to consume the next token.
  //! @return std::optional<tokenization::Token> The token that was consumed.
  [[nodiscard]] inline std::optional<tokenization::Token>
  try_consume() noexcept;

  //! @brief Try to consume the next token if it is the provided TokenType.
  //!
  //! Try to consume the next token if it is a specific TokenType. If the token
  //! type matches, consume it, otherwise log an error and exit.
  //! @param token_type tokenization::TokenType The token type we are asserting
  //! is next.
  inline tokenization::Token
  try_consume(tokenization::TokenType token_type) noexcept;

  //! @brief Parse a Statement into the AST.
  //! @return std::shared_ptr<node::StmtNode> The Statement node of the AST.
  std::shared_ptr<node::StmtNode> parse_statement();

  //! @brief Parse an Expression into the AST.
  //! @return std::shared_ptr<node::ExprNode> The Expression node of the AST.
  std::shared_ptr<node::ExprNode>
  parse_expression(uint8_t min_precedence = 0);

  //! @brief Parse a Scope into the AST.
  //! @return std::shared_ptr<node::ScopeNode> The Scope node of the AST.
  std::shared_ptr<node::ScopeNode> parse_scope();

  //! @brief Parse an If Predicate into the AST.
  //! @return std::optional<std::shared_ptr<node::IfPred>> The If Predicate node
  //! of the AST.
  std::optional<std::shared_ptr<node::IfPredNode>> parse_ifpred();

  //! @brief Parse a Term into the AST.
  //! @return std::shared_ptr<node::TermNode> The Term node of the AST.
  std::shared_ptr<node::TermNode> parse_term();

private:
  //! @brief The lexer used by this parser.
  const tokenization::Lexer m_lexer;
  //! @brief The current parsing index.
  size_t m_index{0};
  //! @brief The root node of the ast.
  node::ProgNode m_prog;
};

} // namespace pimento::ast