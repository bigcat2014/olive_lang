#include <utility>
#include <variant>

#include <pimento/generator.hpp>
#include <pimento/utils.hpp>

namespace pimento::generation {

Generator::Generator(std::istream* istream, std::ostream* ostream)
    : mParser(istream)
    , mOutput(ostream)
{}

void Generator::generate() noexcept
{
    (*mOutput) << "global _start\n_start:\n";

    const ast::node::ProgNode PROGRAM = mParser.getProgram();
    for (const auto& statement : PROGRAM.statements) {
        genStatement(statement);
    }
}

void Generator::genStatement(const std::shared_ptr<ast::node::StmtNode>& node) noexcept
{
    struct Visitor
    {
        explicit Visitor(Generator& gen)
            : gen(gen)
        {}

        Visitor(const Visitor& other) = delete;
        Visitor(Visitor&& other)      = delete;

        Generator& gen;

        void operator()(const std::shared_ptr<ast::node::StmtExitNode>& stmt) const
        {
            gen.genExpression(stmt->expression);
            (*gen.mOutput) << "    mov rax, 60\n";
            gen.pop("rdi");
            (*gen.mOutput) << "    syscall\n";
        }

        void operator()(const std::shared_ptr<ast::node::StmtLetNode>& stmt) const
        {
            // TODO(lthomas): Not implemented
            (void)stmt;
        }

        void operator()(const std::shared_ptr<ast::node::StmtAssignNode>& stmt) const
        {
            // TODO(lthomas): Not implemented
            (void)stmt;
        }

        void operator()(const std::shared_ptr<ast::node::ScopeNode>& stmt) const
        {
            (*gen.mOutput) << "    ;; scope\n";
            gen.genScope(stmt);
            (*gen.mOutput) << "    ;; /scope\n";
        }

        void operator()(const std::shared_ptr<ast::node::StmtIfNode>& stmt) const
        {
            gen.genExpression(stmt->expression);
            gen.pop("rax");
            const std::string LABEL = pimento::generation::Generator::createLabel();
            (*gen.mOutput) << "    test rax, rax\n";
            // TODO(lthomas): Hardcoded 0 evaluates to false and > 0 evaluates to
            // true
            (*gen.mOutput) << "    jz " << LABEL << "\n";
            gen.genScope(stmt->scope);
            if (stmt->ifpred.has_value()) {
                const std::string END_LABEL = pimento::generation::Generator::createLabel();
                (*gen.mOutput) << "    jmp " << END_LABEL << "\n";
                (*gen.mOutput) << LABEL << ":\n";
                gen.genIfpred(stmt->ifpred.value(), END_LABEL);
                (*gen.mOutput) << END_LABEL << ":\n";
            }
            else {
                (*gen.mOutput) << LABEL << ":\n";
            }
        }

        void operator()(const std::shared_ptr<ast::node::StmtWhileNode>& stmt) const
        {
            const std::string LOOP_LABEL = pimento::generation::Generator::createLabel();
            const std::string END_LABEL  = pimento::generation::Generator::createLabel();

            (*gen.mOutput) << LOOP_LABEL << ":\n";
            gen.genExpression(stmt->expression);
            gen.pop("rax");
            (*gen.mOutput) << "    test rax, rax\n";
            (*gen.mOutput) << "    jz " << END_LABEL << "\n";
            gen.genScope(stmt->scope);
            (*gen.mOutput) << "    jmp " << LOOP_LABEL << "\n";
            (*gen.mOutput) << END_LABEL << ":\n";
        }
    };

    try {
        std::visit(Visitor(*this), node->node);
    } catch (const std::bad_variant_access& e) {
        // Should never happen
        auto& logger = utils::getLogger();
        logger.error("Generator encountered bad variant access in \"{}\": {}", __FUNCTION__, e.what());
        exit(EXIT_FAILURE);
    }
}

void Generator::genExpression(const std::shared_ptr<ast::node::ExprNode>& node) noexcept
{
    struct Visitor
    {
        explicit Visitor(Generator& gen)
            : gen(gen)
        {}

        Visitor(const Visitor& other) = delete;
        Visitor(Visitor&& other)      = delete;

        Generator& gen;

        void operator()(const std::shared_ptr<ast::node::TermNode>& expr) const { gen.genTerm(expr); }

        void operator()(const std::shared_ptr<ast::node::BinExprNode>& expr) const { gen.genBinExpr(expr); }
    };

    try {
        std::visit(Visitor(*this), node->node);
    } catch (const std::bad_variant_access& e) {
        // Should never happen
        auto& logger = utils::getLogger();
        logger.error("Generator encountered bad variant access in \"{}\": {}", __FUNCTION__, e.what());
        exit(EXIT_FAILURE);
    }
}

void Generator::genScope(const std::shared_ptr<ast::node::ScopeNode>& node) noexcept
{
    for (const auto& stmt : node->statements) {
        beginScope();
        genStatement(stmt);
        endScope();
    }
}

void Generator::genIfpred(const std::shared_ptr<ast::node::IfPredNode>& node, const std::string& endLabel) noexcept
{
    struct Visitor
    {
        Visitor(Generator& gen, const std::string& endLabel)
            : gen(gen)
            , endLabel(endLabel)
        {}

        Visitor(const Visitor& other) = delete;
        Visitor(Visitor&& other)      = delete;

        Generator& gen;
        const std::string endLabel;

        void operator()(const std::shared_ptr<ast::node::IfPredElifNode>& ifpred) const
        {
            gen.genExpression(ifpred->expression);
            gen.pop("rax");
            const std::string LABEL = pimento::generation::Generator::createLabel();
            (*gen.mOutput) << "    test rax, rax\n";
            // TODO(lthomas): Hardcoded 0 evaluates to false and > 0 evaluates to
            // true
            (*gen.mOutput) << "    jz " << LABEL << "\n";
            gen.genScope(ifpred->scope);
            (*gen.mOutput) << "    jmp " << endLabel << "\n";
            (*gen.mOutput) << LABEL << ":\n";
            if (ifpred->ifpred.has_value()) {
                gen.genIfpred(ifpred->ifpred.value(), endLabel);
            }
        }

        void operator()(const std::shared_ptr<ast::node::IfPredElseNode>& ifpred) const { gen.genScope(ifpred->scope); }
    };

    try {
        std::visit(Visitor(*this, endLabel), node->node);
    } catch (const std::bad_variant_access& e) {
        // Should never happen
        auto& logger = utils::getLogger();
        logger.error("Generator encountered bad variant access in \"{}\": {}", __FUNCTION__, e.what());
        exit(EXIT_FAILURE);
    }
}

void Generator::genTerm(const std::shared_ptr<ast::node::TermNode>& node) noexcept
{
    struct Visitor
    {
        explicit Visitor(Generator& gen)
            : gen(gen)
        {}

        Visitor(const Visitor& other) = delete;
        Visitor(Visitor&& other)      = delete;

        Generator& gen;

        void operator()(const std::shared_ptr<ast::node::TermIntLitNode>& term) const
        {
            // TODO(lthomas): Not implemented
            (void)term;
        }

        void operator()(const std::shared_ptr<ast::node::TermIdentNode>& term) const
        {
            // TODO(lthomas): Not implemented
            (void)term;
        }

        void operator()(const std::shared_ptr<ast::node::TermExprNode>& term) const
        {
            gen.genExpression(term->expression);
        }
    };

    try {
        std::visit(Visitor(*this), node->node);
    } catch (const std::bad_variant_access& e) {
        // Should never happen
        auto& logger = utils::getLogger();
        logger.error("Generator encountered bad variant access in \"{}\": {}", __FUNCTION__, e.what());
        exit(EXIT_FAILURE);
    }
}

void Generator::genBinExpr(const std::shared_ptr<ast::node::BinExprNode>& node) noexcept
{
    struct Visitor
    {
        explicit Visitor(Generator& gen)
            : gen(gen)
        {}

        Visitor(const Visitor& other) = delete;
        Visitor(Visitor&& other)      = delete;

        Generator& gen;

        void operator()(const std::shared_ptr<ast::node::BinExprPowerNode>& binExpr) const
        {
            gen.genExpression(binExpr->right);
            gen.genExpression(binExpr->left);

            std::string const loopLabel = pimento::generation::Generator::createLabel();
            std::string const negLabel  = pimento::generation::Generator::createLabel();
            std::string const endLabel  = pimento::generation::Generator::createLabel();

            gen.pop("rax");
            (*gen.mOutput) << "    mov rbx, rax\n";
            gen.pop("rcx");
            (*gen.mOutput) << loopLabel << ":\n";
            (*gen.mOutput) << "    sub rcx, 1\n";
            (*gen.mOutput) << "    jc " << negLabel << "\n";
            (*gen.mOutput) << "    jz " << endLabel << "\n";
            (*gen.mOutput) << "    mul rbx\n";
            (*gen.mOutput) << "    jmp " << loopLabel << "\n";
            (*gen.mOutput) << negLabel << ":\n";
            (*gen.mOutput) << "    mov rax, 1\n";
            (*gen.mOutput) << endLabel << ":\n";
            gen.push("rax");
        }

        void operator()(const std::shared_ptr<ast::node::BinExprModNode>& binExpr) const
        {
            gen.genExpression(binExpr->right);
            gen.genExpression(binExpr->left);
            gen.pop("rax");
            gen.pop("rbx");
            (*gen.mOutput) << "    div rbx\n";
            gen.push("rdx");
        }

        void operator()(const std::shared_ptr<ast::node::BinExprMulNode>& binExpr) const
        {
            gen.genExpression(binExpr->right);
            gen.genExpression(binExpr->left);
            gen.pop("rax");
            gen.pop("rbx");
            (*gen.mOutput) << "    mul rbx\n";
            gen.push("rax");
        }

        void operator()(const std::shared_ptr<ast::node::BinExprDivNode>& binExpr) const
        {
            gen.genExpression(binExpr->right);
            gen.genExpression(binExpr->left);
            gen.pop("rax");
            gen.pop("rbx");
            (*gen.mOutput) << "    div rbx\n";
            gen.push("rax");
        }

        void operator()(const std::shared_ptr<ast::node::BinExprPlusNode>& binExpr) const
        {
            gen.genExpression(binExpr->right);
            gen.genExpression(binExpr->left);
            gen.pop("rax");
            gen.pop("rbx");
            (*gen.mOutput) << "    add rax, rbx\n";
            gen.push("rax");
        }

        void operator()(const std::shared_ptr<ast::node::BinExprMinusNode>& binExpr) const
        {
            gen.genExpression(binExpr->right);
            gen.genExpression(binExpr->left);
            gen.pop("rax");
            gen.pop("rbx");
            (*gen.mOutput) << "    sub rax, rbx\n";
            gen.push("rax");
        }

        void operator()(const std::shared_ptr<ast::node::BinExprLessThanNode>& binExpr) const
        {
            gen.genExpression(binExpr->right);
            gen.genExpression(binExpr->left);

            std::string const label    = pimento::generation::Generator::createLabel();
            std::string const endLabel = pimento::generation::Generator::createLabel();

            gen.pop("rax");
            gen.pop("rbx");
            (*gen.mOutput) << "    cmp rax, rbx\n";
            (*gen.mOutput) << "    jl " << label << "\n";
            (*gen.mOutput) << "    mov rax, 0\n";
            (*gen.mOutput) << "    jmp " << endLabel << "\n";
            (*gen.mOutput) << label << ":\n";
            (*gen.mOutput) << "    mov rax, 1\n";
            (*gen.mOutput) << endLabel << ":\n";
            gen.push("rax");
        }

        void operator()(const std::shared_ptr<ast::node::BinExprGreaterThanNode>& binExpr) const
        {
            gen.genExpression(binExpr->right);
            gen.genExpression(binExpr->left);

            std::string const label    = pimento::generation::Generator::createLabel();
            std::string const endLabel = pimento::generation::Generator::createLabel();

            gen.pop("rax");
            gen.pop("rbx");
            (*gen.mOutput) << "    cmp rax, rbx\n";
            (*gen.mOutput) << "    jg " << label << "\n";
            (*gen.mOutput) << "    mov rax, 0\n";
            (*gen.mOutput) << "    jmp " << endLabel << "\n";
            (*gen.mOutput) << label << ":\n";
            (*gen.mOutput) << "    mov rax, 1\n";
            (*gen.mOutput) << endLabel << ":\n";
            gen.push("rax");
        }
    };

    try {
        std::visit(Visitor(*this), node->node);
    } catch (const std::bad_variant_access& e) {
        // Should never happen
        auto& logger = utils::getLogger();
        logger.error("Generator encountered bad variant access in \"{}\": {}", __FUNCTION__, e.what());
        exit(EXIT_FAILURE);
    }
}

void Generator::push(const std::string& reg) noexcept
{
    (*mOutput) << "    push " << reg << "\n";
    mStackSize++;
}

void Generator::pop(const std::string& reg) noexcept
{
    (*mOutput) << "    pop " << reg << "\n";
    mStackSize--;
}

void Generator::beginScope() noexcept
{
    mScopes.emplace_back(mVars.size());
}

void Generator::endScope() noexcept
{
    const size_t POP_COUNT = mVars.size() - mScopes.back();
    if (POP_COUNT != 0) {
        (*mOutput) << "    add rsp, " << POP_COUNT * 8 << "\n";
    }
    mStackSize -= POP_COUNT;
    for (size_t i = 0; i < POP_COUNT; i++) {
        mVars.pop_back();
    }
    mScopes.pop_back();
}

std::string Generator::createLabel() noexcept
{
    static size_t sLabelCount{0};

    std::ostringstream oss;
    oss << "label" << std::to_string(sLabelCount++);
    return oss.str();
}

Generator::Var::Var(std::string name, size_t stackLoc)
    : name(std::move(name))
    , stackLoc(stackLoc)
{}

}  // namespace pimento::generation