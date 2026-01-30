#ifndef _AST_VISUALIZER_H
#define _AST_VISUALIZER_H

struct ast_node;
void open_dot_graph();
void write_dot_graph(struct ast_node *, int);
void close_dot_graph();

#endif
