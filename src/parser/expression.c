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
        struct token     previous_token = get_previous_token();
        struct ast_node *left           = expr;
        expr                            = mk_node(AST_BIN_OP, previous_token.type, left, multiplication());
    }

    return expr;
}

static struct ast_node *multiplication() {
    struct ast_node *expr = primary();

    while (match(T_STAR) || match(T_SLASH)) {
        struct token     previous_token = get_previous_token();
        struct ast_node *left           = expr;
        expr                            = mk_node(AST_BIN_OP, previous_token.type, left, primary());
    }

    return expr;
}

static struct ast_node *primary() {

    struct token previous_token;
    if (match(T_INTLIT)) {
        previous_token = get_previous_token();
        return mk_leaf(AST_INTEGER, previous_token.type, previous_token.value);
    }

    if (match(T_DOUBLELIT)) {
        previous_token = get_previous_token();
        return mk_leaf(AST_DOUBLE, previous_token.type, previous_token.value);
    }

    if (match(T_IDENTIFIER)) {
        previous_token = get_previous_token();
        return mk_leaf(AST_IDENTIFIER, previous_token.type, previous_token.value);
    }

    struct token current_token = get_current_token();
    printf("Unexpected token: %.*s on line %d\n", (int)current_token.lexeme_length, current_token.lexeme_start, current_token.line);
    exit(1);
    return NULL;
}
