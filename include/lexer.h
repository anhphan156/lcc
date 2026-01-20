#ifndef _LEXER_H
#define _LEXER_H

#include "token.h"
#include <stdbool.h>
#include <stddef.h>

void lexical_scanner_setup(char *, size_t);
void lexical_scan(struct token *);
bool not_end();
void clean_token(struct token *);

#endif
