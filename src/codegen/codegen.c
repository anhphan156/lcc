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
static int ast_walker_while(struct ast_node *);
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
    int left_reg = NO_REG, right_reg = NO_REG;

    if (node->ast_node_type == AST_STMT_BRANCH) {
        return ast_walker_if(node);
    }

    if (node->ast_node_type == AST_STMT_WHILE) {
        return ast_walker_while(node);
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
            return cg_add(left_reg, right_reg);
        case T_MINUS:
            return cg_sub(left_reg, right_reg);
        case T_STAR:
            return cg_mul(left_reg, right_reg);
        case T_SLASH:
            return NO_REG;
        case T_ANDAND:
            return cg_and(left_reg, right_reg);
        case T_OROR:
            return cg_or(left_reg, right_reg);
        case T_EQEQ:
            return cg_compare_set(left_reg, right_reg, "sete");
        case T_NOTEQ:
            return cg_compare_set(left_reg, right_reg, "setne");
        case T_GT:
            return cg_compare_set(left_reg, right_reg, "setg");
        case T_GE:
            return cg_compare_set(left_reg, right_reg, "setge");
        case T_LT:
            return cg_compare_set(left_reg, right_reg, "setl");
        case T_LE:
            return cg_compare_set(left_reg, right_reg, "setle");
        default:
            return NO_REG;
        }
    }

    if (node->ast_node_type == AST_UN_OP) {
        switch (node->token_type) {
        case T_MINUS:
            return cg_neg(left_reg);
        case T_NOT:
            return cg_not(left_reg);
        default:
            return NO_REG;
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
        case T_PRINT:
            cg_print(left_reg);
            return NO_REG;
        default:
            return NO_REG;
        }
    }

    return NO_REG;
}

static int ast_walker_if(struct ast_node *node) {
    if (!(node && node->ast_node_type == AST_STMT_BRANCH && node->left && node->right)) {
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

    int expr_reg = ast_walker(condition, l_else, node);
    cg_test_jmp(expr_reg, l_else);
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

static int ast_walker_while(struct ast_node *node) {
    if (!(node && node->ast_node_type == AST_STMT_WHILE && node->left && node->right)) {
        BREAKPOINT;
    }

    int l_start = label();
    int l_end   = label();

    cg_label(l_start);
    int expr_reg = ast_walker(node->left, l_end, node);
    cg_test_jmp(expr_reg, l_end);
    reg_free_all();

    ast_walker(node->right, NO_REG, node->right);
    reg_free_all();
    cg_jmp(l_start);
    cg_label(l_end);

    return NO_REG;
}

static int label(void) {
    static int id = 1;
    return id++;
}
