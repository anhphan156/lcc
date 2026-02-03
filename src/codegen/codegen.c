#include "codegen/codegen.h"
#include "defs.h"
#include "parser/utils.h"
#include "utils.h"
#include "codegen/asm_file.h"
#include "codegen/x86/cg.h"
#include "ast/ast.h"
#include "data_table/symbol_table.h"
#include "token.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NO_REG -1

static void ast_walker(struct ast_node *);
static int  ast_walker_expression(struct ast_node *);
static void ast_walker_if(struct ast_node *);
static void ast_walker_while(struct ast_node *);
static void ast_walker_assignment(struct ast_node *);
static void ast_walker_print(struct ast_node *);
static void ast_walker_return(struct ast_node *);
static int  label(void);

void asm_preamble() {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL) {
        exit(1);
    }
    preamble();
}

void generate_code(struct ast_node *node) {
    ast_walker(node);
}

void gen_globl_sym(const char *symbol_name, uint8_t size) {
    cg_globl_sym(symbol_name, size);
}

static void ast_walker(struct ast_node *node) {

    if (node->ast_node_type == AST_DECL)
        return;

    if (node->ast_node_type == AST_FUNC_CALL) {
        const char *sym_name = get_symbol_name(node->value.id);
        if (sym_name == NULL) {
            fprintf(stderr, "funtion call failed: symbol_name not found\n");
            exit(1);
        }

        return cg_call_stmt(sym_name);
    }

    if (node->ast_node_type == AST_FUNC) {
        const char *symbol_name = get_symbol_name(node->left->value.id);
        if (symbol_name == NULL) {
            fprintf(stderr, "funtion preamble failed: symbol_name not found\n");
            exit(1);
        }
        fn_preamble(symbol_name);
        ast_walker(node->right);
        if (node->data_type == ET_VOID) {
            fn_postamble();
        }

        return;
    }

    if (node->ast_node_type == AST_STMT) {
        switch (node->token_type) {
        case T_IF:
            return ast_walker_if(node);
        case T_WHILE:
            return ast_walker_while(node);
        case T_FOR:
            ast_walker_assignment(node->left);
            return ast_walker_while(node->right);
        case T_EQ:
            return ast_walker_assignment(node);
        case T_PRINT:
            return ast_walker_print(node);
        case T_RETURN:
            return ast_walker_return(node);
        default:
            return;
        }
    }

    if (node->left) {
        ast_walker(node->left);
    }

    if (node->right) {
        ast_walker(node->right);
    }

    if (node->ast_node_type == AST_STMTS_BLOCK) {
        reg_free_all();
    }
}

static int ast_walker_expression(struct ast_node *node) {
    int left_reg = NO_REG, right_reg = NO_REG;

    if (node->left) {
        left_reg = ast_walker_expression(node->left);
    }

    if (node->right) {
        right_reg = ast_walker_expression(node->right);
    }

    if (node->ast_node_type == AST_FUNC_CALL) {
        const char *sym_name = get_symbol_name(node->value.id);
        if (sym_name == NULL) {
            fprintf(stderr, "funtion call failed: symbol_name not found\n");
            exit(1);
        }

        return cg_call_expr(sym_name);
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
            return cg_div(left_reg, right_reg);
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

    if (node->ast_node_type == AST_LITERAL_NUMBER) {
        return cg_load(node->value.intval);
    }

    if (node->ast_node_type == AST_GROUPING) {
        return left_reg;
    }

    if (node->ast_node_type == AST_DEREF) {
        const char *sym_name = get_symbol_name(node->value.id);
        if (sym_name == NULL) {
            fprintf(stderr, "Prepass failed: symbol_name not found\n");
            BREAKPOINT;
        }
        return cg_dereference(sym_name, node->data_type);
    }

    if (node->ast_node_type == AST_ADDR) {
        const char *sym_name = get_symbol_name(node->value.id);
        if (sym_name == NULL) {
            fprintf(stderr, "Prepass failed: symbol_name not found\n");
            BREAKPOINT;
        }
        return cg_address(sym_name);
    }

    if (node->ast_node_type == AST_IDENTIFIER) {
        const char *sym_name = get_symbol_name(node->value.id);
        if (sym_name == NULL) {
            fprintf(stderr, "Prepass failed: symbol_name not found\n");
            BREAKPOINT;
        }

        return cg_load_globl(sym_name, node->data_type);
    }

    return NO_REG;
}

static void ast_walker_if(struct ast_node *node) {
    struct ast_node *condition   = node->left;
    struct ast_node *consequence = node->right->left;
    struct ast_node *alternative = node->right->right;

    int l_else = 0;
    int l_end  = 0;

    l_else = label();
    if (alternative) {
        l_end = label();
    }

    int expr_reg = ast_walker_expression(condition);
    cg_test_jmp(expr_reg, l_else);
    reg_free_all();

    ast_walker(consequence);
    reg_free_all();

    if (alternative) {
        cg_jmp(l_end);
    }

    cg_label(l_else);
    if (alternative) {
        ast_walker(alternative);
        reg_free_all();

        cg_label(l_end);
    }
}

static void ast_walker_while(struct ast_node *node) {
    int l_start = label();
    int l_end   = label();

    cg_label(l_start);
    int expr_reg = ast_walker_expression(node->left);
    cg_test_jmp(expr_reg, l_end);
    reg_free_all();

    ast_walker(node->right);
    reg_free_all();
    cg_jmp(l_start);
    cg_label(l_end);
}

static void ast_walker_assignment(struct ast_node *node) {
    const char *sym_name = get_symbol_name(node->right->value.id);
    if (sym_name == NULL) {
        fprintf(stderr, "Codegen failed: symbol_name not found\n");
        BREAKPOINT;
    }

    int r = ast_walker_expression(node->left);
    cg_store_globl(r, sym_name, node->data_type);
    reg_free_all();
}

static void ast_walker_print(struct ast_node *node) {
    cg_print(ast_walker_expression(node->left));
}

static void ast_walker_return(struct ast_node *node) {
    int expr_reg = ast_walker_expression(node->left);
    cg_return(expr_reg);
}

static int label(void) {
    static int id = 1;
    return id++;
}
