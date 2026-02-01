#ifndef _LEXER_H
#define _LEXER_H

#include "token.h"
#include <stdbool.h>
#include <stddef.h>

void lexical_scanner_setup(const char *, size_t);

bool         match(enum TOKEN_TYPE);
bool         match_type();
bool         match_type_with_void();
struct token get_current_token();
struct token get_previous_token();

#endif
