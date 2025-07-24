//! @file generator.hpp
//! @brief Pimento Assembly generator.
//! @author Logan Thomas

#pragma once

#include <istream>
#include <memory>
#include <ostream>
#include <string>

#include <pimento/ast.hpp>
#include <pimento/parser.hpp>

namespace pimento::generation {

class Generator {
public:
  //! @brief Constructor for the Generator.
  //! @param istream std::istream& Input stream of characters to
  //! lex, parse, and generate.
  //! @param ostream std::ostream& Output stream to generate to.
  explicit Generator(std::istream &istream, std::ostream &ostream);

  //! @brief Lex, parse, and generate output.
  void generate() noexcept;

private:
  //! @brief Generate assembly for a Statement.
  //! @param node std::shared_ptr<ast::node::StmtNode> Input statement node for
  //! which to generate assembly.
  void gen_statement(const std::shared_ptr<ast::node::StmtNode> &node) noexcept;

  //! @brief Generate assembly for an Expression.
  //! @param node std::shared_ptr<ast::node::ExprNode> Input expression node for
  //! which to generate assembly.
  void
  gen_expression(const std::shared_ptr<ast::node::ExprNode> &node) noexcept;

  //! @brief Generate assembly for a Scope.
  //! @param node std::shared_ptr<ast::node::Scope> Input scope node for which
  //! to generate assembly.
  void gen_scope(const std::shared_ptr<ast::node::ScopeNode> &node) noexcept;

  //! @brief Generate assembly for an If Predicate.
  //! @param node std::shared_ptr<ast::node::IfPredNode> Input if predicate node
  //! for which to generate assembly.
  void gen_ifpred(const std::shared_ptr<ast::node::IfPredNode> &node,
                  const std::string &end_label) noexcept;

  //! @brief Generate assembly for a Term.
  //! @param node std::shared_ptr<ast::node::TermNode> Input term node for
  //! which to generate assembly.
  void gen_term(const std::shared_ptr<ast::node::TermNode> &node) noexcept;

  //! @brief Generate assembly for a Binary Expression.
  //! @param node std::shared_ptr<ast::node::BinExprNode> Input binary
  //! expression node for which to generate assembly.
  void
  gen_bin_expr(const std::shared_ptr<ast::node::BinExprNode> &node) noexcept;

  //! @brief Helper function to push a value in a registry onto the stack.
  //! @param reg const std::string& The registry to push onto the stack.
  void push(const std::string &reg) noexcept;

  //! @brief Helper function to pop a value from the stack into a registry.
  //! @param reg const std::string& The registry to pop the value into.
  void pop(const std::string &reg) noexcept;

  //! @brief Helper function to manage variables in the stack when entering a
  //! scope.
  void begin_scope() noexcept;

  //! @brief Helper function to manage variables in the stack when exiting a
  //! scope.
  void end_scope() noexcept;

  //! @brief Helper function to create unique assembly labels.
  //! @return std::string The new label
  static std::string create_label() noexcept;

private:
  //! @brief Helper struct for maintaining variables on the stack.
  struct Var {
    //! @brief The identifier of the variable.
    std::string name;
    //! @brief The location in the stack of this variable.
    size_t stack_loc;

    //! @brief Constructor for the Var.
    //! @param name const std::string& Identifier of the variable.
    //! @param stack_loc size_t Stack location of the variable
    Var(const std::string &name, size_t stack_loc);
  };

private:
  //! @brief Parser object for parsing tokens into AST.
  ast::Parser m_parser;
  //! @brief Output stream.
  std::ostream &m_output;
  //! @brief The current stack size.
  size_t m_stack_size{0};
  //! @brief Current variables in use.
  std::vector<Var> m_vars;
  //! @brief Current scopes in use.
  std::vector<size_t> m_scopes;
};

} // namespace pimento::generation