#include "ast/prepass.h"
#include "ast/ast.h"
#include "codegen/codegen.h"
#include "defs.h"
#include "symbol_table.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void prepass(struct ast_node *node) {
    if (!node)
        return;

    if (node->left)
        prepass(node->left);

    if (node->right)
        prepass(node->right);

    if (node->ast_node_type == AST_FUNC_CALL) {
        enum STRUCTURE_TYPE symbol_stype = get_symbol_stype(node->value.id);
        if (symbol_stype == -1) {
            fprintf(stderr, "Prepass failed: symbol not found\n");
            exit(1);
        }

        const char *symbol_name = get_symbol_name(node->value.id);
        if (symbol_stype != ST_FUNCTION) {
            fprintf(stderr, "Prepass failed: identifier `%s` is not a function\n", symbol_name);
            exit(1);
        }
    }

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
