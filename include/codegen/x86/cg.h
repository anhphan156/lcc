#ifndef _X86CG_H
#define _X86CG_H

void preamble();
void postamble();
void cg_print(int);
int  cg_load(int);
int  cg_add(int, int);
int  cg_sub(int, int);
int  cg_mul(int, int);
int  cg_neg(int);
void cg_globl_sym(const char *);
void cg_store_globl(int, const char *);
int  cg_load_globl(const char *);

#endif
