#include "codegen/asm_file.h"
#include <stdio.h>

static FILE *asm_stream = NULL;

bool asmfopen(const char *path) {
    asm_stream = fopen(path, "w");
    return asm_stream != NULL;
}

void asmfclose(void) {
    if (fclose(asm_stream) == EOF) {
        perror("fclose");
    }
}

FILE *asmfget(void) {
    return asm_stream;
}
