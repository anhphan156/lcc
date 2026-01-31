#ifndef _AST_H
#define _AST_H

#include "defs.h"
#include "token.h"
enum AST_NODE_TYPE { AST_STMTS_BLOCK, AST_STMT, AST_DECL, AST_FUNC, AST_FUNC_CALL, AST_LVALUE, AST_BIN_OP, AST_UN_OP, AST_LITERAL_NUMBER, AST_DOUBLE, AST_GROUPING, AST_IDENTIFIER };

struct ast_node {
    struct ast_node     *left;
    struct ast_node     *right;
    union token_literal  value;
    enum AST_NODE_TYPE   ast_node_type;
    enum TOKEN_TYPE      token_type;
    enum EXPRESSION_TYPE data_type;
};

struct ast_node *mk_node(enum AST_NODE_TYPE, enum TOKEN_TYPE, struct ast_node *, struct ast_node *);
struct ast_node *mk_leaf(enum AST_NODE_TYPE, enum TOKEN_TYPE, union token_literal);

#endif
