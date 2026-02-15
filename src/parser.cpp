#include <magic_enum/magic_enum.hpp>

#include <pimento/parser.hpp>
#include <pimento/utils.hpp>

namespace pimento::ast {

Parser::Parser(std::istream* istream)
    : mLexer(istream)
{
    mLexer.tokenize();
    parse();
}

[[nodiscard]] const node::ProgNode& Parser::getProgram() const noexcept
{
    return mProg;
}

void Parser::parse()
{
    while (peek().has_value()) {
        std::shared_ptr<node::StmtNode> const stmt = parseStatement();
        mProg.statements.push_back(stmt);
    }
}

[[nodiscard]] inline std::optional<tokenization::Token> Parser::peek(size_t lookahead) const noexcept
{
    /// @brief The tokens to parse.
    const std::vector<tokenization::Token> M_TOKENS = mLexer.tokens();
    try {
        return M_TOKENS.at(mIndex + lookahead);
    } catch (const std::out_of_range&) {
        return {};
    }
}

inline void Parser::advance() noexcept
{
    ++mIndex;
}

[[nodiscard]] inline std::optional<tokenization::Token> Parser::tryConsume() noexcept
{
    if (auto ret = peek()) {
        advance();
        return ret;
    }
    return {};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
inline tokenization::Token Parser::tryConsume(tokenization::TokenType tokenType) noexcept
{
    auto next = tryConsume();

    if (!next.has_value()) {
        auto& logger = utils::getLogger();
        logger.error("nexpected EOF in \"{}\"", __FUNCTION__);
        exit(EXIT_FAILURE);
    }

    if (next.value().tokenType != tokenType) {
        // TODO(lthomas): I don't know if I want to log and exit here
        auto& logger = utils::getLogger();
        logger.error("Token mismatch in \"{}\": expected {}, got {}",
                     __FUNCTION__,
                     magic_enum::enum_name(tokenType),
                     magic_enum::enum_name(next.value().tokenType));
        exit(EXIT_FAILURE);
    }

    return next.value();
}

std::shared_ptr<node::StmtNode> Parser::parseStatement()
{
    auto& logger = utils::getLogger();

    std::shared_ptr<node::StmtNode> stmt;
    tokenization::Token currentToken;

    if (auto tokenOpt = peek()) {
        currentToken = tokenOpt.value();
    }
    else {
        logger.error("Expected statement at TODO Line & Column number");
        exit(EXIT_FAILURE);
    }

    switch (currentToken.tokenType) {
        // Parse format exit([Expr]);
        case tokenization::TokenType::TT_EXIT: {
            tryConsume(tokenization::TokenType::TT_EXIT);
            tryConsume(tokenization::TokenType::TT_LEFT_PAREN);

            std::shared_ptr<node::ExprNode> const expression = parseExpression();

            tryConsume(tokenization::TokenType::TT_RIGHT_PAREN);
            tryConsume(tokenization::TokenType::TT_SEMI);

            stmt = std::make_shared<node::StmtNode>(std::make_shared<node::StmtExitNode>(expression));
            break;
        }
        // Parse format let ident = [Expr];
        // case tokenization::TokenType::TT_LET: {
        //   tryConsume(tokenization::TokenType::TT_LET);
        //   tokenization::Token identifier =
        //       tryConsume(tokenization::TokenType::TT_IDENTIFIER);

        //   tryConsume(tokenization::TokenType::TT_EQUAL);

        //   std::shared_ptr<node::ExprNode> expression = parseExpression();

        //   tryConsume(tokenization::TokenType::TT_SEMI);

        //   stmt = std::make_shared<node::StmtNode>(
        //       std::make_shared<node::StmtLetNode>(identifier, expression));
        //   break;
        // }
        // Parse format ident = [Expr];
        case tokenization::TokenType::TT_IDENT: {
            tryConsume(tokenization::TokenType::TT_IDENT);
            // tryConsume(tokenization::TokenType::TT_EQUAL);

            std::shared_ptr<node::ExprNode> const expression = parseExpression();

            tryConsume(tokenization::TokenType::TT_SEMI);

            stmt = std::make_shared<node::StmtNode>(std::make_shared<node::StmtAssignNode>(currentToken, expression));
            break;
        }
        // Parse format if [Expr] [Scope] [IfPred]
        case tokenization::TokenType::TT_IF: {
            tryConsume(tokenization::TokenType::TT_IF);

            std::shared_ptr<node::ExprNode> const expression = parseExpression();

            std::shared_ptr<node::ScopeNode> const scope = parseScope();

            std::optional<std::shared_ptr<node::IfPredNode>> const ifpred = parseIfPred();

            stmt = std::make_shared<node::StmtNode>(std::make_shared<node::StmtIfNode>(expression, scope, ifpred));

            break;
        }
        // Parse format while [Expr] [Scope]
        case tokenization::TokenType::TT_WHILE: {
            tryConsume(tokenization::TokenType::TT_WHILE);

            std::shared_ptr<node::ExprNode> const expression = parseExpression();

            std::shared_ptr<node::ScopeNode> const scope = parseScope();

            stmt = std::make_shared<node::StmtNode>(std::make_shared<node::StmtWhileNode>(expression, scope));

            break;
        }
        // Parse format {[Stmt]*}
        case tokenization::TokenType::TT_LEFT_CURLY: {
            stmt = std::make_shared<node::StmtNode>(parseScope());
            break;
        }
        default: {
            logger.error("Expected statement at TODO Line & Column number");
            exit(EXIT_FAILURE);
        }
    }
    return stmt;
}

std::shared_ptr<node::ExprNode> Parser::parseExpression(uint8_t /*minPrecedence*/)
{
    auto& logger = utils::getLogger();
    std::shared_ptr<node::ExprNode> expr;

    std::shared_ptr<node::TermNode> termLhs = parseTerm();

    while (std::optional<tokenization::Token> next = peek()) {
        const tokenization::Token& currentToken = next.value();

        // TODO(lthomas): I don't like this logic here. Lots of repetitive code
        switch (currentToken.tokenType) {
            // case tokenization::TokenType::TT_DOUBLE_CARET: {
            //   tryConsume(tokenization::TokenType::TT_DOUBLE_CARET);
            //   std::pair<uint8_t, tokenization::BinOpProperties::Associativity>
            //       properties = tokenization::TokenTypeUtil::get_bin_expr_properties(
            //           current_token.tokenType);

            //   if (properties.first < minPrecedence) {
            //     expr = std::make_shared<node::ExprNode>(term_lhs);
            //   } else {
            //     uint8_t next_prec =
            //         properties.second ==
            //                 tokenization::BinOpProperties::Associativity::LEFT
            //             ? properties.first + 1
            //             : properties.first;

            //     std::shared_ptr<node::ExprNode> lhs =
            //         std::make_shared<node::ExprNode>(term_lhs);
            //     std::shared_ptr<node::BinExprPowerNode> bin_expr =
            //         std::make_shared<node::BinExprPowerNode>(
            //             lhs, parseExpression(next_prec));
            //     expr = std::make_shared<node::ExprNode>(
            //         std::make_shared<node::BinExprNode>(bin_expr));
            //   }
            //   break;
            // }
            // case tokenization::TokenType::TT_PERCENT: {
            //   tryConsume(tokenization::TokenType::TT_PERCENT);
            //   std::pair<uint8_t, tokenization::BinOpProperties::Associativity>
            //       properties = tokenization::TokenTypeUtil::get_bin_expr_properties(
            //           current_token.tokenType);

            //   if (properties.first < minPrecedence) {
            //     expr = std::make_shared<node::ExprNode>(term_lhs);
            //   } else {
            //     uint8_t next_prec =
            //         properties.second ==
            //                 tokenization::BinOpProperties::Associativity::LEFT
            //             ? properties.first + 1
            //             : properties.first;

            //     std::shared_ptr<node::ExprNode> lhs =
            //         std::make_shared<node::ExprNode>(term_lhs);
            //     std::shared_ptr<node::BinExprModNode> bin_expr =
            //         std::make_shared<node::BinExprModNode>(lhs,
            //                                                parseExpression(next_prec));
            //     expr = std::make_shared<node::ExprNode>(
            //         std::make_shared<node::BinExprNode>(bin_expr));
            //   }
            //   break;
            // }
            // case tokenization::TokenType::TT_STAR: {
            //   tryConsume(tokenization::TokenType::TT_STAR);
            //   std::pair<uint8_t, tokenization::BinOpProperties::Associativity>
            //       properties = tokenization::TokenTypeUtil::get_bin_expr_properties(
            //           current_token.tokenType);

            //   if (properties.first < minPrecedence) {
            //     expr = std::make_shared<node::ExprNode>(term_lhs);
            //   } else {
            //     uint8_t next_prec =
            //         properties.second ==
            //                 tokenization::BinOpProperties::Associativity::LEFT
            //             ? properties.first + 1
            //             : properties.first;

            //     std::shared_ptr<node::ExprNode> lhs =
            //         std::make_shared<node::ExprNode>(term_lhs);
            //     std::shared_ptr<node::BinExprMulNode> bin_expr =
            //         std::make_shared<node::BinExprMulNode>(lhs,
            //                                                parseExpression(next_prec));
            //     expr = std::make_shared<node::ExprNode>(
            //         std::make_shared<node::BinExprNode>(bin_expr));
            //   }
            //   break;
            // }
            // case tokenization::TokenType::TT_FORWARD_SLASH: {
            //   tryConsume(tokenization::TokenType::TT_FORWARD_SLASH);
            //   std::pair<uint8_t, tokenization::BinOpProperties::Associativity>
            //       properties = tokenization::TokenTypeUtil::get_bin_expr_properties(
            //           current_token.tokenType);

            //   if (properties.first < minPrecedence) {
            //     expr = std::make_shared<node::ExprNode>(term_lhs);
            //   } else {
            //     uint8_t next_prec =
            //         properties.second ==
            //                 tokenization::BinOpProperties::Associativity::LEFT
            //             ? properties.first + 1
            //             : properties.first;

            //     std::shared_ptr<node::ExprNode> lhs =
            //         std::make_shared<node::ExprNode>(term_lhs);
            //     std::shared_ptr<node::BinExprDivNode> bin_expr =
            //         std::make_shared<node::BinExprDivNode>(lhs,
            //                                                parseExpression(next_prec));
            //     expr = std::make_shared<node::ExprNode>(
            //         std::make_shared<node::BinExprNode>(bin_expr));
            //   }
            //   break;
            // }
            // case tokenization::TokenType::TT_PLUS: {
            //   tryConsume(tokenization::TokenType::TT_PLUS);
            //   std::pair<uint8_t, tokenization::BinOpProperties::Associativity>
            //       properties = tokenization::TokenTypeUtil::get_bin_expr_properties(
            //           current_token.tokenType);

            //   if (properties.first < minPrecedence) {
            //     expr = std::make_shared<node::ExprNode>(term_lhs);
            //   } else {
            //     uint8_t next_prec =
            //         properties.second ==
            //                 tokenization::BinOpProperties::Associativity::LEFT
            //             ? properties.first + 1
            //             : properties.first;

            //     std::shared_ptr<node::ExprNode> lhs =
            //         std::make_shared<node::ExprNode>(term_lhs);
            //     std::shared_ptr<node::BinExprPlusNode> bin_expr =
            //         std::make_shared<node::BinExprPlusNode>(
            //             lhs, parseExpression(next_prec));
            //     expr = std::make_shared<node::ExprNode>(
            //         std::make_shared<node::BinExprNode>(bin_expr));
            //   }
            //   break;
            // }
            // case tokenization::TokenType::TT_MINUS: {
            //   tryConsume(tokenization::TokenType::TT_MINUS);
            //   std::pair<uint8_t, tokenization::BinOpProperties::Associativity>
            //       properties = tokenization::TokenTypeUtil::get_bin_expr_properties(
            //           current_token.tokenType);

            //   if (properties.first < minPrecedence) {
            //     expr = std::make_shared<node::ExprNode>(term_lhs);
            //   } else {
            //     uint8_t next_prec =
            //         properties.second ==
            //                 tokenization::BinOpProperties::Associativity::LEFT
            //             ? properties.first + 1
            //             : properties.first;

            //     std::shared_ptr<node::ExprNode> lhs =
            //         std::make_shared<node::ExprNode>(term_lhs);
            //     std::shared_ptr<node::BinExprMinusNode> bin_expr =
            //         std::make_shared<node::BinExprMinusNode>(
            //             lhs, parseExpression(next_prec));
            //     expr = std::make_shared<node::ExprNode>(
            //         std::make_shared<node::BinExprNode>(bin_expr));
            //   }
            //   break;
            // }
            // case tokenization::TokenType::TT_LT: {
            //   tryConsume(tokenization::TokenType::TT_LT);
            //   std::pair<uint8_t, tokenization::BinOpProperties::Associativity>
            //       properties = tokenization::TokenTypeUtil::get_bin_expr_properties(
            //           current_token.tokenType);

            //   if (properties.first < minPrecedence) {
            //     expr = std::make_shared<node::ExprNode>(term_lhs);
            //   } else {
            //     uint8_t next_prec =
            //         properties.second ==
            //                 tokenization::BinOpProperties::Associativity::LEFT
            //             ? properties.first + 1
            //             : properties.first;

            //     std::shared_ptr<node::ExprNode> lhs =
            //         std::make_shared<node::ExprNode>(term_lhs);
            //     std::shared_ptr<node::BinExprLessThanNode> bin_expr =
            //         std::make_shared<node::BinExprLessThanNode>(
            //             lhs, parseExpression(next_prec));
            //     expr = std::make_shared<node::ExprNode>(
            //         std::make_shared<node::BinExprNode>(bin_expr));
            //   }
            //   break;
            // }
            // case tokenization::TokenType::TT_GT: {
            //   tryConsume(tokenization::TokenType::TT_GT);
            //   std::pair<uint8_t, tokenization::BinOpProperties::Associativity>
            //       properties = tokenization::TokenTypeUtil::get_bin_expr_properties(
            //           current_token.tokenType);

            //   if (properties.first < minPrecedence) {
            //     expr = std::make_shared<node::ExprNode>(term_lhs);
            //   } else {
            //     uint8_t next_prec =
            //         properties.second ==
            //                 tokenization::BinOpProperties::Associativity::LEFT
            //             ? properties.first + 1
            //             : properties.first;

            //     std::shared_ptr<node::ExprNode> lhs =
            //         std::make_shared<node::ExprNode>(term_lhs);
            //     std::shared_ptr<node::BinExprGreaterThanNode> bin_expr =
            //         std::make_shared<node::BinExprGreaterThanNode>(
            //             lhs, parseExpression(next_prec));
            //     expr = std::make_shared<node::ExprNode>(
            //         std::make_shared<node::BinExprNode>(bin_expr));
            //   }
            //   break;
            // }
            default: {
                expr = std::make_shared<node::ExprNode>(termLhs);
            }
        }

        return expr;
    }

    logger.error("Expected expression at TODO Line & Column number");
    exit(EXIT_FAILURE);
}

std::shared_ptr<node::ScopeNode> Parser::parseScope()
{
    tryConsume(tokenization::TokenType::TT_LEFT_CURLY);

    std::shared_ptr<node::ScopeNode> scope = std::make_shared<node::ScopeNode>();
    while (true) {
        std::optional<tokenization::Token> next = peek();
        if (!next.has_value() || next.value().tokenType == tokenization::TokenType::TT_RIGHT_CURLY) {
            break;
        }

        std::shared_ptr<node::StmtNode> const statement = parseStatement();
        scope->statements.push_back(statement);
    }

    tryConsume(tokenization::TokenType::TT_RIGHT_CURLY);
    return scope;
}

std::optional<std::shared_ptr<node::IfPredNode>> Parser::parseIfPred()
{
    std::optional<std::shared_ptr<node::IfPredNode>> ifpred;
    tokenization::Token currentToken;

    if (auto tokenOpt = peek()) {
        currentToken = tokenOpt.value();
    }
    else {
        return {};
    }

    switch (currentToken.tokenType) {
        // Parse format ident;
        case tokenization::TokenType::TT_ELIF: {
            tryConsume(tokenization::TokenType::TT_ELIF);
            std::shared_ptr<node::ExprNode> const expression                  = parseExpression();
            std::shared_ptr<node::ScopeNode> const scope                      = parseScope();
            std::optional<std::shared_ptr<node::IfPredNode>> const ifPredElif = parseIfPred();
            ifpred                                                            = std::make_shared<node::IfPredNode>(
                std::make_shared<node::IfPredElifNode>(expression, scope, ifPredElif));
            break;
        }
        // Parse format int_lit
        case tokenization::TokenType::TT_ELSE: {
            tryConsume(tokenization::TokenType::TT_ELSE);
            std::shared_ptr<node::ScopeNode> const scope = parseScope();
            ifpred = std::make_shared<node::IfPredNode>(std::make_shared<node::IfPredElseNode>(scope));
            break;
        }
        default:
            ifpred = {};
    }

    return ifpred;
}

std::shared_ptr<node::TermNode> Parser::parseTerm()
{
    auto& logger = utils::getLogger();
    std::shared_ptr<node::TermNode> term;
    tokenization::Token currentToken;

    if (auto tokenOpt = tryConsume()) {
        currentToken = tokenOpt.value();
    }
    else {
        logger.error("Expected term at TODO Line & Column number");
        exit(EXIT_FAILURE);
    }

    switch (currentToken.tokenType) {
        // Parse format ident;
        case tokenization::TokenType::TT_IDENT: {
            term = std::make_shared<node::TermNode>(std::make_shared<node::TermIdentNode>(currentToken));
            break;
        }
        // Parse format int_lit
        // case tokenization::TokenType::TT_INT_LITERAL: {
        //   term = std::make_shared<node::TermNode>(
        //       std::make_shared<node::TermIntLitNode>(current_token));
        //   break;
        // }
        // Parse format ident = ([Expr]);
        case tokenization::TokenType::TT_LEFT_PAREN: {
            std::shared_ptr<node::ExprNode> const expression = parseExpression();
            tryConsume(tokenization::TokenType::TT_RIGHT_PAREN);

            term = std::make_shared<node::TermNode>(std::make_shared<node::TermExprNode>(expression));
            break;
        }
        default:
            logger.error("Expected term at TODO Line & Column number");
            exit(EXIT_FAILURE);
    }

    return term;
}

}  // namespace pimento::ast