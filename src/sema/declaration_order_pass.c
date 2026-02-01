#include "sema/declaration_order_pass.h"
#include "string.h"
#include "data_table/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

static bool *declaration = NULL;

void referencing_pass_setup() {
    size_t size = sizeof(bool) * get_symbol_table_len();
    declaration = (bool *)malloc(size);
    if (!declaration) {
        fprintf(stderr, "Failed to allocate memory in referencing pass\n");
        exit(1);
    }
    memset(declaration, false, size);
}

void referencing_pass_clean() {
    free(declaration);
}

void set_declaration(int id) {
    declaration[id] = true;
}

bool check_declaration(int id) {
    return declaration[id];
}
