#include "lexer.h"
#include "breakpoint.h"
#include "token.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char   *source      = 0;
static size_t        source_len  = 0;
static uint32_t      token_start = 0;
static uint32_t      cur         = 0;
static uint32_t      line        = 1;
static struct token *_token      = NULL;

static uint32_t old_token_start = 0;
static uint32_t old_cur         = 0;

static void add_token(const enum TOKEN_TYPE);
static void add_int_token(const enum TOKEN_TYPE);
static void add_double_token(const enum TOKEN_TYPE);
static char peek();
static char peek_previous();
static void advance();
static void parse_number();
static void parse_identifier();

int8_t lexical_scan(struct token *token) {
    if (!not_end())
        return -1;

    _token = token;
scan:
    old_token_start = token_start;
    old_cur         = cur;

    token_start       = cur;
    char current_char = peek();
    advance();

    switch (current_char) {
    case 0xA: // newline
        line += 1;
    case 0x20: // space
    case 0x9:  // tab
        if (!not_end()) {
            return -1;
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
    case ';':
        add_token(T_SEMICOLON);
        break;

    default: {
        if (current_char <= '9' && current_char >= '0') {
            parse_number();
            return 0;
        }

        if (isalpha(current_char) || current_char == '_') {
            parse_identifier();
            return 0;
        }
    }
    }

    return 0;
}

void lexical_go_back() {
    token_start = old_token_start;
    cur         = old_cur;
}

void lexical_scanner_setup(const char *input, size_t input_len) {
    source     = input;
    source_len = input_len;
}

bool not_end() {
    return cur < source_len;
}

static char peek() {
    if (cur >= source_len || cur < 0) {
        return 0x0;
    }

    return source[cur];
}

static char peek_previous() {
    if (old_cur >= source_len || old_cur < 0) {
        return 0x0;
    }

    return source[old_cur];
}

static void advance() {
    cur += 1;
}

static void add_token(const enum TOKEN_TYPE type) {
    if (_token == NULL) {
        BREAKPOINT;
    }

    _token->type          = type;
    _token->line          = line;
    _token->lexeme_start  = source + token_start;
    _token->lexeme_length = cur - token_start;
}

static void add_int_token(const enum TOKEN_TYPE type) {
    if (_token == NULL) {
        BREAKPOINT;
    }

    size_t lexeme_len = cur - token_start;
    char  *lexeme_str = (char *)malloc(lexeme_len + 1);
    memcpy(lexeme_str, source + token_start, lexeme_len);
    lexeme_str[lexeme_len] = 0;

    int64_t value = atoi(lexeme_str);
    free(lexeme_str);

    _token->value.intval  = value;
    _token->line          = line;
    _token->type          = type;
    _token->lexeme_start  = source + token_start;
    _token->lexeme_length = cur - token_start;
}

static void add_double_token(const enum TOKEN_TYPE type) {
    if (_token == NULL) {
        BREAKPOINT;
    }

    size_t lexeme_len = cur - token_start;
    char  *lexeme_str = (char *)malloc(lexeme_len + 1);
    memcpy(lexeme_str, source + token_start, lexeme_len);
    lexeme_str[lexeme_len] = 0;

    double value = strtod(lexeme_str, NULL);
    free(lexeme_str);

    _token->value.doubleval = value;
    _token->line            = line;
    _token->type            = type;
    _token->lexeme_start    = source + token_start;
    _token->lexeme_length   = cur - token_start;
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

        add_double_token(T_DOUBLELIT);
        return;
    }

    add_int_token(T_INTLIT);
}

static void parse_identifier() {
    char first_char = peek_previous();
    char current_char;

    do {
        advance();
        current_char = peek();
    } while (isalpha(current_char));

    switch (first_char) {
    case 's': {
        size_t len    = cur - token_start;
        char  *lexeme = (char *)malloc(len + 1);
        if (lexeme == NULL) {
            BREAKPOINT;
            fprintf(stderr, "Failed to allocate memory for lexeme\n");
            exit(1);
        }
        memcpy(lexeme, source + token_start, len);
        lexeme[len] = 0;

        if (!strcmp(lexeme, "scribe")) {
            add_token(T_SCRIBE);
        }
        free(lexeme);
        break;
    }
    default:
        add_token(T_IDENTIFIER);
        break;
    }
}
