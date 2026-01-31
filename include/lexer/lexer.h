#ifndef _LEXER_H
#define _LEXER_H

#include "token.h"
#include <stdbool.h>
#include <stddef.h>

void lexical_scanner_setup(const char *, size_t);

bool         match(enum TOKEN_TYPE);
struct token get_current_token();
struct token get_previous_token();

#endif
