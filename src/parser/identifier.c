#include "parser/identifier.h"
#include "ast/ast.h"
#include "lexer/lexer.h"
#include "parser/expression.h"
#include "parser/utils.h"
#include "token.h"
#include <stdio.h>

static struct ast_node *function_call(struct token *);

struct ast_node *parse_identifier() {
    identifier();
    struct token id_token = get_previous_token();

    struct token current_token = get_current_token();
    if (current_token.type == T_LPAREN) {
        return function_call(&id_token);
    }

    if (current_token.type == T_EQ) {
        return assignment(&id_token);
    }

    return mk_leaf(AST_IDENTIFIER, id_token.type, id_token.value);
}

struct ast_node *assignment(struct token *id_token) {
    if (!id_token) {
        identifier();
    }

    struct token id_token_local = get_previous_token();
    id_token                    = &id_token_local;

    match(T_EQ);

    struct ast_node *right = mk_leaf(AST_LVALUE, T_IDENTIFIER, id_token->value);
    struct ast_node *left  = expression();

    return mk_node(AST_STMT, T_EQ, left, right);
}

static struct ast_node *function_call(struct token *id_token) {
    if (!id_token) {
        identifier();
    }

    struct token id_token_local = get_previous_token();
    id_token                    = &id_token_local;

    if (match(T_LPAREN)) {
        if (!match(T_RPAREN)) {
            while (expression()) {
                if (!match(T_COMMA)) {
                    break;
                }
            }
            match(T_RPAREN);
        }
        return mk_leaf(AST_FUNC_CALL, 0, id_token->value);
    }

    return NULL;
}
