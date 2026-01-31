#include "codegen/x86/cg.h"
#include "codegen/asm_file.h"
#include "defs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_REG 4
static char *registers_list[MAX_REG]  = {"%r8", "%r9", "%r10", "%r11"};
static char  registers_avail[MAX_REG] = {0};

static int  reg_alloc(void);
static void reg_free(int);

void preamble() {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL) {
        exit(1);
    }
    fprintf(asm_stream, ".section .note.GNU-stack,\"\",@progbits\n"
                        ".data\n"
                        "str: .asciz \"%%d\\n\"\n");
}

void fn_preamble(const char *name) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL) {
        exit(1);
    }
    fprintf(asm_stream,
            ".text\n"
            ".globl %s\n"
            ".type %s, @function\n"
            "%s:\n"
            "push %%rbp\n"
            "mov %%rsp, %%rbp\n",
            name, name, name);
}

void fn_postamble() {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL) {
        exit(1);
    }
    fprintf(asm_stream, "leave \n"
                        "xor %%rax, %%rax\n"
                        "ret\n");
}

void cg_call(const char *name) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return;

    fprintf(asm_stream, "call %s\n", name);
}

int cg_add(int r1, int r2) {
    FILE *asm_stream = asmfget();
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

int cg_sub(int r1, int r2) {
    FILE *asm_stream = asmfget();
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

int cg_mul(int r1, int r2) {
    FILE *asm_stream = asmfget();
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

int cg_div(int r1, int r2) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return -1;

    fprintf(asm_stream, "movq %s, %%rax\n", registers_list[r1]);
    fprintf(asm_stream, "cqo\n");
    fprintf(asm_stream, "idivq %s\n", registers_list[r2]);
    fprintf(asm_stream, "movq %%rax, %s\n", registers_list[r1]);

    reg_free(r2);
    return r1;
}

int cg_neg(int reg) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return -1;

    if (reg == -1) {
        fprintf(stderr, "cg_op negative register location\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "neg %s\n", registers_list[reg]);
    return reg;
}

void cg_jmp(int label_id) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return;

    fprintf(asm_stream, "jmp L%d\n", label_id);
}

int cg_and(int r1, int r2) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return -1;

    if (r1 == -1 || r2 == -1) {
        fprintf(stderr, "cg_op negative register location\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "andq %s, %s\n", registers_list[r1], registers_list[r2]);
    reg_free(r1);
    return r2;
}

int cg_or(int r1, int r2) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return -1;

    if (r1 == -1 || r2 == -1) {
        fprintf(stderr, "cg_op negative register location\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "orq %s, %s\n", registers_list[r1], registers_list[r2]);
    reg_free(r1);
    return r2;
}

int cg_not(int r1) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return -1;

    if (r1 == -1) {
        fprintf(stderr, "cg_op negative register location\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "test %s, %s\n", registers_list[r1], registers_list[r1]);
    fprintf(asm_stream, "setz %sb\n", registers_list[r1]);
    return r1;
}

int cg_compare_set(int r1, int r2, const char *op) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return -1;

    if (r1 == -1 || r2 == -1) {
        fprintf(stderr, "cg_op negative register location\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "cmpq %s, %s\n", registers_list[r2], registers_list[r1]);
    fprintf(asm_stream, "%s %sb\n", op, registers_list[r2]);
    fprintf(asm_stream, "andq $0x1, %s\n", registers_list[r2]);

    reg_free(r1);
    return r2;
}

void cg_test_jmp(int reg, int label_id) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return;

    if (reg == -1) {
        fprintf(stderr, "cg_op negative register location\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "test %s, %s\n", registers_list[reg], registers_list[reg]);
    fprintf(asm_stream, "je L%d\n", label_id);

    reg_free(reg);
}

void cg_print(int reg) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return;

    fprintf(asm_stream,
            "leaq str(%%rip), %%rdi\n"
            "movq %s, %%rsi\n"
            "xor %%rax, %%rax\n"
            "call printf\n",
            registers_list[reg]);

    reg_free_all();
}

int cg_load(int v) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return -1;

    int reg = reg_alloc();
    if (reg == -1) {
        fprintf(stderr, "cg_load: can't allocate register\n");
        BREAKPOINT;
    }

    fprintf(asm_stream, "movq $%d, %s\n", v, registers_list[reg]);

    return reg;
}

void cg_label(int id) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return;

    fprintf(asm_stream, "L%d:\n", id);
}

void cg_store_globl(int reg, const char *sym, enum EXPRESSION_TYPE size) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return;

    switch (size) {
    case ET_CHAR:
        fprintf(asm_stream, "movb %sb, %s(%%rip)\n", registers_list[reg], sym);
        break;
    case ET_INT:
        fprintf(asm_stream, "movl %sd, %s(%%rip)\n", registers_list[reg], sym);
        break;
    case ET_LONG:
        fprintf(asm_stream, "movq %s, %s(%%rip)\n", registers_list[reg], sym);
        break;
    default:
        break;
    }
    reg_free(reg);
}

int cg_load_globl(const char *sym, enum EXPRESSION_TYPE size) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL)
        return -1;

    int reg = reg_alloc();
    if (reg == -1) {
        fprintf(stderr, "cg_load_globl: can't allocate register\n");
        BREAKPOINT;
    }

    switch (size) {
    case ET_CHAR:
        fprintf(asm_stream, "movb %s(%%rip), %sb\n", sym, registers_list[reg]);
        break;
    case ET_INT:
        fprintf(asm_stream, "movl %s(%%rip), %sd\n", sym, registers_list[reg]);
        break;
    case ET_LONG:
        fprintf(asm_stream, "movq %s(%%rip), %s\n", sym, registers_list[reg]);
        break;
    default:
        break;
    }

    return reg;
}

void cg_globl_sym(const char *symbol_name, uint8_t size) {
    FILE *asm_stream = asmfget();
    if (asm_stream == NULL) {
        fprintf(stderr, "Prepass failed: asm_stream is NULL\n");
        exit(1);
    }

    fprintf(asm_stream, ".comm %s,%d,8\n", symbol_name, size);
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

void reg_free_all(void) {
    for (uint8_t i = 0; i < MAX_REG; i += 1) {
        reg_free(i);
    }
}
