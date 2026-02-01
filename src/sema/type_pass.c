#include "sema/type_pass.h"
#include "ast/ast.h"
#include "defs.h"
#include "sema/declaration_order_pass.h"
#include "data_table/symbol_table.h"
#include "token.h"
#include "utils.h"
#include <stdio.h>

static bool tree_walker(struct ast_node *);
static bool type_promotion_lr(enum EXPRESSION_TYPE *, enum EXPRESSION_TYPE *);
static bool type_promotion_r(const enum EXPRESSION_TYPE *, enum EXPRESSION_TYPE *);

bool type_validator(struct ast_node *node) {
    return tree_walker(node);
}

static bool tree_walker(struct ast_node *node) {
    bool is_left_valid  = true;
    bool is_right_valid = true;

    if (node->left)
        is_left_valid = tree_walker(node->left);

    if (node->right)
        is_right_valid = tree_walker(node->right);

    if (node->ast_node_type == AST_DECL) {
        const char *symbol_name = get_symbol_name(node->value.id);
        if (!symbol_name) {
            fprintf(stderr, "Type check failed: symbol not found\n");
            return false;
        }

        if (check_declaration(node->value.id)) {
            fprintf(stderr, "Type check failed: redeclaration of symbol `%s`\n", symbol_name);
            return false;
        }

        set_declaration(node->value.id);
    }

    if (node->ast_node_type == AST_LVALUE || node->ast_node_type == AST_IDENTIFIER || node->ast_node_type == AST_FUNC_CALL) {
        const char *symbol_name = get_symbol_name(node->value.id);
        if (!symbol_name) {
            fprintf(stderr, "Type check failed: symbol not found\n");
            return false;
        }

        enum EXPRESSION_TYPE et = get_symbol_etype(node->value.id);
        if (et == ET_NONE) {
            fprintf(stderr, "Type check failed: symbol `%s` doesn't have a type\n", symbol_name);
            return false;
        }

        node->data_type = et;

        if (!check_declaration(node->value.id)) {
            fprintf(stderr, "Referencing `%s` before its declaration\n", symbol_name);
            return false;
        }

        return true;
    }

    if (node->ast_node_type == AST_STMT) {
        switch (node->token_type) {
        case T_EQ:
            if (!(node->right->data_type && node->left->data_type)) {
                fprintf(stderr, "Either side of assignment doesn't have a type\n");
                BREAKPOINT;
                return false;
            }

            type_promotion_r(&node->right->data_type, &node->left->data_type);
            node->data_type = node->right->data_type;
        case T_IF:
            if (node->left->data_type == ET_VOID || node->left->data_type == ET_NONE) {
                fprintf(stderr, "Type checking: void type on if predicate\n");
                return false;
            }
        default:
            break;
        }
    }

    if (node->ast_node_type == AST_BIN_OP) {
        type_promotion_lr(&node->left->data_type, &node->right->data_type);
        node->data_type = node->left->data_type;
    }

    if (node->ast_node_type == AST_UN_OP || node->ast_node_type == AST_GROUPING) {
        node->data_type = node->left->data_type;
    }

    if (node->ast_node_type == AST_LITERAL_NUMBER) {
        if (node->token_type == T_INTLIT) {
            if (node->value.intval < 256) {
                node->data_type = ET_CHAR;
            }

            if (node->value.intval < 2147483648) {
                node->data_type = ET_INT;
            }

            node->data_type = ET_LONG;
        }
    }

    if (node->ast_node_type == AST_FUNC_CALL) {
        const char *symbol_name = get_symbol_name(node->value.id);
        if (!symbol_name) {
            fprintf(stderr, "Type check failed: symbol not found\n");
        }

        enum STRUCTURE_TYPE symbol_stype = get_symbol_stype(node->value.id);
        if (symbol_stype != ST_FUNCTION) {
            fprintf(stderr, "Type check failed: identifier `%s` is not a function\n", symbol_name);
            return false;
        }
    }

    return is_left_valid && is_right_valid;
}

static bool type_promotion_lr(enum EXPRESSION_TYPE *left, enum EXPRESSION_TYPE *right) {
    if (*left == ET_VOID || *right == ET_VOID)
        return false;

    if (*left == ET_CHAR && *right == ET_INT) {
        *left = ET_INT;
        return true;
    }

    if (*left == ET_INT && *right == ET_CHAR) {
        *right = ET_INT;
        return true;
    }

    return true;
}

static bool type_promotion_r(const enum EXPRESSION_TYPE *left, enum EXPRESSION_TYPE *right) {
    *right = *left;

    return true;
}
