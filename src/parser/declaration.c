#include "parser/declaration.h"
#include "ast/ast.h"
#include "defs.h"
#include "lexer/lexer.h"
#include "parser/statement.h"
#include "parser/utils.h"
#include "data_table/symbol_table.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

static struct ast_node *function_definition(struct token *, struct token *);

struct ast_node *top_level_declaration() {
    if (match(T_EOF)) {
        return NULL;
    }

    if (match_type_with_void()) {
        struct token type_token = get_previous_token();

        identifier();
        struct token id_token = get_previous_token();
        set_symbol_stype(id_token.value.id, ST_VARIABLE);
        set_symbol_etype(id_token.value.id, get_expression_type(type_token.type));

        if (get_current_token().type == T_LPAREN) {
            return function_definition(&type_token, &id_token);
        }

        semicolon();
        return mk_leaf(AST_DECL, type_token.type, id_token.value);
    }

    struct token current_token = get_current_token();
    printf("Unexpected token: `%.*s` on line %d\n", (int)current_token.lexeme_length, current_token.lexeme_start, current_token.line);
    exit(1);
    return NULL;
}

static struct ast_node *function_definition(struct token *type_token, struct token *id_token) {
    set_symbol_stype(id_token->value.id, ST_FUNCTION);
    struct ast_node *name = mk_leaf(AST_DECL, type_token->type, id_token->value);
    l_paren();

    while (match_type()) {
        identifier();
        if (!match(T_COMMA))
            break;
    }

    r_paren();

    struct ast_node *body = statements_block();

    return mk_node(AST_FUNC, 0, name, body);
}
