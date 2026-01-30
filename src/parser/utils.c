#include "parser/utils.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

void semicolon() {
    if (!match(T_SEMICOLON)) {
        fprintf(stderr, "Expected a `;` on line %d\n", get_previous_token().line);
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
