#ifndef _ASM_FILE_H
#define _ASM_FILE_H

#include <stdbool.h>
#include <stdio.h>

bool  asmfopen(const char *);
void  asmfclose(void);
FILE *asmfget(void);

#endif
