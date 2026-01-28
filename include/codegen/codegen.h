#ifndef _CODEGEN_H
#define _CODEGEN_H

struct ast_node;
void generate_code(struct ast_node *);
void gen_globl_sym(const char *);

#endif
