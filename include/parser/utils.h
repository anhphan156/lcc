#ifndef _PARSER_UTILS_H
#define _PARSER_UTILS_H

#include "defs.h"
#include "token.h"
#include <stdbool.h>

struct token;

void semicolon();
void identifier();
void l_paren();
void r_paren();

enum EXPRESSION_TYPE get_expression_type(enum TOKEN_TYPE);
bool                 is_type_token(enum TOKEN_TYPE);

bool is_ptr(enum EXPRESSION_TYPE);

enum EXPRESSION_TYPE prim_to_ptr(enum EXPRESSION_TYPE);
enum EXPRESSION_TYPE ptr_to_prim(enum EXPRESSION_TYPE);

#endif
