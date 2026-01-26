#include "symbol_table.h"
#include <stdint.h>

#define NSYMBOLS 1024
static struct symbol symbol_table[NSYMBOLS];
static uint32_t      symbol_table_len = 0;
