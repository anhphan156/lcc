#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

struct symbol {
    char *name;
};

int find_symbol(char *);
int add_symbol(char *);

#endif
