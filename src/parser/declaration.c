#include "parser/declaration.h"
#include "ast/ast.h"
#include "defs.h"
#include "lexer.h"
#include "parser/statement.h"
#include "parser/utils.h"
#include "symbol_table.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

struct ast_node *top_level_declaration() {
    if (match(T_EOF)) {
        return NULL;
    }

    struct token    current_token = get_current_token();
    enum TOKEN_TYPE type_token    = current_token.type;
    if (is_type_token(type_token)) {
        match(type_token);
        identifier();

        struct token id_token = get_previous_token();
        set_symbol_stype(id_token.value.id, ST_VARIABLE);
        set_symbol_etype(id_token.value.id, get_expression_type(type_token));

        if (get_current_token().type == T_LPAREN) {
            set_symbol_stype(id_token.value.id, ST_FUNCTION);
            struct ast_node *name = mk_leaf(AST_DECL, type_token, id_token.value);
            l_paren();
            r_paren();

            struct ast_node *body = statements_block();
            return mk_node(AST_FUNC, 0, name, body);
        }

        semicolon();
        return mk_leaf(AST_DECL, type_token, id_token.value);
    }

    current_token = get_current_token();
    printf("Unexpected token: `%.*s` on line %d\n", (int)current_token.lexeme_length, current_token.lexeme_start, current_token.line);
    exit(1);
    return NULL;
}
