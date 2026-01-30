#ifndef _X86CG_H
#define _X86CG_H

#include <stdint.h>

void preamble();
void fn_preamble(const char *);
void fn_postamble();
void cg_call(const char *);
void cg_print(int);
int  cg_load(int);
int  cg_add(int, int);
int  cg_sub(int, int);
int  cg_mul(int, int);
int  cg_div(int, int);
int  cg_neg(int);
void cg_jmp(int);
int  cg_and(int, int);
int  cg_or(int, int);
int  cg_not(int);
int  cg_compare_set(int, int, const char *);
void cg_test_jmp(int, int);
void cg_globl_sym(const char *, uint8_t);
void cg_label(int);
void cg_store_globl(int, const char *);
int  cg_load_globl(const char *);

void reg_free_all(void);

#endif
