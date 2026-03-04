/// @file generator.hpp
/// @brief Pimento Assembly generator.
/// @author Logan Thomas

#pragma once

#include <istream>
#include <memory>
#include <ostream>
#include <string>

#include <pimento/ast.hpp>
#include <pimento/parser.hpp>

namespace pimento::generation {

/// @brief Class responsible for generating asembly from the AST produced by a parser.
class Generator
{
public:
    /// @brief Constructor for the Generator.
    /// @param istream Input stream of characters to lex, parse, and generate.
    /// @param ostream Output stream to generate to.
    explicit Generator(std::istream* istream, std::ostream* ostream);

    /// @brief Lex, parse, and generate output.
    void generate() noexcept;

private:
    /// @brief Generate assembly for a Statement.
    /// @param node Input statement node for which to generate assembly.
    void genStatement(const std::shared_ptr<ast::node::StmtNode>& node) noexcept;

    /// @brief Generate assembly for an Expression.
    /// @param node Input expression node for which to generate assembly.
    void genExpression(const std::shared_ptr<ast::node::ExprNode>& node) noexcept;

    /// @brief Generate assembly for a Scope.
    /// @param node Input scope node for which to generate assembly.
    void genScope(const std::shared_ptr<ast::node::ScopeNode>& node) noexcept;

    /// @brief Generate assembly for an If Predicate.
    /// @param node Input if predicate node for which to generate assembly.
    /// @param endLabel The name of the label to jump to in order to skip the ifpred body when the condition is false.
    void genIfpred(const std::shared_ptr<ast::node::IfPredNode>& node, const std::string& endLabel) noexcept;

    /// @brief Generate assembly for a Term.
    /// @param node Input term node for which to generate assembly.
    void genTerm(const std::shared_ptr<ast::node::TermNode>& node) noexcept;

    /// @brief Generate assembly for a Binary Expression.
    /// @param node Input binary expression node for which to generate assembly.
    void genBinExpr(const std::shared_ptr<ast::node::BinExprNode>& node) noexcept;

    /// @brief Helper function to push a value in a registry onto the stack.
    /// @param reg The registry to push onto the stack.
    void push(const std::string& reg) noexcept;

    /// @brief Helper function to pop a value from the stack into a registry.
    /// @param reg The registry to pop the value into.
    void pop(const std::string& reg) noexcept;

    /// @brief Helper function to manage variables in the stack when entering a scope.
    void beginScope() noexcept;

    /// @brief Helper function to manage variables in the stack when exiting a scope.
    void endScope() noexcept;

    /// @brief Helper function to create unique assembly labels.
    /// @return The new label.
    static std::string createLabel() noexcept;

private:
    /// @brief Helper struct for maintaining variables on the stack.
    struct Var
    {
        /// @brief The identifier of the variable.
        std::string name;
        /// @brief The location in the stack of this variable.
        size_t stackLoc;

        /// @brief Constructor for the Var.
        /// @param name Identifier of the variable.
        /// @param stackLoc Stack location of the variable.
        Var(std::string name, size_t stackLoc);
    };

private:
    /// @brief Parser object for parsing tokens into AST.
    ast::Parser mParser;
    /// @brief Output stream.
    std::ostream* mOutput;
    /// @brief The current stack size.
    size_t mStackSize{0};
    /// @brief Current variables in use.
    std::vector<Var> mVars;
    /// @brief Current scopes in use.
    std::vector<size_t> mScopes;
};

}  // namespace pimento::generation