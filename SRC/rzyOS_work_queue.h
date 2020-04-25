#ifndef RZYOS_WORK_QUEUE_H
#define RZYOS_WORK_QUEUE_H

#include "rzyOS.h"
#include "rzyOS_semaphore.h"


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

//工作队列节点信息结构
typedef struct rzyOS_wqueue_info_s
{
	//启示执行延时时间
	uint32_t start_delay_tick;
	//周期执行时间
	uint32_t period_tick;
	//回调函数
	worker_t worker;
	//回调函数参数
	void *arg;
	//工作队列配置
	uint32_t wqueue_config;
	//当前状态
	rzyOS_wqueue_status_e rzyOS_wqueue_status;
} rzyOS_wqueue_info_s;

//工作队列节点结构
typedef struct rzyOS_wqueue_s
{
	//工作队列节点
	node_t node;
	//启示执行延时时间
	uint32_t start_delay_tick;
	//周期执行时间
	uint32_t period_tick;
	//时间计数
	uint32_t count_tick;
	//回调函数
	worker_t worker;
	//回调函数参数
	void *arg;
	//工作队列配置
	uint32_t wqueue_config;
	//当前状态
	rzyOS_wqueue_status_e rzyOS_wqueue_status;
} rzyOS_wqueue_s;

//高速工作队列
#define HIGH_WORK_QUEUE 1
//低速工作队列
#define LOW_WORK_QUEUE 0

//工作队列节点初始化函数
void rzyOS_wqueue_init(rzyOS_wqueue_s *rzyOS_wqueue, uint32_t start_delay_tick, uint32_t period_tick, worker_t worker, void *arg, uint32_t wqueue_config);

//启动指定工作队列节点(工作队列组件的外部任务调用)
void rzyOS_wqueue_start(rzyOS_wqueue_s *rzyOS_wqueue);

//停止指定工作队列节点(工作队列组件的外部任务调用)
void rzyOS_wqueue_stop(rzyOS_wqueue_s *rzyOS_wqueue);

//销毁工作队列节点
void rzyOS_wqueue_destroy(rzyOS_wqueue_s *rzyOS_wqueue);

//工作队列节点信息获取函数
void rzyOS_wqueue_get_info(rzyOS_wqueue_s *rzyOS_wqueue, rzyOS_wqueue_info_s *rzyOS_wqueue_info);

//系统节拍， tick周期性工作队列处理函数
void rzyOS_wqueue_tick_handle(void);

//工作队列模块初始化
void rzyOS_wqueue_module_init(void);

void rzyOS_wqueue_task_init(void);

#endif
