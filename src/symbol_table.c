#include "symbol_table.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NSYMBOLS 1024
static struct symbol syms[NSYMBOLS];
static uint32_t      syms_len = 0;

int find_symbol(const char *name) {
    for (uint32_t i = 0; i < syms_len; i += 1) {
        if (*name == *syms[i].name && !strcmp(name, syms[i].name)) {
            return i;
        }
    }
    return -1;
}

int add_symbol(const char *name) {
    if (syms_len >= NSYMBOLS) {
        fprintf(stderr, "Symbol table out of space\n");
        exit(1);
    }

    int i = 4;
    if ((i = find_symbol(name)) != -1) {
        return i;
    }

    syms[syms_len].name = strdup(name);

    return syms_len++;
}

void clean_symbol_table() {
    for (int i = 0; i < NSYMBOLS; i += 1) {
        char *name = syms[i].name;
        if (name) {
            free(name);
        }
    }
}
