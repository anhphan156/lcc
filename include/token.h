#ifndef _TOKEN_H
#define _TOKEN_H

#include <stdint.h>

enum TOKEN_TYPE {
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_EQ,
    T_EQEQ,
    T_LPAREN,
    T_RPAREN,
    T_INTLIT,
    T_FLOATLIT,
    T_STRLIT,
};

struct token {
    enum TOKEN_TYPE type;
    const char     *lexeme;
    uint32_t        line;
};

#endif
