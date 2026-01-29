#include "codegen/codegen.h"
#include "breakpoint.h"
#include "codegen/asm_file.h"
#include "codegen/x86/cg.h"
#include "ast/ast.h"
#include "symbol_table.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

#define NO_REG -1

static int ast_walker(struct ast_node *, int, struct ast_node *);
static int ast_walker_if(struct ast_node *);
static int label(void);

void generate_code(struct ast_node *node) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL) {
        exit(1);
    }
    preamble();
    ast_walker(node, NO_REG, NULL);
    postamble();
}

void gen_globl_sym(const char *symbol_name) {
    cg_globl_sym(symbol_name);
}

static int ast_walker(struct ast_node *node, int in_reg, struct ast_node *parent) {
    int left_reg = NO_REG, right_reg = NO_REG, result_reg = NO_REG;

    if (node->ast_node_type == AST_STMT_BRANCH) {
        return ast_walker_if(node);
    }

    if (node->left) {
        left_reg = ast_walker(node->left, NO_REG, node);
    }

    if (node->right) {
        right_reg = ast_walker(node->right, left_reg, node);
    }

    if (node->ast_node_type == AST_STMTS_BLOCK) {
        reg_free_all();
        return NO_REG;
    }

    if (node->ast_node_type == AST_LVALUE) {
        const char *sym_name = get_symbol_name(node->value.id);
        if (sym_name == NULL) {
            fprintf(stderr, "Prepass failed: symbol_name not found\n");
            BREAKPOINT;
        }
        cg_store_globl(in_reg, sym_name);
        reg_free_all();
        return NO_REG;
    }

    if (node->ast_node_type == AST_BIN_OP) {
        switch (node->token_type) {
        case T_PLUS:
            result_reg = cg_add(left_reg, right_reg);
            break;
        case T_MINUS:
            result_reg = cg_sub(left_reg, right_reg);
            break;
        case T_STAR:
            result_reg = cg_mul(left_reg, right_reg);
            break;
        case T_SLASH:
            return NO_REG;
        case T_ANDAND:
            result_reg = cg_and(left_reg, right_reg);
            break;
        case T_OROR:
            result_reg = cg_or(left_reg, right_reg);
            break;
        case T_EQEQ:
            result_reg = cg_compare_set(left_reg, right_reg, "sete");
            break;
        case T_NOTEQ:
            result_reg = cg_compare_set(left_reg, right_reg, "setne");
            break;
        case T_GT:
            result_reg = cg_compare_set(left_reg, right_reg, "setg");
            break;
        case T_GE:
            result_reg = cg_compare_set(left_reg, right_reg, "setge");
            break;
        case T_LT:
            result_reg = cg_compare_set(left_reg, right_reg, "setl");
            break;
        case T_LE:
            result_reg = cg_compare_set(left_reg, right_reg, "setle");
            break;
        default:
            break;
        }
    }

    if (node->ast_node_type == AST_UN_OP) {
        switch (node->token_type) {
        case T_MINUS:
            result_reg = cg_neg(left_reg);
            break;
        case T_NOT:
            result_reg = cg_not(left_reg);
            break;
        default:
            break;
        }
    }

    if (node->ast_node_type == AST_INTEGER) {
        result_reg = cg_load(node->value.intval);
    }

    if (node->ast_node_type == AST_GROUPING) {
        result_reg = left_reg;
    }

    if (node->ast_node_type == AST_IDENTIFIER) {
        const char *sym_name = get_symbol_name(node->value.id);
        if (sym_name == NULL) {
            fprintf(stderr, "Prepass failed: symbol_name not found\n");
            BREAKPOINT;
        }

        result_reg = cg_load_globl(sym_name);
    }

    if (node->ast_node_type == AST_STMT) {
        switch (node->token_type) {
        case T_PRINT:
            cg_print(left_reg);
            break;
        default:
            break;
        }
    }

    if (parent && parent->ast_node_type == AST_STMT_BRANCH) {
        cg_test_jmp(result_reg, in_reg);
    }

    return result_reg;
}

static int ast_walker_if(struct ast_node *node) {
    if (node->ast_node_type != AST_STMT_BRANCH || !node->left || !node->right) {
        BREAKPOINT;
    }

    struct ast_node *condition   = node->left;
    struct ast_node *consequence = node->right->left;
    struct ast_node *alternative = node->right->right;

    int l_else = 0;
    int l_end  = 0;

    l_else = label();
    if (alternative) {
        l_end = label();
    }

    ast_walker(condition, l_else, node);
    reg_free_all();

    ast_walker(consequence, NO_REG, node->right);
    reg_free_all();

    if (alternative) {
        cg_jmp(l_end);
    }

    cg_label(l_else);
    if (alternative) {
        ast_walker(alternative, NO_REG, node->right);
        reg_free_all();

        cg_label(l_end);
    }

    return NO_REG;
}

static int label(void) {
    static int id = 1;
    return id++;
}
