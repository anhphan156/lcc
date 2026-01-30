#ifndef _UTILS_H
#define _UTILS_H

#ifdef DEBUG_BUILD
#define BREAKPOINT __asm__ volatile("int3");
#endif

void print_token_name(int);

#endif
