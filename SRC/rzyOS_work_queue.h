#ifndef RZYOS_WORK_QUEUE_H
#define RZYOS_WORK_QUEUE_H

#include "rzyOS_event.h"

//Defines the work callback
typedef void (*worker_t)(void *arg);

//工作队列状态
typedef enum rzyOS_wqueue_status_e
{
	//创建状态
	wqueue_create,
	//开始状态
	wqueue_start,
	//运行状态
	wqueue_running,
	//停止状态
	wqueue_stop,
	//销毁状态
	wqueue_destroy,
} rzyOS_wqueue_status_e;


typedef struct rzyOS_wqueue_s
{
	node_t node;
	uint32_t start_delay_tick;
	uint32_t period_tick;
	uint32_t count_tick;
	worker_t worker;
	void *arg;
	uint32_t wqueue_config;
	rzyOS_wqueue_status_e rzyOS_wqueue_status;
} rzyOS_wqueue_s;

//高速工作队列
#define HIGH_WORK_QUEUE 1
//低速工作队列
#define LOW_WORK_QUEUE 0

//工作队列初始化函数
void rzyOS_queue_init(rzyOS_wqueue_s *rzyOS_wqueue, uint32_t start_delay_tick, uint32_t period_tick, worker_t worker, void *arg, uint32_t wqueue_config);

#endif
