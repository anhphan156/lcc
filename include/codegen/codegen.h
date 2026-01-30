#ifndef _CODEGEN_H
#define _CODEGEN_H

#include <stdint.h>

struct ast_node;
void asm_preamble();
void generate_code(struct ast_node *);
void gen_globl_sym(const char *, uint8_t);

#endif
