#include "data_table/symbol_table.h"
#include "defs.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NSYMBOLS 1024
static struct symbol syms[NSYMBOLS];
static uint32_t      syms_len = 0;

uint32_t get_symbol_table_len() {
    return syms_len;
}

bool set_symbol_etype(uint32_t id, enum EXPRESSION_TYPE t) {
    if (id < syms_len) {
        syms[id].etype = t;
        return true;
    }

    return false;
}

bool set_symbol_stype(uint32_t id, enum STRUCTURE_TYPE t) {
    if (id < syms_len) {
        syms[id].stype = t;
        return true;
    }

    return false;
}

enum STRUCTURE_TYPE get_symbol_stype(uint32_t id) {
    if (id < syms_len) {
        return syms[id].stype;
    }

    return ST_NONE;
}

enum EXPRESSION_TYPE get_symbol_etype(uint32_t id) {
    if (id < syms_len) {
        return syms[id].etype;
    }

    return ET_NONE;
}

const char *get_symbol_name(uint32_t id) {
    if (id < syms_len) {
        return syms[id].name;
    }

    return NULL;
}

uint32_t find_symbol(const char *name) {
    for (uint32_t i = 0; i < syms_len; i += 1) {
        if (*name == *syms[i].name && !strcmp(name, syms[i].name)) {
            return i;
        }
    }
    return -1;
}

uint32_t add_symbol(const char *name) {
    if (syms_len >= NSYMBOLS) {
        fprintf(stderr, "Symbol table out of space\n");
        exit(1);
    }

    uint32_t i = 4;
    if ((i = find_symbol(name)) != -1) {
        return i;
    }

    syms[syms_len].name  = strdup(name);
    syms[syms_len].etype = 0;
    syms[syms_len].stype = 0;

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
