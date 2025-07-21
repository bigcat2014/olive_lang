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

    for (const auto &statement : m_prog.statements) {
      gen_statement(statement);
    }

    std::cout << m_output.str() << std::endl;
  }

private:
  void
  gen_statement(const std::unique_ptr<ast::node::StmtNode> &stmt) noexcept {
    struct StmtVisitor {
      std::ostringstream &m_output;

      void operator()(
          const std::unique_ptr<ast::node::StmtExitNode> &stmt_exit) const {
        m_output << "    ;; exit\n";
        m_output << "    mov rax, 60\n";
        m_output << "    mov rdi, 23\n";
        m_output << "    syscall\n";
        m_output << "    ;; /exit\n";
      }

      void operator()(
          const std::unique_ptr<ast::node::StmtLetNode> &stmt_let) const {
        m_output << "    ;; let\n";
        m_output << "    ;; /let\n";
      }

      void operator()(
          const std::unique_ptr<ast::node::StmtAssignNode> &stmt_assign) const {
        m_output << "    ;; assign";
        m_output << "    ;; /assign";
      }

      void
      operator()(const std::unique_ptr<ast::node::ScopeNode> &scope) const {
        m_output << "    ;; scope\n";
        m_output << "    ;; /scope\n";
      }

      void
      operator()(const std::unique_ptr<ast::node::StmtIfNode> &stmt_if) const {
        m_output << "    ;; if\n";
        m_output << "    ;; /if\n";
      }
    };

    StmtVisitor visitor{.m_output = m_output};
    std::visit(visitor, stmt->node);
  }

  // TODO(lthomas): Write output to this file
  std::filesystem::path m_output_path;
  std::ostringstream m_output;
  const ast::node::ProgNode &m_prog;
};

} // namespace pimento::generation