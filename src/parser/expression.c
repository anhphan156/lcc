#include "parser/expression.h"
#include "ast/ast.h"
#include "lexer/lexer.h"
#include "parser/identifier.h"
#include "parser/utils.h"
#include "token.h"

static struct ast_node *logical_or();
static struct ast_node *logical_and();
static struct ast_node *equality();
static struct ast_node *comparison();
static struct ast_node *addition();
static struct ast_node *multiplication();
static struct ast_node *unary();
static struct ast_node *primary();

struct ast_node *expression() {
    return logical_or();
}

static struct ast_node *logical_or() {
    struct ast_node *expr = logical_and();

    while (match(T_OROR)) {
        struct token     previous_token = get_previous_token();
        struct ast_node *left           = expr;
        expr                            = mk_node(AST_BIN_OP, previous_token.type, left, logical_and());
    }

    return expr;
}

static struct ast_node *logical_and() {
    struct ast_node *expr = equality();

    while (match(T_ANDAND)) {
        struct token     previous_token = get_previous_token();
        struct ast_node *left           = expr;
        expr                            = mk_node(AST_BIN_OP, previous_token.type, left, equality());
    }

    return expr;
}

static struct ast_node *equality() {
    struct ast_node *expr = comparison();

    while (match(T_NOTEQ) || match(T_EQEQ)) {
        struct token     previous_token = get_previous_token();
        struct ast_node *left           = expr;
        expr                            = mk_node(AST_BIN_OP, previous_token.type, left, comparison());
    }

    return expr;
}

static struct ast_node *comparison() {
    struct ast_node *expr = addition();

    while (match(T_LT) || match(T_LE) || match(T_GT) || match(T_GE)) {
        struct token     previous_token = get_previous_token();
        struct ast_node *left           = expr;
        expr                            = mk_node(AST_BIN_OP, previous_token.type, left, addition());
    }

    return expr;
}

static struct ast_node *addition() {
    struct ast_node *expr = multiplication();

    while (match(T_PLUS) || match(T_MINUS)) {
        struct token     previous_token = get_previous_token();
        struct ast_node *left           = expr;
        expr                            = mk_node(AST_BIN_OP, previous_token.type, left, multiplication());
    }

    return expr;
}

static struct ast_node *multiplication() {
    struct ast_node *expr = unary();

    while (match(T_STAR) || match(T_SLASH)) {
        struct token     previous_token = get_previous_token();
        struct ast_node *left           = expr;
        expr                            = mk_node(AST_BIN_OP, previous_token.type, left, unary());
    }

    return expr;
}

static struct ast_node *unary() {
    if (match(T_MINUS) || match(T_NOT)) {
        struct token previous_token = get_previous_token();
        return mk_node(AST_UN_OP, previous_token.type, primary(), NULL);
    }

    match(T_PLUS);
    return primary();
}

static struct ast_node *primary() {
    if (match(T_INTLIT) || match(T_DOUBLELIT)) {
        struct token previous_token = get_previous_token();
        return mk_leaf(AST_LITERAL_NUMBER, previous_token.type, previous_token.value);
    }

    if (get_current_token().type == T_LPAREN) {
        l_paren();
        struct ast_node *expr = expression();
        r_paren();
        return mk_node(AST_GROUPING, T_LPAREN, expr, NULL);
    }

    return parse_identifier();
}
