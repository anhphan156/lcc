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
    T_DOUBLELIT,
    T_STRLIT,
};

struct token {
    union {
        int64_t  intval;
        double   doubleval;
        uint32_t id;
    } value;
    uint32_t        line;
    enum TOKEN_TYPE type;
};

#endif
