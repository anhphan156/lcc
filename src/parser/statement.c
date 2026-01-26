#include "parser/statement.h"
#include "ast/ast.h"
#include "lexer.h"
#include "parser/expression.h"
#include <stdio.h>
#include <stdlib.h>

struct ast_node *statements_block() {
    struct ast_node *block = statement();

    struct ast_node *stmt;
    while ((stmt = statement()) != NULL) {
        block = mk_node(AST_STMTS_BLOCK, 0, block, stmt);
    }

    return block;
}

struct ast_node *statement() {
    struct token current_token;
    if (lexical_scan(&current_token) == -1) {
        // EOF
        return NULL;
    }

    int              current_line = current_token.line;
    struct ast_node *stmt         = NULL;

    switch (current_token.type) {
    case T_SCRIBE: {
        stmt = mk_node(AST_STMT, T_SCRIBE, expression(), NULL);

        if (lexical_scan(&current_token) == -1) {
            fprintf(stderr, "Expected a `;` on line %d\n", current_line);
            exit(1);
        }

        if (current_token.type != T_SEMICOLON) {
            fprintf(stderr, "Expected a `;` on line %d\n", current_line);
            exit(1);
        }

        break;
    }
    default:
        fprintf(stderr, "Unexpected token on line %d\n", current_token.line);
        exit(1);
        break;
    }

    return stmt;
}
