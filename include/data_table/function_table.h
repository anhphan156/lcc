#ifndef FUNCTION_TABLE_H
#define FUNCTION_TABLE_H

#include "defs.h"
#include <stdint.h>

struct function_info {
    uint8_t               args_len;
    uint8_t               args_cap;
    enum EXPRESSION_TYPE *args_type;
    enum EXPRESSION_TYPE  ret_type;
};

#endif
