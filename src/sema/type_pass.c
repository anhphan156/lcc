#include "sema/type_pass.h"
#include "ast/ast.h"
#include "symbol_table.h"
#include <stdio.h>

static bool tree_walker(struct ast_node *node);

bool type_validator(struct ast_node *node) {
    return tree_walker(node);
}

static bool tree_walker(struct ast_node *node) {
    bool is_type_valid = true;

    if (node->left)
        is_type_valid = tree_walker(node->left);

    if (node->right)
        is_type_valid = tree_walker(node->right);

    if (node->ast_node_type == AST_FUNC_CALL) {
        enum STRUCTURE_TYPE symbol_stype = get_symbol_stype(node->value.id);
        if (symbol_stype == -1) {
            fprintf(stderr, "Prepass failed: symbol not found\n");
            return false;
        }

        const char *symbol_name = get_symbol_name(node->value.id);
        if (symbol_stype != ST_FUNCTION) {
            fprintf(stderr, "Prepass failed: identifier `%s` is not a function\n", symbol_name);
            return false;
        }

        return true;
    }

    return is_type_valid;
}
