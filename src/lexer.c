#include "lexer.h"
#include "token.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char    *source      = 0;
static size_t   source_len  = 0;
static uint32_t token_start = 0;
static uint32_t cur         = 0;
static uint32_t line        = 1;

static char peek() {
    if (cur >= source_len || cur < 0) {
        return 0x0;
    }

    return source[cur];
}

static void advance() {
    cur += 1;
    if (cur >= source_len) {
        cur = -1;
    }
}

static void add_token(const enum TOKEN_TYPE type) {
    uint32_t token_len = cur - token_start;
    char    *token     = malloc(token_len + 1);

    memcpy(token, source + token_start, token_len);
    token[token_len] = 0;

    printf("Token: %s on line %d\n", token, line);

    free(token);
}

static void parse_number() {
    char current_char = peek();

    while (current_char <= '9' && current_char >= '0') {
        advance();
        current_char = peek();
    }

    if (current_char == '.') {
        do {
            advance();
            current_char = peek();
        } while (current_char <= '9' && current_char >= '0');

        add_token(T_FLOATLIT);
        return;
    }

    add_token(T_INTLIT);
}

void lexical_scanning() {
scan:
    token_start       = cur;
    char current_char = peek();
    advance();

    if (current_char <= '9' && current_char >= '0') {
        parse_number();
        return;
    }

    switch (current_char) {
    case 0xA: // newline
        line += 1;
    case 0x20: // space
    case 0x9:  // tab
        if (cur == -1)
            break;
        goto scan;
    case '+':
        add_token(T_PLUS);
        break;
    case '-':
        add_token(T_MINUS);
        break;
    case '*':
        add_token(T_STAR);
        break;
    case '/':
        add_token(T_SLASH);
        break;

    default:
        printf("Unrecognized token %c\n", current_char);
        break;
    }
}

void lexical_scanner_setup(char *input, size_t input_len) {
    source     = input;
    source_len = input_len;
}

bool not_end() {
    return cur != -1;
}
