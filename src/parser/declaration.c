#include "parser/declaration.h"
#include "ast/ast.h"
#include "lexer.h"
#include "parser/statement.h"
#include "parser/utils.h"
#include <stdio.h>
#include <stdlib.h>

static struct ast_node *function_definition();

struct ast_node *declaration() {
    struct token     current_token = get_current_token();
    struct ast_node *decl          = NULL;

    if (match(T_EOF)) {
        return NULL;
    }

    if (current_token.type == T_VOID) {
        return function_definition();
    }

    if (current_token.type == T_INT) {
        decl = variable_declaration();
        semicolon();
        return decl;
    }

    printf("Unexpected token: `%.*s` on line %d\n", (int)current_token.lexeme_length, current_token.lexeme_start, current_token.line);
    exit(1);
    return NULL;
}

static struct ast_node *function_definition() {
    match(T_VOID);
    if (!match(T_IDENTIFIER)) {
        fprintf(stderr, "aa");
        exit(1);
    }
    struct ast_node *name = mk_leaf(AST_DECL, T_VOID, get_previous_token().value);
    l_paren();
    r_paren();

    struct ast_node *body = statements_block();
    return mk_node(AST_FUNC, 0, name, body);
}

struct ast_node *variable_declaration() {
    match(T_INT);
    if (!match(T_IDENTIFIER)) {
        fprintf(stderr, "Expected an identifier on line %d\n", get_current_token().line);
        exit(1);
    }

    return mk_leaf(AST_DECL, T_INT, get_current_token().value);
}
