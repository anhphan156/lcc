#ifndef _BREAKPOINT_H
#define _BREAKPOINT_H

#ifdef DEBUG_BUILD
#define BREAKPOINT __asm__ volatile("int3");
#endif

#endif
