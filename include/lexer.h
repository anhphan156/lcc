#ifndef _LEXER_H
#define _LEXER_H

#include <stdbool.h>
#include <stddef.h>

void lexical_scanner_setup(char *, size_t);
void lexical_scanning();
bool not_end();

#endif
