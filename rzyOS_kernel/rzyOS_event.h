#ifndef RZYOS_EVENT_H
#define RZYOS_EVENT_H

#include "rzyOS_bitmap.h"
#include "rzyOS_task.h"
#include "rzyOS_schedule.h"


//事件类型
typedef enum rzyOS_event_type_e
{
	//未知事件类型，初始化时使用
	event_type_unknow = 0,
	//信号量事件类型
	event_type_semaphore = 1,
	//消息队列事件类型
	event_type_msg_queue = 2,
	//内存管理事件类型
	event_type_mem_block = 3,
	//事件组
	event_type_flag_group = 4,
	//互斥锁
	event_type_mutex = 5,
} rzyOS_event_type_e;

//事件控制块
typedef struct rzyOS_ecb_s
{
	//事件类型
	rzyOS_event_type_e type;
	//事件等待列表
	list_t wait_list;
} rzyOS_ecb_s;


//事件初始化
void rzyOS_event_init(rzyOS_ecb_s *ecb, rzyOS_event_type_e type);

//把任务插入到事件的等待队列
void rzyOS_event_wait(rzyOS_ecb_s *rzyOS_ecb, task_tcb_s *task_tcb, void *msg, uint32_t state, uint32_t timeout);

//唤醒通知相关事件控制块
task_tcb_s *rzyOS_event_wakeup(rzyOS_ecb_s *rzyOS_ecb, void *msg, uint32_t result);

//把指定的任务从事件控制块强制移出
void rzyOS_event_remove(task_tcb_s *task_tcb, void *msg, uint32_t result);

//
task_tcb_s *rzyOS_event_wakeup_appoint_task(rzyOS_ecb_s *rzyOS_ecb, task_tcb_s *task_tcb, void *msg, uint32_t result);

//移出事件队列中所有的正在等待事件的任务
uint32_t rzyOS_event_remove_all(rzyOS_ecb_s *rzyOS_ecb, void *msg, uint32_t result);

//事件等待队列中,任务的数量
uint32_t rzyOS_event_wait_count(rzyOS_ecb_s *rzyOS_ecb);

#endif
