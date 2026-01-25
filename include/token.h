#ifndef _TOKEN_H
#define _TOKEN_H

#include <stddef.h>
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
    T_SEMICOLON,
    T_PRINT,
    T_IDENTIFIER,
};

union token_literal {
    int64_t  intval;
    double   doubleval;
    uint32_t id;
};

struct token {
    union token_literal value;
    const char         *lexeme_start;
    size_t              lexeme_length;
    uint32_t            line;
    enum TOKEN_TYPE     type;
};

#endif
