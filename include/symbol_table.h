#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#include <stddef.h>
struct symbol {
    char *name;
};

const char *get_symbol_name(int);
int         find_symbol(const char *);
int         add_symbol(const char *);
void        clean_symbol_table();

#endif
