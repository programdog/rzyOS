#ifndef __RZYOS_H
#define __RZYOS_H

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct tTask
{
	tTaskStack *stack;
} tTask; 

#endif
