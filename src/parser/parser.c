#include "parser/parser.h"
#include "ast/ast.h"
#include "parser/statement.h"
#include "parser/declaration.h"
#include <stdio.h>
#include <stdlib.h>

static AstArray ast_array;

AstArray *ast_parse() {
    ast_array.cap  = 16;
    ast_array.len  = 0;
    ast_array.item = (struct ast_node **)malloc(sizeof(struct ast_node *) * ast_array.cap);
    if (!ast_array.item) {
        fprintf(stderr, "Failed to allocate memory for ast_array\n");
        exit(1);
    }

    struct ast_node *decl = NULL;
    while ((decl = top_level_declaration()) != NULL) {
        if (ast_array.len >= ast_array.cap) {
            ast_array.cap *= 2;
            ast_array.item = realloc(ast_array.item, ast_array.cap);
        }
        ast_array.item[ast_array.len] = decl;
        ast_array.len += 1;
    }

    return &ast_array;
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
