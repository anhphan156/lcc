#include "parser/parser.h"
#include "ast/ast.h"
#include "parser/statement.h"
#include <stdlib.h>

struct ast_node *ast_parse() {
    return top_level();
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
