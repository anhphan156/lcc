#include "parser.h"
#include "ast.h"
#include "breakpoint.h"
#include "lexer.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

static struct token current_token;

static struct ast_node *statement();
static struct ast_node *statements_block();
static struct ast_node *expression();
static struct ast_node *addition();
static struct ast_node *multiplication();
static struct ast_node *primary();

static struct ast_node *mk_node(enum AST_NODE_TYPE, enum TOKEN_TYPE, struct ast_node *, struct ast_node *);
static struct ast_node *mk_leaf(enum AST_NODE_TYPE, enum TOKEN_TYPE, union token_literal);

struct ast_node *ast_parse() {
    return statements_block();
}

void ast_clean(struct ast_node *expr) {
    if (expr == NULL) {
        return;
    }

    struct ast_node *left  = expr->left;
    struct ast_node *right = expr->right;
    free(expr);

    if (left)
        ast_clean(left);

    if (right)
        ast_clean(right);
}

void ast_print(struct ast_node *expr, int level) {
    switch (expr->ast_node_type) {
    case AST_GLUE:
        ast_print(expr->left, level);
        printf("\n");
        ast_print(expr->right, level);
        break;
    case AST_STMT:
        printf("[AST_STMT: ");
        if (expr->token_type == T_SCRIBE)
            printf("(PRINT)");

        printf("\n");
        for (int i = 0; i < level; i++)
            printf("\t");
        ast_print(expr->left, level + 1);

        printf("\n");
        for (int i = 1; i < level; i++)
            printf("\t");
        printf("]");
        break;
    case AST_BIN_OP:
        printf("[AST_BIN_OP: ");
        if (expr->token_type == T_PLUS)
            printf("(+)");
        if (expr->token_type == T_MINUS)
            printf("(-)");
        if (expr->token_type == T_STAR)
            printf("(*)");
        if (expr->token_type == T_SLASH)
            printf("(/)");

        printf("\n");
        for (int i = 0; i < level; i++)
            printf("\t");
        ast_print(expr->left, level + 1);

        printf("\n");
        for (int i = 0; i < level; i++)
            printf("\t");
        ast_print(expr->right, level + 1);

        printf("\n");
        for (int i = 1; i < level; i++)
            printf("\t");
        printf("]");
        break;
    case AST_INTEGER:
        printf("[AST_NUMBER (%ld)]", expr->value.intval);
        break;
    case AST_DOUBLE:
        printf("[AST_NUMBER (%f)]", expr->value.doubleval);
        break;
    default:
        printf("PRINT_AST: not implemented node\n");
        break;
    }
}

static struct ast_node *statements_block() {
    struct ast_node *glue = statement();

    struct ast_node *stmt;
    while ((stmt = statement()) != NULL) {
        glue = mk_node(AST_GLUE, 0, glue, stmt);
    }

    return glue;
}

static struct ast_node *statement() {
    if (lexical_scan(&current_token) == -1) {
        fprintf(stderr, "Expected a statement on line %d\n", current_token.line);
        return NULL;
    }
    int              current_line = current_token.line;
    struct ast_node *stmt         = NULL;

    switch (current_token.type) {
    case T_SCRIBE: {
        stmt = mk_node(AST_STMT, T_SCRIBE, expression(), NULL);

        if (lexical_scan(&current_token) == -1) {
            fprintf(stderr, "Expected a `;` on line %d\n", current_line);
            exit(1);
        }

        if (current_token.type != T_SEMICOLON) {
            fprintf(stderr, "Expected a `;` on line %d\n", current_line);
            exit(1);
        }

        break;
    }
    default:
        fprintf(stderr, "Unexpected token on line %d\n", current_token.line);
        exit(1);
        break;
    }

    return stmt;
}

static struct ast_node *expression() {
    return addition();
}

static struct ast_node *addition() {
    struct ast_node *expr = multiplication();

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

static struct ast_node *multiplication() {
    struct ast_node *expr = primary();

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

static struct ast_node *primary() {
    struct ast_node *expr;

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

static struct ast_node *mk_node(enum AST_NODE_TYPE ast_node_type, enum TOKEN_TYPE token_type, struct ast_node *left, struct ast_node *right) {
    struct ast_node *node = (struct ast_node *)malloc(sizeof(struct ast_node));
    if (node == NULL) {
        fprintf(stderr, "Unable to create new ast_node");
        BREAKPOINT;
        exit(1);
    }
    node->ast_node_type = ast_node_type;
    node->token_type    = token_type;
    node->left          = left;
    node->right         = right;

    union token_literal empty_literal = {0};
    node->value                       = empty_literal;

    return node;
}
static struct ast_node *mk_leaf(enum AST_NODE_TYPE type, enum TOKEN_TYPE token_type, union token_literal value) {
    struct ast_node *node = (struct ast_node *)malloc(sizeof(struct ast_node));
    if (node == NULL) {
        fprintf(stderr, "Unable to create new ast_node");
        BREAKPOINT;
        exit(1);
    }
    node->ast_node_type = type;
    node->token_type    = token_type;
    node->value         = value;
    node->left          = NULL;
    node->right         = NULL;

    return node;
}
