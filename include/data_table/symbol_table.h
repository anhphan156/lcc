#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#include "defs.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct symbol {
    char                *name;
    enum EXPRESSION_TYPE etype;
    enum STRUCTURE_TYPE  stype;
};

const char          *get_symbol_name(uint32_t);
enum STRUCTURE_TYPE  get_symbol_stype(uint32_t);
enum EXPRESSION_TYPE get_symbol_etype(uint32_t);
uint32_t             get_symbol_table_len();
bool                 set_symbol_etype(uint32_t, enum EXPRESSION_TYPE);
bool                 set_symbol_stype(uint32_t, enum STRUCTURE_TYPE);
uint32_t             find_symbol(const char *);
uint32_t             add_symbol(const char *);
void                 clean_symbol_table();

#endif
