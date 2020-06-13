#ifndef __RZYOS_SCHEDULE_H
#define __RZYOS_SCHEDULE_H

#include <stdint.h>
#include "rzyOS_bitmap.h"
#include "rzyOSconfig.h"
#include "rzyOS_event.h"
#include "rzyOS_task.h"

//当前任务的指针
extern task_tcb_s *currentTask;

//下一个任务的指针
extern task_tcb_s *nextTask;



//从位图中找到就绪的最高优先级
task_tcb_s *task_highest_ready(void);


//不会引起任务调度的延时
void delay(int count);

//进入临界区保护
uint32_t task_enter_critical(void);

//退出临界区保护
void task_exit_critical(uint32_t status);


//从MSP转换PSP,配置pendsv优先级
void task_run_first(void);


//触发pendsv中断
void task_switch(void);


//task调度
void task_schedule(void);

//初始化位图
void task_schedule_init(void);

//调度失能 , 计数调度锁 + 1
void task_schedule_disable(void);

//调度使能 , 计数调度锁 - 1
void task_schedule_enable(void);


//按照优先级,把就绪的任务的任务节点插入就绪任务list中
void task_insert_ready_list(task_tcb_s *task_tcb);

//把任务从就绪队列中删移出(内部使用)
void task_remove_ready_list(task_tcb_s *task_tcb);

//在延时list中加入当前任务的延时节点
void delay_list_insert_time_node(task_tcb_s *task_tcb, uint32_t ticks);

//把任务在延时队列中删除(内部调用)
void delay_list_remove_time_node(task_tcb_s *task_tcb);

//systick中断调用此函数
void task_systemtick_handler(void);

//systick中断周期配置(sim)
void set_systick_period(uint32_t ms);

//systick中断周期配置(STM32F407)
void rzyOS_systick_init(uint8_t sysclk);

//systick中断使能
void rzyOS_systick_enable(void);

//延时函数，将触发调度
void task_delay(uint32_t delay);

//任务初始化
void rzyOS_app_init(void);

//把任务从就绪队列中移出
void rzyOS_task_ready_list_remove(task_tcb_s *task_tcb);

//把任务在延时队列中删除(外部调用)
void rzyOS_task_delay_list_remove(task_tcb_s *task_tcb);

//获取cpu使用率，并返回
float rzyOS_get_cpu_usage(void);



void rzyOS_kernel_init(void);

void rzyOS_start(void);


#endif
