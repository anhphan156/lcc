#include "lexer.h"
#include "symbol_table.h"
#include "token.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *source      = 0;
static size_t      source_len  = 0;
static uint32_t    token_start = 0;
static uint32_t    cur         = 0;
static uint32_t    line        = 1;

static struct token current_token;
static struct token previous_token;

static void   add_token(const enum TOKEN_TYPE);
static void   add_int_token(const enum TOKEN_TYPE);
static void   add_double_token(const enum TOKEN_TYPE);
static bool   add_keyword(const enum TOKEN_TYPE, const char *);
static void   add_identifier();
static char   peek();
static void   advance();
static void   parse_number();
static void   parse_identifier(char);
static bool   is_eof();
static int8_t lexical_next();

int8_t lexical_next() {
scan:
    if (is_eof()) {
        current_token.type = T_EOF;
        return -1;
    }

    token_start       = cur;
    char current_char = peek();
    advance();

    switch (current_char) {
    case 0xA: // newline
        line += 1;
    case 0x20:     // space
    case 0x9:      // tab
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
    case '|':
        if (source[cur] == '|') {
            advance();
            add_token(T_OROR);
            break;
        }
        add_token(T_OR);
        break;
    case '&':
        if (source[cur] == '&') {
            advance();
            add_token(T_ANDAND);
            break;
        }
        add_token(T_AND);
        break;
    case '!':
        if (source[cur] == '=') {
            advance();
            add_token(T_NOTEQ);
            break;
        }
        add_token(T_NOT);
        break;
    case '=':
        if (source[cur] == '=') {
            advance();
            add_token(T_EQEQ);
            break;
        }
        add_token(T_EQ);
        break;
    case '>':
        if (source[cur] == '=') {
            advance();
            add_token(T_GE);
            break;
        }
        add_token(T_GT);
        break;
    case '<':
        if (source[cur] == '=') {
            advance();
            add_token(T_LE);
            break;
        }
        add_token(T_LT);
        break;
    case '(':
        add_token(T_LPAREN);
        break;
    case ')':
        add_token(T_RPAREN);
        break;
    default: {
        if (current_char <= '9' && current_char >= '0') {
            parse_number();
            return 0;
        }

        if (isalpha(current_char) || current_char == '_') {
            parse_identifier(current_char);
            return 0;
        }

        fprintf(stderr, "Unrecognized token %c on line %d\n", current_char, line);
        exit(1);
    }
    }

    return 0;
}

bool match(enum TOKEN_TYPE token_type) {
    if (current_token.type == token_type) {
        previous_token = current_token;
        lexical_next();
        return true;
    }
    return false;
}

struct token get_current_token() {
    return current_token;
}

struct token get_previous_token() {
    return previous_token;
}

void lexical_scanner_setup(const char *input, size_t input_len) {
    source     = input;
    source_len = input_len;
    lexical_next(); // scan once on startup so the current_token becomes the first token
}

bool is_eof() {
    return cur >= source_len;
}

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
    current_token.type          = type;
    current_token.line          = line;
    current_token.lexeme_start  = source + token_start;
    current_token.lexeme_length = cur - token_start;
}

static void add_int_token(const enum TOKEN_TYPE type) {
    char   *lexeme_str          = strndup(source + token_start, cur - token_start);
    int64_t value               = atoi(lexeme_str);
    current_token.value.intval  = value;
    current_token.line          = line;
    current_token.type          = type;
    current_token.lexeme_start  = source + token_start;
    current_token.lexeme_length = cur - token_start;
    free(lexeme_str);
}

static void add_double_token(const enum TOKEN_TYPE type) {
    char  *lexeme_str             = strndup(source + token_start, cur - token_start);
    double value                  = strtod(lexeme_str, NULL);
    current_token.value.doubleval = value;
    current_token.line            = line;
    current_token.type            = type;
    current_token.lexeme_start    = source + token_start;
    current_token.lexeme_length   = cur - token_start;
    free(lexeme_str);
}

static void add_identifier() {
    char *name                  = strndup(source + token_start, cur - token_start);
    current_token.value.id      = add_symbol(name);
    current_token.line          = line;
    current_token.type          = T_IDENTIFIER;
    current_token.lexeme_start  = source + token_start;
    current_token.lexeme_length = cur - token_start;
    free(name);
}

static bool add_keyword(const enum TOKEN_TYPE token_type, const char *keyword) {
    char *lexeme = strdup(keyword);

    bool valid_keyword = false;
    if (!strcmp(lexeme, keyword)) {
        add_token(token_type);
        valid_keyword = true;
    }
    free(lexeme);
    return valid_keyword;
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

static void parse_identifier(char first_char) {
    char current_char = peek();

    while (isalpha(current_char)) {
        current_char = peek();
        advance();
    }

    bool valid_keyword = false;
    switch (first_char) {
    case 'i':
        valid_keyword = add_keyword(T_INT, "int");
        break;
    case 's':
        valid_keyword = add_keyword(T_SCRIBE, "scribe");
        break;
    default:
        break;
    }

    if (!valid_keyword) {
        add_identifier();
    }
}
