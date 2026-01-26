#include "parser/expression.h"
#include "ast/ast.h"
#include "lexer.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

static struct ast_node *addition();
static struct ast_node *multiplication();
static struct ast_node *primary();

struct ast_node *expression() {
    return addition();
}

static struct ast_node *addition() {
    struct ast_node *expr = multiplication();

    while (match(T_PLUS) || match(T_MINUS)) {
        struct token     current_token = get_current_token();
        struct ast_node *left          = expr;
        expr                           = mk_node(AST_BIN_OP, current_token.type, left, multiplication());
    }

    return expr;
}

static struct ast_node *multiplication() {
    struct ast_node *expr = primary();

    while (match(T_STAR) || match(T_SLASH)) {
        struct token     current_token = get_current_token();
        struct ast_node *left          = expr;
        expr                           = mk_node(AST_BIN_OP, current_token.type, left, primary());
    }

    return expr;
}

static struct ast_node *primary() {
    struct token current_token = get_current_token();

    if (match(T_INTLIT)) {
        current_token = get_current_token();
        return mk_leaf(AST_INTEGER, current_token.type, current_token.value);
    }

    if (match(T_DOUBLELIT)) {
        current_token = get_current_token();
        return mk_leaf(AST_DOUBLE, current_token.type, current_token.value);
    }

    printf("Unexpected token: %.*s on line %d\n", (int)current_token.lexeme_length, current_token.lexeme_start, current_token.line);
    exit(1);
    return NULL;
}
