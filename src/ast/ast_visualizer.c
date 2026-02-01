#include "ast/ast_visualizer.h"
#include "ast/ast.h"
#include "token.h"
#include <stdint.h>
#include <stdio.h>

static FILE    *dot_file = NULL;
static uint32_t node_id  = 1;
static uint32_t ast_walker(struct ast_node *node);

void open_dot_graph() {
    dot_file = fopen("ast.dot", "w");
    fprintf(dot_file, "graph{");
}

void close_dot_graph() {
    fprintf(dot_file, "}");
    fclose(dot_file);
}

void write_dot_graph(struct ast_node *node, int id) {
    if (node == NULL) {
        return;
    }

    fprintf(dot_file, "subgraph cluster_%d {", id);
    ast_walker(node);
    fprintf(dot_file, "}");
}

static uint32_t ast_walker(struct ast_node *node) {
    uint32_t left_id = 0, right_id = 0;

    if (node->left) {
        left_id = ast_walker(node->left);
    }

    if (node->right) {
        right_id = ast_walker(node->right);
    }

    if (node->ast_node_type == AST_FUNC) {
        fprintf(dot_file, "%d [label = function];", node_id);
    }

    if (node->ast_node_type == AST_FUNC_CALL) {
        fprintf(dot_file, "%d [label = \"call f[%d]\"];", node_id, node->value.id);
    }

    if (node->ast_node_type == AST_STMTS_BLOCK) {
        fprintf(dot_file, "%d [label = stmts_block];", node_id);
    }

    if (node->ast_node_type == AST_STMT) {
        if (node->token_type == T_PRINT) {
            fprintf(dot_file, "%d [label = print];", node_id);
        }

        if (node->token_type == T_RETURN) {
            fprintf(dot_file, "%d [label = return];", node_id);
        }

        if (node->token_type == T_EQ) {
            fprintf(dot_file, "%d [label = \"=\"];", node_id);
        }

        if (node->token_type == T_WHILE) {
            fprintf(dot_file, "%d [label = \"while\"];", node_id);
        }

        if (node->token_type == T_FOR) {
            fprintf(dot_file, "%d [label = \"for\"];", node_id);
        }

        if (node->token_type == T_IF) {
            fprintf(dot_file, "%d [label = \"if\"];", node_id);
        }
    }

    if (node->ast_node_type == AST_IDENTIFIER) {
        fprintf(dot_file, "%d [label = \"ref: s[%d]\"];", node_id, node->value.id);
    }

    if (node->ast_node_type == AST_DECL) {
        fprintf(dot_file, "%d [label = \"decl: s[%d]\"];", node_id, node->value.id);
    }

    if (node->ast_node_type == AST_LVALUE) {
        fprintf(dot_file, "%d [label = \"lv: s[%d]\"];", node_id, node->value.id);
    }

    if (node->ast_node_type == AST_BIN_OP) {
        if (node->token_type == T_PLUS)
            fprintf(dot_file, "%d [label = \"bin[+]\"];", node_id);
        if (node->token_type == T_MINUS)
            fprintf(dot_file, "%d [label = \"bin[-]\"];", node_id);
        if (node->token_type == T_STAR)
            fprintf(dot_file, "%d [label = \"bin[*]\"];", node_id);
        if (node->token_type == T_SLASH)
            fprintf(dot_file, "%d [label = \"bin[/]\"];", node_id);
        if (node->token_type == T_ANDAND)
            fprintf(dot_file, "%d [label = \"bin[&&]\"];", node_id);
        if (node->token_type == T_OROR)
            fprintf(dot_file, "%d [label = \"bin[||]\"];", node_id);
        if (node->token_type == T_NOTEQ)
            fprintf(dot_file, "%d [label = \"bin[!=]\"];", node_id);
        if (node->token_type == T_EQEQ)
            fprintf(dot_file, "%d [label = \"bin[==]\"];", node_id);
        if (node->token_type == T_GT)
            fprintf(dot_file, "%d [label = \"bin[>]\"];", node_id);
        if (node->token_type == T_GE)
            fprintf(dot_file, "%d [label = \"bin[>=]\"];", node_id);
        if (node->token_type == T_LT)
            fprintf(dot_file, "%d [label = \"bin[<]\"];", node_id);
        if (node->token_type == T_LE)
            fprintf(dot_file, "%d [label = \"bin[<=]\"];", node_id);
    }
    if (node->ast_node_type == AST_UN_OP) {
        if (node->token_type == T_MINUS)
            fprintf(dot_file, "%d [label = \"un[-]\"];", node_id);
        if (node->token_type == T_NOT)
            fprintf(dot_file, "%d [label = \"un[!]\"];", node_id);
    }

    if (node->ast_node_type == AST_LITERAL_NUMBER) {
        fprintf(dot_file, "%d [label = %ld];", node_id, node->value.intval);
    }

    if (node->ast_node_type == AST_GROUPING) {
        fprintf(dot_file, "%d [label = \"()\"];", node_id);
    }

    if (left_id != 0)
        fprintf(dot_file, "%d -- %d;", node_id, left_id);

    if (right_id != 0)
        fprintf(dot_file, "%d -- %d;", node_id, right_id);

    return node_id++;
}
