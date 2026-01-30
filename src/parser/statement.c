#include "parser/statement.h"
#include "ast/ast.h"
#include "lexer.h"
#include "parser/expression.h"
#include "parser/utils.h"
#include "token.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static struct ast_node *statement();
static struct ast_node *if_statement();
static struct ast_node *for_statement();
static struct ast_node *while_statement();
static struct ast_node *print_statement();
static struct ast_node *decl_statement();
static struct ast_node *asgn_statement();

struct ast_node *statements_block() {
    if (!match(T_LBRACE)) {
        fprintf(stderr, "Expected a `{` on line %d\n", get_current_token().line);
        exit(1);
    }

    struct ast_node *block = statement();

    struct ast_node *stmt;
    while ((stmt = statement()) != NULL) {
        block = mk_node(AST_STMTS_BLOCK, 0, block, stmt);
    }

    if (!match(T_RBRACE)) {
        fprintf(stderr, "Expected a `}` on line %d\n", get_current_token().line);
        exit(1);
    }

    return block;
}

static struct ast_node *statement() {
    struct token     current_token = get_current_token();
    struct ast_node *stmt          = NULL;

    if (match(T_EOF)) {
        return NULL;
    }

    if (current_token.type == T_IF) {
        return if_statement();
    }

    if (current_token.type == T_FOR) {
        return for_statement();
    }

    if (current_token.type == T_WHILE) {
        return while_statement();
    }

    if (current_token.type == T_PRINT) {
        stmt = print_statement();
        semicolon();
        return stmt;
    }

    if (current_token.type == T_INT) {
        stmt = decl_statement();
        semicolon();
        return stmt;
    }

    if (current_token.type == T_IDENTIFIER) {
        stmt = asgn_statement();
        semicolon();
        return stmt;
    }

    if (current_token.type == T_RBRACE) {
        return NULL;
    }

    printf("Unexpected token: `%.*s` on line %d\n", (int)current_token.lexeme_length, current_token.lexeme_start, current_token.line);
    exit(1);

    return NULL;
}

static struct ast_node *for_statement() {
    match(T_FOR);
    l_paren();
    struct ast_node *initialization = asgn_statement();
    semicolon();
    struct ast_node *predicate = expression();
    semicolon();
    struct ast_node *increment = asgn_statement();
    r_paren();

    struct ast_node *for_body = statements_block();
    for_body                  = mk_node(AST_STMTS_BLOCK, 0, for_body, increment);

    struct ast_node *for_stmt = mk_node(AST_STMT, T_WHILE, predicate, for_body);
    return mk_node(AST_STMT, T_FOR, initialization, for_stmt);
}

static struct ast_node *while_statement() {
    match(T_WHILE);
    l_paren();
    struct ast_node *predicate = expression();
    r_paren();
    struct ast_node *body = statements_block();

    return mk_node(AST_STMT, T_WHILE, predicate, body);
}

static struct ast_node *if_statement() {
    match(T_IF);
    l_paren();
    struct ast_node *predicate = expression();
    r_paren();

    struct ast_node *body;
    struct ast_node *consequence = statements_block();
    if (match(T_ELSE)) {
        struct ast_node *alternative = statements_block();
        body                         = mk_node(AST_STMTS_BLOCK, T_IF, consequence, alternative);
    } else {
        body = mk_node(AST_STMTS_BLOCK, T_IF, consequence, NULL);
    }

    return mk_node(AST_STMT, T_IF, predicate, body);
}

static struct ast_node *print_statement() {
    match(T_PRINT);
    return mk_node(AST_STMT, T_PRINT, expression(), NULL);
}

static struct ast_node *decl_statement() {
    match(T_INT);
    if (!match(T_IDENTIFIER)) {
        fprintf(stderr, "Expected an identifier on line %d\n", get_current_token().line);
        exit(1);
    }

    return mk_leaf(AST_DECL, T_INT, get_current_token().value);
}

static struct ast_node *asgn_statement() {
    match(T_IDENTIFIER);
    struct token     previous_token = get_previous_token();
    struct ast_node *right          = mk_leaf(AST_LVALUE, T_IDENTIFIER, previous_token.value);

    if (!match(T_EQ)) {
        fprintf(stderr, "Expected a `=` on line %d\n", get_previous_token().line);
        exit(1);
    }

    struct ast_node *left = expression();
    return mk_node(AST_STMT, T_EQ, left, right);
}
