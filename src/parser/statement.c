#include "parser/statement.h"
#include "ast/ast.h"
#include "lexer.h"
#include "parser/expression.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

static struct ast_node *statement();
static struct ast_node *print_statement();
static struct ast_node *decl_statement();
static struct ast_node *asgn_statement();

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

    if (match(T_INT)) {
        return decl_statement();
    }

    if (match(T_IDENTIFIER)) {
        return asgn_statement();
    }

    fprintf(stderr, "Unexpected statement\n");

    return NULL;
}

static struct ast_node *print_statement() {
    struct ast_node *stmt = mk_node(AST_STMT, T_SCRIBE, expression(), NULL);

    if (!match(T_SEMICOLON)) {
        fprintf(stderr, "Expected a `;` on line %d\n", get_previous_token().line);
        exit(1);
    }

    return stmt;
}

static struct ast_node *decl_statement() {
    if (!match(T_IDENTIFIER)) {
        fprintf(stderr, "Expected an identifier on line %d\n", get_current_token().line);
        exit(1);
    }

    struct token previous_token = get_previous_token();

    if (!match(T_SEMICOLON)) {
        fprintf(stderr, "Expected a `;` on line %d\n", get_previous_token().line);
        exit(1);
    }

    return mk_leaf(AST_DECL, T_INT, previous_token.value);
}

static struct ast_node *asgn_statement() {
    struct token     previous_token = get_previous_token();
    struct ast_node *right          = mk_leaf(AST_LVALUE, T_IDENTIFIER, previous_token.value);

    if (!match(T_EQ)) {
        fprintf(stderr, "Expected a `=` on line %d\n", get_previous_token().line);
        exit(1);
    }

    struct ast_node *left = expression();
    struct ast_node *asgn = mk_node(AST_STMT, T_EQ, left, right);

    if (!match(T_SEMICOLON)) {
        fprintf(stderr, "Expected a `;` on line %d\n", get_previous_token().line);
        exit(1);
    }

    return asgn;
}
