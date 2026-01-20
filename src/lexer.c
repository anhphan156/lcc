#include "lexer.h"
#include "token.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char         *source      = 0;
static size_t        source_len  = 0;
static uint32_t      token_start = 0;
static uint32_t      cur         = 0;
static uint32_t      line        = 1;
static struct token *_token      = NULL;

static char peek() {
    if (cur >= source_len || cur < 0) {
        return 0x0;
    }

    return source[cur];
}

static void advance() {
    cur += 1;
}

static void add_token(const enum TOKEN_TYPE type) {
    uint32_t lexeme_len = cur - token_start;
    char    *lexeme     = malloc(lexeme_len + 1);

    memcpy(lexeme, source + token_start, lexeme_len);
    lexeme[lexeme_len] = 0;

    if (_token == NULL) {
        __asm__ volatile("int3");
    }

    _token->type   = type;
    _token->lexeme = lexeme;
    _token->line   = line;
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

void lexical_scan(struct token *token) {
    _token = token;
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
        if (cur == -1) {
            return;
        }
        goto scan; // skip all whitespaces and scan again for the next token
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
        printf("Unrecognized token %x on line %d\n", current_char, line);
        break;
    }
}

void lexical_scanner_setup(char *input, size_t input_len) {
    source     = input;
    source_len = input_len;
}

bool not_end() {
    return cur < source_len;
}

void clean_token(struct token *token) {
    if (token->lexeme != NULL) {
        free((void *)token->lexeme);
        token->lexeme = NULL;
    }
}
