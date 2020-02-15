#ifndef RZYOS_EVENT_H
#define RZYOS_EVENT_H

#include "bitMapLib.h"
#include "rzyOS_task.h"

//事件类型
typedef enum rzyOS_event_type_e
{
	//未知事件类型，初始化时使用
	event_type_unknow = 0,
	//信号量事件类型
	event_type_semaphore = 1,
	//邮箱事件类型
	event_type_mbox = 2,
	//内存管理事件类型
	event_type_mem_block = 3,
} rzyOS_event_type_e;

//事件控制块
typedef struct rzyOS_ecb_s
{
	//事件类型
	rzyOS_event_type_e type;
	//事件等待列表
	list_t wait_list;
} rzyOS_ecb_s;


//任务希望请求的事件标志组时间类型
#define FLAGGROUP_CLEAR (0x0 << 0)
#define FLAGGROUP_SET (0x1 << 0)
#define FLAGGROUP_ANY (0x0 << 1)
#define FLAGGROUP_ALL (0x1 << 1)

#define FLAGGROUP_SET_ALL (FLAGGROUP_SET | FLAGGROUP_ALL)
#define FLAGGROUP_SET_ANY (FLAGGROUP_SET | FLAGGROUP_ANY)
#define FLAGGROUP_CLEAR_ALL (FLAGGROUP_CLEAR | FLAGGROUP_ALL)
#define FLAGGROUP_CLEAR_ANY (FLAGGROUP_CLEAR | FLAGGROUP_ANY)

#define FLAGGROUP_CONSUME (1 << 7)


//事件初始化
void rzyOS_event_init(rzyOS_ecb_s *ecb, rzyOS_event_type_e type);

//事件等待函数
void rzyOS_event_wait(rzyOS_ecb_s *rzyOS_ecb, task_tcb_s *task_tcb, void *msg, uint32_t state, uint32_t timeout);

//唤醒通知相关事件控制块
task_tcb_s *rzyOS_event_wakeup(rzyOS_ecb_s *rzyOS_ecb, void *msg, uint32_t result);

//把指定的任务从事件控制块强制移出
void rzyOS_event_remove(task_tcb_s *task_tcb, void *msg, uint32_t result);

//移出事件队列中所有的正在等待事件的任务
uint32_t rzyOS_event_remove_all(rzyOS_ecb_s *rzyOS_ecb, void *msg, uint32_t result);

//事件等待队列中,任务的数量
uint32_t rzyOS_event_wait_count(rzyOS_ecb_s *rzyOS_ecb);

#endif
