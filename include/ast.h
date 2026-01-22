#ifndef _AST_H
#define _AST_H

#include "token.h"
enum AST_NODE_TYPE { AST_BIN_OP, AST_INTEGER, AST_DOUBLE, AST_BLOCK };

struct ast_node {
    struct ast_node    *left;
    struct ast_node    *right;
    union token_literal value;
    enum AST_NODE_TYPE  ast_node_type;
    enum TOKEN_TYPE     token_type;
};

#endif
