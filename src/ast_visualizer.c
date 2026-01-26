#include "ast/ast_visualizer.h"
#include "ast/ast.h"
#include "token.h"
#include <stdint.h>
#include <stdio.h>

static FILE    *dot_file = NULL;
static uint32_t node_id  = 1;
static uint32_t ast_walker(struct ast_node *node);

void write_dot_graph(struct ast_node *node) {
    if (node == NULL) {
        return;
    }

    dot_file = fopen("ast.dot", "w");
    fprintf(dot_file, "graph{");
    ast_walker(node);
    fprintf(dot_file, "}");
    fclose(dot_file);
}

static uint32_t ast_walker(struct ast_node *node) {
    uint32_t left_id = 0, right_id = 0;

    if (node->left) {
        left_id = ast_walker(node->left);
    }

    if (node->right) {
        right_id = ast_walker(node->right);
    }

    if (node->ast_node_type == AST_GLUE) {
        fprintf(dot_file, "%d [label = glue];", node_id);
    }

    if (node->ast_node_type == AST_STMT) {
        if (node->token_type == T_SCRIBE) {
            fprintf(dot_file, "%d [label = scribe];", node_id);
        }
    }

    if (node->ast_node_type == AST_BIN_OP) {
        if (node->token_type == T_PLUS)
            fprintf(dot_file, "%d [label = add];", node_id);
        if (node->token_type == T_MINUS)
            fprintf(dot_file, "%d [label = minus];", node_id);
        if (node->token_type == T_STAR)
            fprintf(dot_file, "%d [label = multiply];", node_id);
        if (node->token_type == T_SLASH)
            fprintf(dot_file, "%d [label = divide];", node_id);
    }

    if (node->ast_node_type == AST_INTEGER) {
        fprintf(dot_file, "%d [label = %ld];", node_id, node->value.intval);
    }

    if (left_id != 0)
        fprintf(dot_file, "%d -- %d;", node_id, left_id);

    if (right_id != 0)
        fprintf(dot_file, "%d -- %d;", node_id, right_id);

    return node_id++;
}
