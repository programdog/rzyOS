#ifndef RZYOS_TASK_H
#define RZYOS_TASK_H

#include "rzyOS_event.h"

#define RZYOS_TASK_STATUS_READY 0
#define RZYOS_TASK_STATUS_DELAY (1 << 1)
#define RZYOS_TASK_STATUS_SUSPEND (1 << 2)
#define RZYOS_TASK_STATUS_DELETE (1 << 3)

//预留高16位作为事件等待标志
#define RZYOS_TASK_WAIT_MASK (0xff << 16)

typedef uint32_t tTaskStack;


typedef enum rzyOS_error_e
{
	error_no_error = 0,
	error_timeout = 1,
} rzyOS_error_e;

//task.h与event.h结构体互相编译, 则做一下 原型宣告 声明
typedef struct rzyOS_ecb_s rzyOS_ecb_s;

//task TCB
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

	//任务状态
	uint32_t task_status;

	//时间片
	uint32_t slice;
	
	//任务挂起计数器
	uint32_t suspend_count;
	
	//任务删除 ; 任务清除callback函数,和参数,和任务请求删除状态
	void (*clean)(void *param);
	void *clean_param;
	uint8_t request_delete_flag;
	
	//任务事件控制块
	rzyOS_ecb_s *wait_event;
	void *event_msg;
	//等待事件的结果
	uint32_t wait_event_result;
	//请求的事件类型
	uint32_t wait_flag_type; 
	//请求的事件标志
	uint32_t event_flag;
} task_tcb_s;


//task info struct
typedef struct rzyOS_task_info_s
{
	uint32_t delay_ticks;

	uint32_t prio;

	uint32_t task_status;

	uint32_t slice;

	uint32_t suspend_count;
} rzyOS_task_info_s;


void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack);


void rzyOS_task_suspend(task_tcb_s *task);

void rzyOS_task_wakeup(task_tcb_s *task);


void rzyOS_task_clean_callback(task_tcb_s *task, void (*clean)(void *param), void *param);

void rzyOS_task_force_delete(task_tcb_s *task);

void rzyOS_task_request_delete(task_tcb_s *task);

uint8_t rzyOS_task_request_delete_check(void);

void rzyOS_task_delete_self(void);


void rzyOS_task_get_info(task_tcb_s *task, rzyOS_task_info_s *info);

#endif
