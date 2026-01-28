#include "ast/prepass.h"
#include "ast/ast.h"
#include "codegen/codegen.h"
#include "symbol_table.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void prepass(struct ast_node *node) {
    if (node == NULL) {
        return;
    }

    if (node->left) {
        prepass(node->left);
    }

    if (node->right) {
        prepass(node->right);
    }

    if (node->ast_node_type == AST_DECL && node->token_type == T_INT) {
        const char *symbol_name = get_symbol_name(node->value.id);
        if (symbol_name == NULL) {
            fprintf(stderr, "Prepass failed: symbol_name not found\n");
            exit(1);
        }
        gen_globl_sym(symbol_name);
    }
}
