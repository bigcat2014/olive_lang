//! @file generator.hpp
//! @brief Pimento ASM generator
//! @author Logan Thomas

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <pimento/ast.hpp>
#include <variant>

namespace pimento::generation {

class Generator {
public:
  explicit Generator(const ast::node::ProgNode &prog,
                     const std::filesystem::path &path)
      : m_prog(prog), m_output_path(path) {}

  void generate() noexcept {
    m_output << "global _start\n_start:\n";

    for (const auto statement : m_prog.statements) {
      gen_statement(statement);
    }

    std::cout << m_output.str() << std::endl;
  }

private:
  void gen_statement(const ast::node::StmtNode *const stmt) noexcept {
    struct Visitor {
      Generator &gen;

      void operator()(const ast::node::StmtExitNode *const stmt_exit) const {
        gen.m_output << "    ;; exit\n";
        gen.m_output << "    mov rax, 60\n";
        gen.gen_expression(stmt_exit->expression);
        // TODO(lthomas): Remove hard-coded exit value
        gen.m_output << "    mov rdi, 23\n";
        gen.m_output << "    syscall\n";
        gen.m_output << "    ;; /exit\n";
      }

      void operator()(const ast::node::StmtLetNode *const stmt_let) const {
        gen.m_output << "    ;; let\n";
        gen.m_output << "    ;; "
                     << tokenization::TokenTypeUtil::get_type_as_str(
                            stmt_let->identifier.token_type)
                     << "\n";
        gen.gen_expression(stmt_let->expression);
        gen.m_output << "    ;; /let\n";
      }

      void
      operator()(const ast::node::StmtAssignNode *const stmt_assign) const {
        gen.m_output << "    ;; assign";
        gen.m_output << "    ;; "
                     << tokenization::TokenTypeUtil::get_type_as_str(
                            stmt_assign->identifier.token_type)
                     << "\n";
        gen.gen_expression(stmt_assign->expression);
        gen.m_output << "    ;; /assign";
      }

      void operator()(const ast::node::ScopeNode *const scope) const {
        gen.m_output << "    ;; scope\n";
        gen.gen_scope(scope);
        gen.m_output << "    ;; /scope\n";
      }

      void operator()(const ast::node::StmtIfNode *const stmt_if) const {
        gen.m_output << "    ;; if\n";
        gen.gen_expression(stmt_if->expression);
        gen.gen_scope(stmt_if->scope);
        gen.m_output << "    ;; /if\n";
        if (stmt_if->ifpred.has_value()) {
          gen.gen_ifpred(stmt_if->ifpred.value());
        }
      }
    };

    Visitor visitor{.gen = *this};
    std::visit(visitor, stmt->node);
  }

  void gen_expression(const ast::node::ExprNode *const expr) noexcept {
    struct Visitor {
      Generator &gen;

      void operator()(const ast::node::TermNode *const term) const {
        gen.m_output << "    ;; term\n";
        gen.gen_term(term);
        gen.m_output << "    ;; /term\n";
      }

      void operator()(const ast::node::BinExprNode *const bin_expr) const {
        gen.m_output << "    ;; binexpr\n";
        gen.gen_bin_expr(bin_expr);
        gen.m_output << "    ;; /binexpr\n";
      }
    };

    Visitor visitor{.gen = *this};
    std::visit(visitor, expr->node);
  }

  void gen_scope(const ast::node::ScopeNode *const scope) noexcept {
    for (const auto stmt : scope->statements) {
      m_output << "    ;; scope\n";
      gen_statement(stmt);
      m_output << "    ;; /scope\n";
    }
  }

  void gen_ifpred(const ast::node::IfPredNode *const ifpred) noexcept {
    struct Visitor {
      Generator &gen;

      void
      operator()(const ast::node::IfPredElifNode *const ifpred_elif) const {
        gen.m_output << "    ;; elif\n";
        gen.gen_expression(ifpred_elif->expression);
        gen.gen_scope(ifpred_elif->scope);
        gen.m_output << "    ;; /elif\n";
        if (ifpred_elif->ifpred.has_value()) {
          gen.gen_ifpred(ifpred_elif->ifpred.value());
        }
      }

      void
      operator()(const ast::node::IfPredElseNode *const ifpred_else) const {
        gen.m_output << "    ;; else\n";
        gen.gen_scope(ifpred_else->scope);
        gen.m_output << "    ;; /else\n";
      }
    };

    Visitor visitor{.gen = *this};
    std::visit(visitor, ifpred->node);
  }

  void gen_term(const ast::node::TermNode *const term) noexcept {
    struct Visitor {
      Generator &gen;

      void operator()(const ast::node::TermIntLitNode *const int_lit) const {
        gen.m_output << "    ;; int_lit\n";
        gen.m_output << "    ;; /int_lit\n";
      }

      void operator()(const ast::node::TermIdentNode *const ident) const {
        gen.m_output << "    ;; ident\n";
        gen.m_output << "    ;; /ident\n";
      }

      void operator()(const ast::node::TermExprNode *const expr) const {
        gen.m_output << "    ;; expr\n";
        gen.gen_expression(expr->expression);
        gen.m_output << "    ;; /expr\n";
      }
    };

    Visitor visitor{.gen = *this};
    std::visit(visitor, term->node);
  }

  void gen_bin_expr(const ast::node::BinExprNode *const bin_expr) noexcept {
    struct Visitor {
      Generator &gen;

      void operator()(const ast::node::BinExprPowerNode *const node) const {
        gen.m_output << "    ;; power\n";
        gen.gen_expression(node->left);
        gen.gen_expression(node->right);
        gen.m_output << "    ;; /power\n";
      }

      void operator()(const ast::node::BinExprModNode *const node) const {
        gen.m_output << "    ;; mod\n";
        gen.gen_expression(node->left);
        gen.gen_expression(node->right);
        gen.m_output << "    ;; /mod\n";
      }

      void operator()(const ast::node::BinExprMulNode *const node) const {
        gen.m_output << "    ;; multiply\n";
        gen.gen_expression(node->left);
        gen.gen_expression(node->right);
        gen.m_output << "    ;; /multiply\n";
      }

      void operator()(const ast::node::BinExprDivNode *const node) const {
        gen.m_output << "    ;; divide\n";
        gen.gen_expression(node->left);
        gen.gen_expression(node->right);
        gen.m_output << "    ;; /divide\n";
      }

      void operator()(const ast::node::BinExprPlusNode *const node) const {
        gen.m_output << "    ;; add\n";
        gen.gen_expression(node->left);
        gen.gen_expression(node->right);
        gen.m_output << "    ;; /add\n";
      }

      void operator()(const ast::node::BinExprMinusNode *const node) const {
        gen.m_output << "    ;; subtract\n";
        gen.gen_expression(node->left);
        gen.gen_expression(node->right);
        gen.m_output << "    ;; /subtract\n";
      }

      void operator()(const ast::node::BinExprLessThanNode *const node) const {
        gen.m_output << "    ;; less than\n";
        gen.gen_expression(node->left);
        gen.gen_expression(node->right);
        gen.m_output << "    ;; /less than\n";
      }

      void
      operator()(const ast::node::BinExprGreaterThanNode *const node) const {
        gen.m_output << "    ;; greater than\n";
        gen.gen_expression(node->left);
        gen.gen_expression(node->right);
        gen.m_output << "    ;; /greater than\n";
      }
    };

    Visitor visitor{.gen = *this};
    std::visit(visitor, bin_expr->node);
  }

  // TODO(lthomas): Write output to this file
  std::filesystem::path m_output_path;
  std::ostringstream m_output;
  const ast::node::ProgNode &m_prog;
};

} // namespace pimento::generation