#ifndef _IDENTIFIER_H
#define _IDENTIFIER_H

struct ast_node;
struct token;

struct ast_node *parse_identifier();
struct ast_node *assignment(struct token *);

#endif
