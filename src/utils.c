#include "utils.h"
#include "token.h"
#include <stdio.h>

static const char *token_name[] = {
#define X(name) #name,
    TOKEN_LIST
#undef X
};

void print_token_name(int id) {
    printf("%s", token_name[id]);
}
