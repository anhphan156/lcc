#ifndef _TOKEN_H
#define _TOKEN_H

#include <stddef.h>
#include <stdint.h>

#define TOKEN_LIST  \
    X(T_EOF)        \
    X(T_PLUS)       \
    X(T_MINUS)      \
    X(T_STAR)       \
    X(T_SLASH)      \
    X(T_EQ)         \
    X(T_EQEQ)       \
    X(T_NOTEQ)      \
    X(T_LT)         \
    X(T_LE)         \
    X(T_GT)         \
    X(T_GE)         \
    X(T_NOT)        \
    X(T_AND)        \
    X(T_ANDAND)     \
    X(T_OR)         \
    X(T_OROR)       \
    X(T_LPAREN)     \
    X(T_RPAREN)     \
    X(T_LBRACE)     \
    X(T_RBRACE)     \
    X(T_INTLIT)     \
    X(T_DOUBLELIT)  \
    X(T_STRLIT)     \
    X(T_INT)        \
    X(T_CHAR)       \
    X(T_SEMICOLON)  \
    X(T_IDENTIFIER) \
    X(T_PRINT)      \
    X(T_IF)         \
    X(T_ELSE)       \
    X(T_WHILE)      \
    X(T_FOR)        \
    X(T_VOID)

enum TOKEN_TYPE {
#define X(name) name,
    TOKEN_LIST
#undef X
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
