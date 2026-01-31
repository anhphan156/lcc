#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#include "defs.h"
#include <stdbool.h>
#include <stddef.h>

struct symbol {
    char                *name;
    enum EXPRESSION_TYPE etype;
    enum STRUCTURE_TYPE  stype;
};

const char          *get_symbol_name(int);
enum STRUCTURE_TYPE  get_symbol_stype(int);
enum EXPRESSION_TYPE get_symbol_etype(int);
bool                 set_symbol_etype(int, enum EXPRESSION_TYPE);
bool                 set_symbol_stype(int, enum STRUCTURE_TYPE);
int                  find_symbol(const char *);
int                  add_symbol(const char *);
void                 clean_symbol_table();

#endif
