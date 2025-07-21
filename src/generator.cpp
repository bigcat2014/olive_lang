#include <variant>

#include <pimento/generator.hpp>
#include <pimento/utils.hpp>

namespace pimento::generation {

Generator::Generator(std::shared_ptr<std::istream> istream,
                     std::shared_ptr<std::ostream> ostream)
    : m_parser(istream), p_output(ostream) {}

void Generator::generate() noexcept {
  *p_output << "global _start\n_start:\n";

  const ast::node::ProgNode program = m_parser.get_program();
  for (const auto &statement : program.statements) {
    gen_statement(statement);
  }
}

void Generator::gen_statement(
    std::shared_ptr<ast::node::StmtNode> node) noexcept {
  struct Visitor {
    Generator &gen;

    void operator()(std::shared_ptr<ast::node::StmtExitNode> stmt) const {
      gen.gen_expression(stmt->expression);
      *gen.p_output << "    mov rax, 60\n";
      gen.pop("rdi");
      *gen.p_output << "    syscall\n";
    }

    void operator()(std::shared_ptr<ast::node::StmtLetNode> stmt) const {
      const auto it =
          std::ranges::find_if(std::as_const(gen.m_vars), [&](const Var &var) {
            bool match{false};
            // TODO(lthomas): Probably a cleaner way to do this... Refactor
            // later
            struct TokenVisitor {
              bool &match;
              const std::string &name;

              void operator()(tokenization::IdentProperties properties) {
                match = properties.identifier == name;
              }
              void operator()(tokenization::BinOpProperties) { match = false; }
              void operator()(tokenization::IntLitProperties) { match = false; }
              void operator()(std::monostate) { match = false; }
            };

            std::visit(TokenVisitor{.match = match, .name = var.name},
                       stmt->identifier.properties);
            return match;
          });

      if (it != gen.m_vars.cend()) {
        std::ostringstream oss;
        oss << "Identifier already used: ";
        // TODO(lthomas): Probably a cleaner way to do this... Refactor later
        struct TokenVisitor {
          std::ostringstream &oss;

          TokenVisitor(std::ostringstream &oss) : oss(oss) {}

          void operator()(tokenization::IdentProperties properties) {
            oss << properties.identifier;
          }
          void operator()(tokenization::BinOpProperties) {}
          void operator()(tokenization::IntLitProperties) {}
          void operator()(std::monostate) {}
        };

        std::visit(TokenVisitor(oss), stmt->identifier.properties);

        auto &logger = utils::get_logger();
        logger.error(oss.str());
        exit(EXIT_FAILURE);
      }

      // TODO(lthomas): Probably a cleaner way to do this... Refactor later
      struct TokenVisitor {
        std::vector<Var> &vars;
        size_t stack_size;

        TokenVisitor(std::vector<Var> &vars, size_t stack_size)
            : vars(vars), stack_size(stack_size) {}

        void operator()(tokenization::IdentProperties properties) {
          vars.emplace_back(properties.identifier, stack_size);
        }
        void operator()(tokenization::BinOpProperties) {}
        void operator()(tokenization::IntLitProperties) {}
        void operator()(std::monostate) {}
      };

      std::visit(TokenVisitor(gen.m_vars, gen.m_stack_size),
                 stmt->identifier.properties);

      gen.gen_expression(stmt->expression);
    }

    void operator()(std::shared_ptr<ast::node::StmtAssignNode> stmt) const {
      const auto it =
          std::ranges::find_if(std::as_const(gen.m_vars), [&](const Var &var) {
            bool match{false};
            // TODO(lthomas): Probably a cleaner way to do this... Refactor
            // later
            struct TokenVisitor {
              bool &match;
              const std::string &name;

              void operator()(tokenization::IdentProperties properties) {
                match = properties.identifier == name;
              }
              void operator()(tokenization::BinOpProperties) { match = false; }
              void operator()(tokenization::IntLitProperties) { match = false; }
              void operator()(std::monostate) { match = false; }
            };

            std::visit(TokenVisitor{.match = match, .name = var.name},
                       stmt->identifier.properties);
            return match;
          });

      if (it == gen.m_vars.cend()) {
        std::ostringstream oss;
        oss << "Undeclared identifier: ";
        // TODO(lthomas): Probably a cleaner way to do this... Refactor later
        struct TokenVisitor {
          std::ostringstream &oss;

          TokenVisitor(std::ostringstream &oss) : oss(oss) {}

          void operator()(tokenization::IdentProperties properties) {
            oss << properties.identifier;
          }
          void operator()(tokenization::BinOpProperties) {}
          void operator()(tokenization::IntLitProperties) {}
          void operator()(std::monostate) {}
        };

        std::visit(TokenVisitor(oss), stmt->identifier.properties);

        auto &logger = utils::get_logger();
        logger.error(oss.str());
        exit(EXIT_FAILURE);
      }

      gen.gen_expression(stmt->expression);
      gen.pop("rax");
      *gen.p_output << "    mov [rsp + "
                    << (gen.m_stack_size - it->stack_loc - 1) * 8 << "], rax\n";
    }

    void operator()(std::shared_ptr<ast::node::ScopeNode> stmt) const {
      *gen.p_output << "    ;; scope\n";
      gen.gen_scope(stmt);
      *gen.p_output << "    ;; /scope\n";
    }

    void operator()(std::shared_ptr<ast::node::StmtIfNode> stmt) const {

      gen.gen_expression(stmt->expression);
      gen.pop("rax");
      const std::string label = gen.create_label();
      *gen.p_output << "    test rax, rax\n";
      // TODO(lthomas): Hardcoded 0 evaluates to false and > 0 evaluates to
      // true
      *gen.p_output << "    jz " << label << "\n";
      gen.gen_scope(stmt->scope);
      if (stmt->ifpred.has_value()) {
        const std::string end_label = gen.create_label();
        *gen.p_output << "    jmp " << end_label << "\n";
        *gen.p_output << label << ":\n";
        gen.gen_ifpred(stmt->ifpred.value(), end_label);
        *gen.p_output << end_label << ":\n";
      } else {
        *gen.p_output << label << ":\n";
      }
    }

    void operator()(std::shared_ptr<ast::node::StmtWhileNode> stmt) const {

      const std::string loop_label = gen.create_label();
      const std::string end_label = gen.create_label();

      *gen.p_output << loop_label << ":\n";
      gen.gen_expression(stmt->expression);
      gen.pop("rax");
      *gen.p_output << "    test rax, rax\n";
      *gen.p_output << "    jz " << end_label << "\n";
      gen.gen_scope(stmt->scope);
      *gen.p_output << "    jmp " << loop_label << "\n";
      *gen.p_output << end_label << ":\n";
    }
  };

  std::visit(Visitor{.gen = *this}, node->node);
}

void Generator::gen_expression(
    std::shared_ptr<ast::node::ExprNode> node) noexcept {
  struct Visitor {
    Generator &gen;

    void operator()(std::shared_ptr<ast::node::TermNode> expr) const {
      gen.gen_term(expr);
    }

    void operator()(std::shared_ptr<ast::node::BinExprNode> expr) const {
      gen.gen_bin_expr(expr);
    }
  };

  std::visit(Visitor{.gen = *this}, node->node);
}

void Generator::gen_scope(std::shared_ptr<ast::node::ScopeNode> node) noexcept {
  for (const auto &stmt : node->statements) {
    begin_scope();
    gen_statement(stmt);
    end_scope();
  }
}

void Generator::gen_ifpred(std::shared_ptr<ast::node::IfPredNode> node,
                           const std::string &end_label) noexcept {
  struct Visitor {
    Generator &gen;
    const std::string &end_label;

    void operator()(std::shared_ptr<ast::node::IfPredElifNode> ifpred) const {

      gen.gen_expression(ifpred->expression);
      gen.pop("rax");
      const std::string label = gen.create_label();
      *gen.p_output << "    test rax, rax\n";
      // TODO(lthomas): Hardcoded 0 evaluates to false and > 0 evaluates to
      // true
      *gen.p_output << "    jz " << label << "\n";
      gen.gen_scope(ifpred->scope);
      *gen.p_output << "    jmp " << end_label << "\n";
      *gen.p_output << label << ":\n";
      if (ifpred->ifpred.has_value()) {
        gen.gen_ifpred(ifpred->ifpred.value(), end_label);
      }
    }

    void operator()(std::shared_ptr<ast::node::IfPredElseNode> ifpred) const {
      gen.gen_scope(ifpred->scope);
    }
  };

  std::visit(Visitor{.gen = *this, .end_label = end_label}, node->node);
}

void Generator::gen_term(std::shared_ptr<ast::node::TermNode> node) noexcept {
  struct Visitor {
    Generator &gen;

    void operator()(std::shared_ptr<ast::node::TermIntLitNode> term) const {
      // TODO(lthomas): Probably a cleaner way to do this... Refactor later
      struct TokenVisitor {
        Generator &gen;

        TokenVisitor(Generator &gen) : gen(gen) {}

        void operator()(tokenization::IdentProperties) {}
        void operator()(tokenization::BinOpProperties) {}
        void operator()(tokenization::IntLitProperties properties) {
          *gen.p_output << "    mov rax, " << properties.value << "\n";
          gen.push("rax");
        }
        void operator()(std::monostate) {}
      };

      std::visit(TokenVisitor(gen), term->int_lit_token.properties);
    }

    void operator()(std::shared_ptr<ast::node::TermIdentNode> term) const {
      const auto it =
          std::ranges::find_if(std::as_const(gen.m_vars), [&](const Var &var) {
            bool match = false;
            // TODO(lthomas): Probably a cleaner way to do this... Refactor
            // later
            struct TokenVisitor {
              bool &match;
              const std::string &name;

              void operator()(tokenization::IdentProperties properties) {
                match = properties.identifier == name;
              }
              void operator()(tokenization::BinOpProperties) { match = false; }
              void operator()(tokenization::IntLitProperties) { match = false; }
              void operator()(std::monostate) { match = false; }
            };

            std::visit(TokenVisitor{.match = match, .name = var.name},
                       term->identifier_token.properties);
            return match;
          });

      if (it == gen.m_vars.cend()) {
        std::ostringstream oss;
        oss << "Undeclared identifier: ";
        // TODO(lthomas): Probably a cleaner way to do this... Refactor later
        struct TokenVisitor {
          std::ostringstream &oss;

          TokenVisitor(std::ostringstream &oss) : oss(oss) {}

          void operator()(tokenization::IdentProperties properties) {
            oss << properties.identifier;
          }
          void operator()(tokenization::BinOpProperties) {}
          void operator()(tokenization::IntLitProperties) {}
          void operator()(std::monostate) {}
        };

        std::visit(TokenVisitor(oss), term->identifier_token.properties);

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

    void operator()(std::shared_ptr<ast::node::TermExprNode> term) const {
      gen.gen_expression(term->expression);
    }
  };

  std::visit(Visitor{.gen = *this}, node->node);
}

void Generator::gen_bin_expr(
    std::shared_ptr<ast::node::BinExprNode> node) noexcept {
  struct Visitor {
    Generator &gen;

    void
    operator()(std::shared_ptr<ast::node::BinExprPowerNode> bin_expr) const {
      gen.gen_expression(bin_expr->right);
      gen.gen_expression(bin_expr->left);

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

    void operator()(std::shared_ptr<ast::node::BinExprModNode> bin_expr) const {
      gen.gen_expression(bin_expr->right);
      gen.gen_expression(bin_expr->left);
      gen.pop("rax");
      gen.pop("rbx");
      *gen.p_output << "    div rbx\n";
      gen.push("rdx");
    }

    void operator()(std::shared_ptr<ast::node::BinExprMulNode> bin_expr) const {
      gen.gen_expression(bin_expr->right);
      gen.gen_expression(bin_expr->left);
      gen.pop("rax");
      gen.pop("rbx");
      *gen.p_output << "    mul rbx\n";
      gen.push("rax");
    }

    void operator()(std::shared_ptr<ast::node::BinExprDivNode> bin_expr) const {
      gen.gen_expression(bin_expr->right);
      gen.gen_expression(bin_expr->left);
      gen.pop("rax");
      gen.pop("rbx");
      *gen.p_output << "    div rbx\n";
      gen.push("rax");
    }

    void
    operator()(std::shared_ptr<ast::node::BinExprPlusNode> bin_expr) const {
      gen.gen_expression(bin_expr->right);
      gen.gen_expression(bin_expr->left);
      gen.pop("rax");
      gen.pop("rbx");
      *gen.p_output << "    add rax, rbx\n";
      gen.push("rax");
    }

    void
    operator()(std::shared_ptr<ast::node::BinExprMinusNode> bin_expr) const {
      gen.gen_expression(bin_expr->right);
      gen.gen_expression(bin_expr->left);
      gen.pop("rax");
      gen.pop("rbx");
      *gen.p_output << "    sub rax, rbx\n";
      gen.push("rax");
    }

    void
    operator()(std::shared_ptr<ast::node::BinExprLessThanNode> bin_expr) const {
      gen.gen_expression(bin_expr->right);
      gen.gen_expression(bin_expr->left);

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

    void operator()(
        std::shared_ptr<ast::node::BinExprGreaterThanNode> bin_expr) const {
      gen.gen_expression(bin_expr->right);
      gen.gen_expression(bin_expr->left);

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

  std::visit(Visitor{.gen = *this}, node->node);
}

void Generator::push(const std::string &reg) noexcept {
  *p_output << "    push " << reg << "\n";
  m_stack_size++;
}

void Generator::pop(const std::string &reg) noexcept {
  *p_output << "    pop " << reg << "\n";
  m_stack_size--;
}

void Generator::begin_scope() noexcept { m_scopes.emplace_back(m_vars.size()); }

void Generator::end_scope() noexcept {
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

std::string Generator::create_label() const noexcept {
  static size_t label_count{0};

  std::ostringstream oss;
  oss << "label" << std::to_string(label_count++);
  return oss.str();
}

Generator::Var::Var(const std::string &name, size_t stack_loc)
    : name(name), stack_loc(stack_loc) {}

} // namespace pimento::generation