#include "parser/statement.h"
#include "ast/ast.h"
#include "lexer.h"
#include "parser/expression.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

static struct ast_node *statement();
static struct ast_node *print_statement();

struct ast_node *statements_block() {
    struct ast_node *block = statement();

    struct ast_node *stmt;
    while ((stmt = statement()) != NULL) {
        block = mk_node(AST_STMTS_BLOCK, 0, block, stmt);
    }

    return block;
}

static struct ast_node *statement() {
    if (match(T_EOF)) {
        return NULL;
    }

    if (match(T_SCRIBE)) {
        return print_statement();
    }

    fprintf(stderr, "Unexpected statement\n");

    return NULL;
}

static struct ast_node *print_statement() {
    struct ast_node *stmt = mk_node(AST_STMT, T_SCRIBE, expression(), NULL);

    if (!match(T_SEMICOLON)) {
        struct token current_token = get_current_token();
        fprintf(stderr, "Expected a `;` on line %d\n", current_token.line);
        exit(1);
    }

    return stmt;
}
