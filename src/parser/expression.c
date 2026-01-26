#include "parser/expression.h"
#include "ast/ast.h"
#include "lexer.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

struct ast_node *expression() {
    return addition();
}

struct ast_node *addition() {
    struct ast_node *expr = multiplication();

    struct token current_token;
    while (1) {
        if (lexical_scan(&current_token) == -1)
            break;

        if (current_token.type == T_PLUS || current_token.type == T_MINUS) {
            enum TOKEN_TYPE  token_type = current_token.type;
            struct ast_node *left       = expr;
            expr                        = mk_node(AST_BIN_OP, token_type, left, multiplication());

            continue;
        }

        lexical_go_back();
        break;
    }

    return expr;
}

struct ast_node *multiplication() {
    struct ast_node *expr = primary();

    struct token current_token;
    while (1) {
        if (lexical_scan(&current_token) == -1)
            break;

        if (current_token.type == T_STAR || current_token.type == T_SLASH) {
            enum TOKEN_TYPE  token_type = current_token.type;
            struct ast_node *left       = expr;
            expr                        = mk_node(AST_BIN_OP, token_type, left, primary());
            continue;
        }

        lexical_go_back();
        break;
    }

    return expr;
}

struct ast_node *primary() {
    struct ast_node *expr;

    struct token current_token;
    if (lexical_scan(&current_token) == -1) {
        fprintf(stderr, "Unexpected EOF\n");
        exit(1);
    };

    switch (current_token.type) {
    case T_INTLIT:
        expr = mk_leaf(AST_INTEGER, current_token.type, current_token.value);
        break;
    case T_DOUBLELIT:
        expr = mk_leaf(AST_DOUBLE, current_token.type, current_token.value);
        break;
    default:
        printf("Unexpected token: %.*s on line %d\n", (int)current_token.lexeme_length, current_token.lexeme_start, current_token.line);
        exit(1);
        break;
    }

    return expr;
}
