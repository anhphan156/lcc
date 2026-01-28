#include "parser/parser.h"
#include "ast/ast.h"
#include "parser/statement.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

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
    case AST_STMTS_BLOCK:
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
