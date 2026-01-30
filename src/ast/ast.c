#include "ast/ast.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"

struct ast_node *mk_node(enum AST_NODE_TYPE ast_node_type, enum TOKEN_TYPE token_type, struct ast_node *left, struct ast_node *right) {
    struct ast_node *node = (struct ast_node *)malloc(sizeof(struct ast_node));
    if (node == NULL) {
        fprintf(stderr, "Unable to create new ast_node");
        BREAKPOINT;
        exit(1);
    }
    node->ast_node_type = ast_node_type;
    node->token_type    = token_type;
    node->left          = left;
    node->right         = right;

    union token_literal empty_literal = {0};
    node->value                       = empty_literal;

    return node;
}

struct ast_node *mk_leaf(enum AST_NODE_TYPE type, enum TOKEN_TYPE token_type, union token_literal value) {
    struct ast_node *node = (struct ast_node *)malloc(sizeof(struct ast_node));
    if (node == NULL) {
        fprintf(stderr, "Unable to create new ast_node");
        BREAKPOINT;
        exit(1);
    }
    node->ast_node_type = type;
    node->token_type    = token_type;
    node->value         = value;
    node->left          = NULL;
    node->right         = NULL;

    return node;
}
