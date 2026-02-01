#include "lowering/pre_codegen.h"
#include "ast/ast.h"
#include "codegen/codegen.h"
#include "data_table/symbol_table.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void pre_codegen(struct ast_node *node) {
    if (!node)
        return;

    if (node->left)
        pre_codegen(node->left);

    if (node->right)
        pre_codegen(node->right);

    if (node->ast_node_type == AST_DECL) {
        if (node->token_type == T_VOID) {
            return;
        }

        const char *symbol_name = get_symbol_name(node->value.id);
        if (symbol_name == NULL) {
            fprintf(stderr, "Prepass failed: symbol_name not found\n");
            exit(1);
        }

        switch (node->token_type) {
        case T_LONG:
            gen_globl_sym(symbol_name, 8);
            break;
        case T_INT:
            gen_globl_sym(symbol_name, 4);
            break;
        case T_CHAR:
            gen_globl_sym(symbol_name, 1);
            break;
        default:
            break;
        }
    }
}
