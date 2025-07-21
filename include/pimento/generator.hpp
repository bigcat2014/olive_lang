//! @file generator.hpp
//! @brief Pimento ASM generator
//! @author Logan Thomas

#pragma once

#include <memory>
#include <ostream>
#include <pimento/ast.hpp>
#include <pimento/utils.hpp>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>

namespace pimento::generation {

class Generator {
public:
  explicit Generator(std::ostream *stream, const ast::node::ProgNode &prog)
      : p_output(stream), m_prog(prog) {}

  void generate() noexcept {
    *p_output << "global _start\n_start:\n";

    for (const auto statement : m_prog.statements) {
      gen_statement(statement);
    }
  }

private:
  void gen_statement(const ast::node::StmtNode *const stmt) noexcept {
    struct Visitor {
      Generator &gen;

      void operator()(const ast::node::StmtExitNode *const stmt_node) const {
        gen.gen_expression(stmt_node->expression);
        *gen.p_output << "    mov rax, 60\n";
        gen.pop("rdi");
        *gen.p_output << "    syscall\n";
      }

      void operator()(const ast::node::StmtLetNode *const stmt_node) const {
        const auto it = std::ranges::find_if(
            std::as_const(gen.m_vars), [&](const Var &var) {
              bool match{false};
              // TODO(lthomas): Probably a cleaner way to do this... Refactor
              // later
              struct TokenVisitor {
                bool &match;
                const std::string &name;

                void operator()(tokenization::IdentProperties properties) {
                  match = properties.identifier == name;
                }
                void operator()(tokenization::BinOpProperties) {
                  match = false;
                }
                void operator()(tokenization::IntLitProperties) {
                  match = false;
                }
                void operator()(std::monostate) { match = false; }
              };

              std::visit(TokenVisitor{.match = match, .name = var.name},
                         stmt_node->identifier.properties);
              return match;
            });

        if (it != gen.m_vars.cend()) {
          std::ostringstream oss;
          oss << "Identifier already used: ";
          // TODO(lthomas): Probably a cleaner way to do this... Refactor later
          struct TokenVisitor {
            std::ostringstream &oss;
            void operator()(tokenization::IdentProperties properties) {
              oss << properties.identifier;
            }
            void operator()(tokenization::BinOpProperties) {}
            void operator()(tokenization::IntLitProperties) {}
            void operator()(std::monostate) {}
          };

          std::visit(TokenVisitor(oss), stmt_node->identifier.properties);

          auto &logger = utils::get_logger();
          logger.error(oss.str());
          exit(EXIT_FAILURE);
        }

        // TODO(lthomas): Probably a cleaner way to do this... Refactor later
        struct TokenVisitor {
          std::vector<Var> &m_vars;
          size_t stack_size;

          void operator()(tokenization::IdentProperties properties) {
            m_vars.emplace_back(properties.identifier, stack_size);
          }
          void operator()(tokenization::BinOpProperties) {}
          void operator()(tokenization::IntLitProperties) {}
          void operator()(std::monostate) {}
        };

        std::visit(TokenVisitor(gen.m_vars, gen.m_stack_size),
                   stmt_node->identifier.properties);

        gen.gen_expression(stmt_node->expression);
      }

      void operator()(const ast::node::StmtAssignNode *const stmt_node) const {
        gen.gen_expression(stmt_node->expression);

        const auto it = std::ranges::find_if(
            std::as_const(gen.m_vars), [&](const Var &var) {
              bool match{false};
              // TODO(lthomas): Probably a cleaner way to do this... Refactor
              // later
              struct TokenVisitor {
                bool &match;
                const std::string &name;

                void operator()(tokenization::IdentProperties properties) {
                  match = properties.identifier == name;
                }
                void operator()(tokenization::BinOpProperties) {
                  match = false;
                }
                void operator()(tokenization::IntLitProperties) {
                  match = false;
                }
                void operator()(std::monostate) { match = false; }
              };

              std::visit(TokenVisitor{.match = match, .name = var.name},
                         stmt_node->identifier.properties);
              return match;
            });

        if (it == gen.m_vars.cend()) {
          std::ostringstream oss;
          oss << "Undeclared identifier: ";
          // TODO(lthomas): Probably a cleaner way to do this... Refactor later
          struct TokenVisitor {
            std::ostringstream &oss;
            void operator()(tokenization::IdentProperties properties) {
              oss << properties.identifier;
            }
            void operator()(tokenization::BinOpProperties) {}
            void operator()(tokenization::IntLitProperties) {}
            void operator()(std::monostate) {}
          };

          std::visit(TokenVisitor(oss), stmt_node->identifier.properties);

          auto &logger = utils::get_logger();
          logger.error(oss.str());
          exit(EXIT_FAILURE);
        }

        gen.gen_expression(stmt_node->expression);
        gen.pop("rax");
        *gen.p_output << "    mov [rsp + "
                      << (gen.m_stack_size - it->stack_loc - 1) * 8
                      << "], rax\n";
      }

      void operator()(const ast::node::ScopeNode *const stmt_node) const {
        *gen.p_output << "    ;; scope\n";
        gen.gen_scope(stmt_node);
        *gen.p_output << "    ;; /scope\n";
      }

      void operator()(const ast::node::StmtIfNode *const stmt_node) const {

        gen.gen_expression(stmt_node->expression);
        gen.pop("rax");
        const std::string label = gen.create_label();
        *gen.p_output << "    test rax, rax\n";
        // TODO(lthomas): Hardcoded 0 evaluates to false and > 0 evaluates to
        // true
        *gen.p_output << "    jz " << label << "\n";
        gen.gen_scope(stmt_node->scope);
        if (stmt_node->ifpred.has_value()) {
          const std::string end_label = gen.create_label();
          *gen.p_output << "    jmp " << end_label << "\n";
          *gen.p_output << label << ":\n";
          gen.gen_ifpred(stmt_node->ifpred.value(), end_label);
          *gen.p_output << end_label << ":\n";
        } else {
          *gen.p_output << label << ":\n";
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
        gen.gen_term(term);
      }

      void operator()(const ast::node::BinExprNode *const bin_expr) const {
        gen.gen_bin_expr(bin_expr);
      }
    };

    Visitor visitor{.gen = *this};
    std::visit(visitor, expr->node);
  }

  void gen_scope(const ast::node::ScopeNode *const scope) noexcept {
    for (const auto stmt : scope->statements) {
      begin_scope();
      gen_statement(stmt);
      end_scope();
    }
  }

  void gen_ifpred(const ast::node::IfPredNode *const ifpred,
                  const std::string &end_label) noexcept {
    struct Visitor {
      Generator &gen;
      const std::string &end_label;

      void
      operator()(const ast::node::IfPredElifNode *const ifpred_elif) const {

        gen.gen_expression(ifpred_elif->expression);
        gen.pop("rax");
        const std::string label = gen.create_label();
        *gen.p_output << "    test rax, rax\n";
        // TODO(lthomas): Hardcoded 0 evaluates to false and > 0 evaluates to
        // true
        *gen.p_output << "    jz " << label << "\n";
        gen.gen_scope(ifpred_elif->scope);
        *gen.p_output << "    jmp " << end_label << "\n";
        *gen.p_output << label << ":\n";
        if (ifpred_elif->ifpred.has_value()) {
          gen.gen_ifpred(ifpred_elif->ifpred.value(), end_label);
        }
      }

      void
      operator()(const ast::node::IfPredElseNode *const ifpred_else) const {
        gen.gen_scope(ifpred_else->scope);
      }
    };

    Visitor visitor{.gen = *this, .end_label = end_label};
    std::visit(visitor, ifpred->node);
  }

  void gen_term(const ast::node::TermNode *const term) noexcept {
    struct Visitor {
      Generator &gen;

      void operator()(const ast::node::TermIntLitNode *const int_lit) const {
        // TODO(lthomas): Probably a cleaner way to do this... Refactor later
        struct TokenVisitor {
          Generator &gen;
          void operator()(tokenization::IdentProperties) {}
          void operator()(tokenization::BinOpProperties) {}
          void operator()(tokenization::IntLitProperties properties) {
            *gen.p_output << "    mov rax, " << properties.value << "\n";
            gen.push("rax");
          }
          void operator()(std::monostate) {}
        };

        std::visit(TokenVisitor(gen), int_lit->int_lit_token.properties);
      }

      void operator()(const ast::node::TermIdentNode *const ident) const {
        const auto it = std::ranges::find_if(
            std::as_const(gen.m_vars), [&](const Var &var) {
              bool match = false;
              // TODO(lthomas): Probably a cleaner way to do this... Refactor
              // later
              struct TokenVisitor {
                bool &match;
                const std::string &name;

                void operator()(tokenization::IdentProperties properties) {
                  match = properties.identifier == name;
                }
                void operator()(tokenization::BinOpProperties) {
                  match = false;
                }
                void operator()(tokenization::IntLitProperties) {
                  match = false;
                }
                void operator()(std::monostate) { match = false; }
              };

              std::visit(TokenVisitor{.match = match, .name = var.name},
                         ident->identifier_token.properties);
              return match;
            });

        if (it == gen.m_vars.cend()) {
          std::ostringstream oss;
          oss << "Undeclared identifier: ";
          // TODO(lthomas): Probably a cleaner way to do this... Refactor later
          struct TokenVisitor {
            std::ostringstream &oss;
            void operator()(tokenization::IdentProperties properties) {
              oss << properties.identifier;
            }
            void operator()(tokenization::BinOpProperties) {}
            void operator()(tokenization::IntLitProperties) {}
            void operator()(std::monostate) {}
          };

          std::visit(TokenVisitor(oss), ident->identifier_token.properties);

          auto &logger = utils::get_logger();
          logger.error(oss.str());
          exit(EXIT_FAILURE);
        }

        std::ostringstream offset;
        // TODO(lthomas): Multiplying by 8 bytes for 64 bit sytem
        offset << "QWORD [rsp + " << (gen.m_stack_size - it->stack_loc - 1) * 8
               << "]";
        gen.push(offset.str());
      }

      void operator()(const ast::node::TermExprNode *const expr) const {
        gen.gen_expression(expr->expression);
      }
    };

    Visitor visitor{.gen = *this};
    std::visit(visitor, term->node);
  }

  void gen_bin_expr(const ast::node::BinExprNode *const bin_expr) noexcept {
    struct Visitor {
      Generator &gen;

      void operator()(const ast::node::BinExprPowerNode *const node) const {
        gen.gen_expression(node->right);
        gen.gen_expression(node->left);

        std::string loop_label = gen.create_label();
        std::string neg_label = gen.create_label();
        std::string end_label = gen.create_label();

        gen.pop("rax");
        *gen.p_output << "    mov rbx, rax\n";
        gen.pop("rcx");
        *gen.p_output << loop_label << ":\n";
        *gen.p_output << "    sub rcx, 1\n";
        *gen.p_output << "    jc " << neg_label << "\n";
        *gen.p_output << "    jz " << end_label << "\n";
        *gen.p_output << "    mul rbx\n";
        *gen.p_output << "    jmp " << loop_label << "\n";
        *gen.p_output << neg_label << ":\n";
        *gen.p_output << "    mov rax, 1\n";
        *gen.p_output << end_label << ":\n";
        gen.push("rax");
      }

      void operator()(const ast::node::BinExprModNode *const node) const {
        gen.gen_expression(node->right);
        gen.gen_expression(node->left);
        gen.pop("rax");
        gen.pop("rbx");
        *gen.p_output << "    div rbx\n";
        gen.push("rdx");
      }

      void operator()(const ast::node::BinExprMulNode *const node) const {
        gen.gen_expression(node->right);
        gen.gen_expression(node->left);
        gen.pop("rax");
        gen.pop("rbx");
        *gen.p_output << "    mul rbx\n";
        gen.push("rax");
      }

      void operator()(const ast::node::BinExprDivNode *const node) const {
        gen.gen_expression(node->right);
        gen.gen_expression(node->left);
        gen.pop("rax");
        gen.pop("rbx");
        *gen.p_output << "    div rbx\n";
        gen.push("rax");
      }

      void operator()(const ast::node::BinExprPlusNode *const node) const {
        gen.gen_expression(node->right);
        gen.gen_expression(node->left);
        gen.pop("rax");
        gen.pop("rbx");
        *gen.p_output << "    add rax, rbx\n";
        gen.push("rax");
      }

      void operator()(const ast::node::BinExprMinusNode *const node) const {
        gen.gen_expression(node->right);
        gen.gen_expression(node->left);
        gen.pop("rax");
        gen.pop("rbx");
        *gen.p_output << "    sub rax, rbx\n";
        gen.push("rax");
      }

      void operator()(const ast::node::BinExprLessThanNode *const node) const {
        gen.gen_expression(node->right);
        gen.gen_expression(node->left);

        std::string label = gen.create_label();
        std::string end_label = gen.create_label();

        gen.pop("rax");
        gen.pop("rbx");
        *gen.p_output << "    cmp rax, rbx\n";
        *gen.p_output << "    jl " << label << "\n";
        *gen.p_output << "    mov rax, 0\n";
        *gen.p_output << "    jmp " << end_label << "\n";
        *gen.p_output << label << ":\n";
        *gen.p_output << "    mov rax, 1\n";
        *gen.p_output << end_label << ":\n";
        gen.push("rax");
      }

      void
      operator()(const ast::node::BinExprGreaterThanNode *const node) const {
        gen.gen_expression(node->right);
        gen.gen_expression(node->left);

        std::string label = gen.create_label();
        std::string end_label = gen.create_label();

        gen.pop("rax");
        gen.pop("rbx");
        *gen.p_output << "    cmp rax, rbx\n";
        *gen.p_output << "    jg " << label << "\n";
        *gen.p_output << "    mov rax, 0\n";
        *gen.p_output << "    jmp " << end_label << "\n";
        *gen.p_output << label << ":\n";
        *gen.p_output << "    mov rax, 1\n";
        *gen.p_output << end_label << ":\n";
        gen.push("rax");
      }
    };

    Visitor visitor{.gen = *this};
    std::visit(visitor, bin_expr->node);
  }

  void push(const std::string &reg) noexcept {
    *p_output << "    push " << reg << "\n";
    m_stack_size++;
  }

  void pop(const std::string &reg) noexcept {
    *p_output << "    pop " << reg << "\n";
    m_stack_size--;
  }

  void begin_scope() { m_scopes.push_back(m_vars.size()); }

  void end_scope() noexcept {
    const size_t pop_count = m_vars.size() - m_scopes.back();
    if (pop_count != 0) {
      *p_output << "    add rsp, " << pop_count * 8 << "\n";
    }
    m_stack_size -= pop_count;
    for (size_t i = 0; i < pop_count; i++) {
      m_vars.pop_back();
    }
    m_scopes.pop_back();
  }

  std::string create_label() noexcept {
    static size_t s_label_count{0};

    std::ostringstream oss;
    oss << "label" << std::to_string(s_label_count++);
    return oss.str();
  }

private:
  struct Var {
    std::string name;
    size_t stack_loc;
  };

  // helpers for visitors
  template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

  std::unique_ptr<std::ostream> p_output;
  const ast::node::ProgNode &m_prog;

  size_t m_stack_size{0};
  std::vector<Var> m_vars{};
  std::vector<size_t> m_scopes{};
};

} // namespace pimento::generation