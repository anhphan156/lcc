#ifndef _PARSER_H
#define _PARSER_H

#include <stddef.h>
struct ast_node;

typedef struct ast_array {
    size_t            len;
    size_t            cap;
    struct ast_node **item;
} AstArray;

AstArray *ast_parse();
void      ast_clean(struct ast_node *);

#endif
