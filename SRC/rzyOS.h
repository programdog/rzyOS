#ifndef __RZYOS_H
#define __RZYOS_H

#include <stdint.h>
#include "bitMapLib.h"
#include "osConfig.h"

typedef uint32_t tTaskStack;

typedef struct task_tcb_s
{
	tTaskStack *stack;
	uint32_t delayTicks;
	uint32_t prio;
} task_tcb_s; 

extern task_tcb_s *currentTask;
extern task_tcb_s *nextTask;

uint32_t task_enter_critical(void);
void task_exit_critical(uint32_t status);

void task_run_first(void);
void task_switch(void);

void task_schedule(void);
void task_schedule_init(void);
void task_schedule_disable(void);
void task_schedule_enable(void);
#endif
