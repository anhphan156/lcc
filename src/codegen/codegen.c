#include "codegen/codegen.h"
#include "breakpoint.h"
#include "codegen/asm_file.h"
#include "codegen/x86/cg.h"
#include "ast/ast.h"
#include "symbol_table.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

static int ast_walker(struct ast_node *, int);

void generate_code(struct ast_node *node) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL) {
        exit(1);
    }
    preamble();
    ast_walker(node, -1);
    postamble();
}

static int ast_walker(struct ast_node *node, int in_reg) {
    int left_reg = -1, right_reg = -1;

    if (node->left)
        left_reg = ast_walker(node->left, -1);

    if (node->right)
        right_reg = ast_walker(node->right, left_reg);

    if (node->ast_node_type == AST_LVALUE) {
        const char *sym_name = get_symbol_name(node->value.id);
        if (sym_name == NULL) {
            fprintf(stderr, "Prepass failed: symbol_name not found\n");
            BREAKPOINT;
        }
        cg_store_globl(in_reg, sym_name);
    }

    if (node->ast_node_type == AST_BIN_OP) {
        switch (node->token_type) {
        case T_PLUS:
            return cg_add(left_reg, right_reg);
        case T_MINUS:
            return cg_sub(left_reg, right_reg);
        case T_STAR:
            return cg_mul(left_reg, right_reg);
        case T_SLASH:
            break;
        case T_ANDAND:
            return cg_and(left_reg, right_reg);
            break;
        case T_OROR:
            return cg_or(left_reg, right_reg);
            break;
        case T_EQEQ:
            return cg_compare(left_reg, right_reg, "sete");
            break;
        case T_NOTEQ:
            return cg_compare(left_reg, right_reg, "setne");
            break;
        case T_GT:
            return cg_compare(left_reg, right_reg, "setg");
            break;
        case T_GE:
            return cg_compare(left_reg, right_reg, "setge");
            break;
        case T_LT:
            return cg_compare(left_reg, right_reg, "setl");
            break;
        case T_LE:
            return cg_compare(left_reg, right_reg, "setle");
            break;
        default:
            break;
        }
    }

    if (node->ast_node_type == AST_UN_OP) {
        switch (node->token_type) {
        case T_MINUS:
            return cg_neg(left_reg);
            break;
        default:
            break;
        }
    }

    if (node->ast_node_type == AST_INTEGER) {
        return cg_load(node->value.intval);
    }

    if (node->ast_node_type == AST_GROUPING) {
        return left_reg;
    }

    if (node->ast_node_type == AST_IDENTIFIER) {
        const char *sym_name = get_symbol_name(node->value.id);
        if (sym_name == NULL) {
            fprintf(stderr, "Prepass failed: symbol_name not found\n");
            BREAKPOINT;
        }
        return cg_load_globl(sym_name);
    }

    if (node->ast_node_type == AST_STMT) {
        switch (node->token_type) {
        case T_SCRIBE:
            cg_print(left_reg);
            break;
        default:
            break;
        }
    }

    return -1;
}

void gen_globl_sym(const char *symbol_name) {
    cg_globl_sym(symbol_name);
}
