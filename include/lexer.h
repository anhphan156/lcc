#ifndef _LEXER_H
#define _LEXER_H

#include "token.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void   lexical_scanner_setup(char *, size_t);
int8_t lexical_scan(struct token *);
bool   not_end();

#endif
