#ifndef _PARSER_H
#define _PARSER_H

#include "ast/ast.h"

struct ast_node *ast_parse();
void             ast_clean(struct ast_node *);
void             ast_print(struct ast_node *, int level);

#endif
