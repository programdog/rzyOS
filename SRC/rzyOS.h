#ifndef __RZYOS_H
#define __RZYOS_H

#include <stdint.h>
#include "bitMapLib.h"
#include "osConfig.h"
#include "rzyOS_event.h"
#include "rzyOS_task.h"
#include "rzyOS_event.h"


extern task_tcb_s *currentTask;
extern task_tcb_s *nextTask;

void delay(int count);


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


void set_systick_period(uint32_t ms);

void task_delay(uint32_t delay);


void rzyOS_app_init(void);


void rzyOS_task_ready_list_remove(task_tcb_s *task_tcb);

void rzyOS_task_delay_list_remove(task_tcb_s *task_tcb);


#endif
