#ifndef __RZYOS_H
#define __RZYOS_H

#include <stdint.h>
#include "bitMapLib.h"
#include "osConfig.h"

#define RZYOS_TASK_STATUS_READY 0
#define RZYOS_TASK_STATUS_DELAY 1

typedef uint32_t tTaskStack;

typedef struct task_tcb_s
{
	//任务堆栈指针
	tTaskStack *stack;

	//任务节点
	node_t link_node;

	//任务注册延时周期
	uint32_t delayTicks;

	//任务延时节点
	node_t delay_node;

	//任务优先级
	uint32_t prio;

	//任务就绪状态(需要等待 or 已经就绪)
	uint32_t ready_status;

	//时间片
	uint32_t slice;
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

void task_insert_ready_list(task_tcb_s *task_tcb);
void task_remove_ready_list(task_tcb_s *task_tcb);
void delay_list_insert_time_node(task_tcb_s *task_tcb, uint32_t ticks);
void delay_list_remove_time_node(task_tcb_s *task_tcb);
void task_systemtick_handler(void);
void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack);
void set_systick_period(uint32_t ms);
void task_delay(uint32_t delay);

#endif
