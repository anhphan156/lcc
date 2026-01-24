#include "codegen.h"
#include "ast.h"
#include "breakpoint.h"
#include "token.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static FILE *asm_stream = NULL;

#define MAX_REG 4
static char *registers_list[MAX_REG]  = {"%r8", "%r9", "%r10", "%r11"};
static char  registers_avail[MAX_REG] = {0};

static int  reg_alloc(void);
static void reg_free(int);
static void reg_free_all(void);

static int ast_walker(struct ast_node *);
static int cg_load(int);
static int cg_add(int, int);
static int cg_sub(int, int);
static int cg_mul(int, int);

void generate_code(struct ast_node *node) {
    asm_stream = fopen("./code.s", "w");
    if (asm_stream == NULL) {
        perror("fopen");
        exit(1);
    }
    fprintf(asm_stream, ".globl main\n"
                        ".section .note.GNU-stack,\"\",@progbits\n"
                        ".data\n"
                        "str: .asciz \"%%d\\n\"\n"
                        ".text\n"
                        "main:\n"
                        "push %%rbp\n"
                        "mov %%rsp, %%rbp\n");

    int reg = ast_walker(node);

    fprintf(asm_stream,
            "leaq str(%%rip), %%rdi\n"
            "movq %s, %%rsi\n"
            "xor %%rax, %%rax\n"
            "call printf\n"
            "leave \n"
            "mov $0, %%rax\n"
            "ret\n",
            registers_list[reg]);
    fclose(asm_stream);
}

static int ast_walker(struct ast_node *node) {
    int left_reg = -1, right_reg = -1;

    if (node->left)
        left_reg = ast_walker(node->left);

    if (node->right)
        right_reg = ast_walker(node->right);

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
        default:
            break;
        }
    }

    if (node->ast_node_type == AST_INTEGER) {
        return cg_load(node->value.intval);
    }

    return -1;
}

static int cg_add(int r1, int r2) {
    if (asm_stream == NULL)
        return -1;

    if (r1 == -1 || r2 == -1) {
        fprintf(stderr, "cg_op negative register location\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "addq %s, %s\n", registers_list[r1], registers_list[r2]);
    reg_free(r1);

    return r2;
}

static int cg_sub(int r1, int r2) {
    if (asm_stream == NULL)
        return -1;

    if (r1 == -1 || r2 == -1) {
        fprintf(stderr, "cg_op negative register location\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "subq %s, %s\n", registers_list[r2], registers_list[r1]);
    reg_free(r2);

    return r1;
}

static int cg_mul(int r1, int r2) {
    if (asm_stream == NULL)
        return -1;

    if (r1 == -1 || r2 == -1) {
        fprintf(stderr, "cg_op negative register location\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "imulq %s, %s\n", registers_list[r1], registers_list[r2]);
    reg_free(r1);

    return r2;
}

static int cg_load(int v) {
    if (asm_stream == NULL)
        return -1;

    int reg = reg_alloc();
    if (reg == -1) {
        BREAKPOINT;
    }

    fprintf(asm_stream, "movq $%d, %s\n", v, registers_list[reg]);

    return reg;
}

static int reg_alloc(void) {
    for (uint8_t i = 0; i < MAX_REG; i += 1) {
        if (registers_avail[i] == 0) {
            registers_avail[i] = -1;
            return i;
        }
    }

    return -1;
}

static void reg_free(int r) {
    if (r >= MAX_REG) {
        BREAKPOINT;
    }

    registers_avail[r] = 0;
}

static void reg_free_all(void) {
    for (uint8_t i = 0; i < MAX_REG; i += 1) {
        reg_free(i);
    }
}
