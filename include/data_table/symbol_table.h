#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#include "data_table/function_table.h"
#include "defs.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct symbol {
    struct function_info function_info;
    enum EXPRESSION_TYPE etype;
    enum STRUCTURE_TYPE  stype;
    char                *name;
};

const char          *get_symbol_name(uint32_t);
uint32_t             get_symbol_table_len();
enum STRUCTURE_TYPE  get_symbol_stype(uint32_t);
enum EXPRESSION_TYPE get_symbol_etype(uint32_t);
bool                 set_symbol_etype(uint32_t, enum EXPRESSION_TYPE);
bool                 set_symbol_stype(uint32_t, enum STRUCTURE_TYPE);
uint32_t             find_symbol(const char *);
uint32_t             add_symbol(const char *);
void                 clean_symbol_table();
void                 add_function_arg(uint32_t, enum EXPRESSION_TYPE);

#endif
