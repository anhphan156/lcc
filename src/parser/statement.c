#include "parser/statement.h"
#include "ast/ast.h"
#include "lexer.h"
#include "parser/expression.h"
#include "token.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static struct ast_node *statements_block();
static struct ast_node *statement();
static struct ast_node *if_statement();
static struct ast_node *while_statement();
static struct ast_node *print_statement();
static struct ast_node *decl_statement();
static struct ast_node *asgn_statement();

struct ast_node *top_level() {
    struct ast_node *block = statement();

    struct ast_node *stmt;
    while ((stmt = statement()) != NULL) {
        block = mk_node(AST_STMTS_BLOCK, 0, block, stmt);
    }

    return block;
}

static struct ast_node *statements_block() {
    if (!match(T_LBRACE)) {
        fprintf(stderr, "Expected a `{` on line %d\n", get_current_token().line);
        exit(1);
    }

    struct ast_node *block = statement();

    struct ast_node *stmt;
    while (!match(T_RBRACE)) {
        if (match(T_EOF)) {
            fprintf(stderr, "Expected a `}` on line %d\n", get_current_token().line);
            exit(1);
        } else if (match(T_IF)) {
            stmt = if_statement();
        } else if (match(T_WHILE)) {
            stmt = while_statement();
        } else if (match(T_PRINT)) {
            stmt = print_statement();
        } else if (match(T_INT)) {
            stmt = decl_statement();
        } else if (match(T_IDENTIFIER)) {
            stmt = asgn_statement();
        } else {
            struct token current_token = get_current_token();
            printf("Unexpected token: `%.*s` on line %d\n", (int)current_token.lexeme_length, current_token.lexeme_start, current_token.line);
            exit(1);
        }

        block = mk_node(AST_STMTS_BLOCK, 0, block, stmt);
    }

    return block;
}

static struct ast_node *statement() {
    if (match(T_EOF)) {
        return NULL;
    }

    if (match(T_IF)) {
        return if_statement();
    }

    if (match(T_WHILE)) {
        return while_statement();
    }

    if (match(T_PRINT)) {
        return print_statement();
    }

    if (match(T_INT)) {
        return decl_statement();
    }

    if (match(T_IDENTIFIER)) {
        return asgn_statement();
    }

    struct token current_token = get_current_token();
    printf("Unexpected token: `%.*s` on line %d\n", (int)current_token.lexeme_length, current_token.lexeme_start, current_token.line);
    exit(1);
    return NULL;
}

static struct ast_node *while_statement() {
    if (!match(T_LPAREN)) {
        fprintf(stderr, "Expected a `(` on line %d\n", get_current_token().line);
        exit(1);
    }

    struct ast_node *expr = expression();

    if (!match(T_RPAREN)) {
        fprintf(stderr, "Expected a `)` on line %d\n", get_current_token().line);
        exit(1);
    }

    struct ast_node *right = statements_block();

    return mk_node(AST_STMT_WHILE, T_WHILE, expr, right);
}

static struct ast_node *if_statement() {
    if (!match(T_LPAREN)) {
        fprintf(stderr, "Expected a `(` on line %d\n", get_current_token().line);
        exit(1);
    }

    struct ast_node *expr = expression();

    if (!match(T_RPAREN)) {
        fprintf(stderr, "Expected a `)` on line %d\n", get_current_token().line);
        exit(1);
    }

    struct ast_node *right;
    struct ast_node *consequence = statements_block();
    if (match(T_ELSE)) {
        struct ast_node *alternative = statements_block();
        right                        = mk_node(AST_STMT_BRANCH_BODY, T_IF, consequence, alternative);
    } else {
        right = mk_node(AST_STMT_BRANCH_BODY, T_IF, consequence, NULL);
    }

    return mk_node(AST_STMT_BRANCH, T_IF, expr, right);
}

static struct ast_node *print_statement() {
    struct ast_node *stmt = mk_node(AST_STMT, T_PRINT, expression(), NULL);

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
