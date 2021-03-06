#ifndef RZYOS_MUTEX_H
#define RZYOS_MUTEX_H

#include "rzyOS_event.h"
#include "rzyOS_schedule.h"

//互斥锁结构
typedef struct rzyOS_mutex_s
{
	//事件控制块
	rzyOS_ecb_s rzyOS_ecb;
	//锁定次数计数
	uint32_t lock_count;
	//拥有者task指针
	task_tcb_s *owner;
	//拥有者task原始优先级
	uint32_t owner_original_prio;
} rzyOS_mutex_s;

////互斥锁信息
typedef struct rzyOS_mutex_info_s
{
	//当前等待事件组的任务计数
	uint32_t task_count;
	//拥有者的优先级
	uint32_t owner_prio;
	//继承优先级
	uint32_t inherit_prio;
	//拥有者task指针
	task_tcb_s *owner;
	//锁定次数计数
	uint32_t lock_count;
} rzyOS_mutex_info_s;

//互斥锁初始化函数
void rzyOS_mutex_init(rzyOS_mutex_s *rzyOS_mutex);

//互斥锁等待函数(阻塞)
uint32_t rzyOS_mutex_wait(rzyOS_mutex_s *rzyOS_mutex, uint32_t wait_time);

//互斥锁等待函数(非阻塞)
uint32_t rzyOS_mutex_no_wait(rzyOS_mutex_s *rzyOS_mutex);

//互斥锁释放函数
uint32_t rzyOS_mutex_post(rzyOS_mutex_s *rzyOS_mutex);

//互斥锁销毁函数
uint32_t rzyOS_mutex_destroy(rzyOS_mutex_s *rzyOS_mutex);

//互斥锁信息获取函数
void rzyOS_mutex_get_info(rzyOS_mutex_s *rzyOS_mutex, rzyOS_mutex_info_s *rzyOS_mutex_info);

#endif
