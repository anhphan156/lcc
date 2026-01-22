#ifndef _AST_H
#define _AST_H

enum NODE_TYPE { BIN_OP, INTEGER, FLOAT, BLOCK };

struct ast {
    enum NODE_TYPE type;
};

#endif
