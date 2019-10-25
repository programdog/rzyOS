#ifndef __RZYOS_H
#define __RZYOS_H

#include <stdint.h>
#include "bitMapLib.h"
#include "osConfig.h"

typedef uint32_t tTaskStack;

typedef struct tTask
{
	tTaskStack *stack;
	uint32_t delayTicks;
	uint32_t prio;
} tTask; 

extern tTask *currentTask;
extern tTask *nextTask;

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);

void tTaskRunFirst(void);
void tTaskSwitch(void);

void tTaskSched(void);
void tTaskSchedInit(void);
void tTaskSchedDisable(void);
void tTaskschedEnable(void);
#endif
