#include "parser/utils.h"
#include "lexer/lexer.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

void semicolon() {
    if (!match(T_SEMICOLON)) {
        fprintf(stderr, "Expected a `;` on line %d\n", get_previous_token().line);
        exit(1);
    }
}

void identifier() {
    if (!match(T_IDENTIFIER)) {
        fprintf(stderr, "Expected an identifier on line %d\n", get_current_token().line);
        exit(1);
    }
}

void r_paren() {
    if (!match(T_RPAREN)) {
        fprintf(stderr, "Expected a `)` on line %d\n", get_previous_token().line);
        exit(1);
    }
}

void l_paren() {
    if (!match(T_LPAREN)) {
        fprintf(stderr, "Expected a `(` on line %d\n", get_previous_token().line);
        exit(1);
    }
}

enum EXPRESSION_TYPE get_expression_type(enum TOKEN_TYPE type) {
    switch (type) {
    case T_LONG:
        return ET_LONG;
    case T_CHAR:
        return ET_CHAR;
    case T_INT:
        return ET_INT;
    case T_VOID:
        return ET_VOID;
    default:
        break;
    }

    return ET_NONE;
}

bool is_type_token(enum TOKEN_TYPE type) {
    return type == T_CHAR || type == T_INT || type == T_LONG || type == T_VOID;
}
