#ifndef _DEFS_H
#define _DEFS_H

enum STRUCTURE_TYPE {
    ST_NONE,
    ST_VARIABLE,
    ST_FUNCTION,
    ST_LITERAL,
};

enum EXPRESSION_TYPE {
    ET_NONE,
    ET_VOID,
    ET_CHAR,
    ET_INT,
    ET_LONG,
    ET_VOID_PTR,
    ET_CHAR_PTR,
    ET_INT_PTR,
    ET_LONG_PTR,
};

#endif
