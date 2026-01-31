#include "sema/type_pass.h"
#include "ast/ast.h"
#include "defs.h"
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

    if (node->ast_node_type == AST_LITERAL_NUMBER) {
        if (node->token_type == T_INTLIT) {
            if (node->value.intval < 256) {
                node->data_type = ET_CHAR;
            }

            if (node->value.intval < 2147483648) {
                node->data_type = ET_INT;
            }

            node->data_type = ET_LONG;
        }
    }

    if (node->ast_node_type == AST_FUNC_CALL) {
        const char *symbol_name = get_symbol_name(node->value.id);
        if (!symbol_name) {
            fprintf(stderr, "Prepass failed: symbol not found\n");
            return false;
        }

        enum STRUCTURE_TYPE symbol_stype = get_symbol_stype(node->value.id);
        if (symbol_stype != ST_FUNCTION) {
            fprintf(stderr, "Prepass failed: identifier `%s` is not a function\n", symbol_name);
            return false;
        }

        return true;
    }

    return is_type_valid;
}
