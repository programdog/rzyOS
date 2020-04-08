#ifndef __RZYOS_SEMAPHORE_H
#define __RZYOS_SEMAPHORE_H

#include <stdint.h>
#include "rzyOS_event.h"
#include "rzyOS.h"

//不限制最大计数信号值
#define NOLIMITE_MAX_COUNT 0

//信号量结构体
typedef struct rzyOS_sem_s
{
	//事件控制块
	rzyOS_ecb_s rzyOS_ecb;
	//计数值
	uint32_t count;
	//最大支持计数值
	//max_count = 0 : 计数值无限制
	uint32_t max_count;
} rzyOS_sem_s;

//信号量信息
typedef struct rzyOS_sem_info_s
{
	//当前信号量计数值
	uint32_t sem_count;
	//当前信号量最大支持计数值
	uint32_t max_count;
	//当前信号量等待的任务数量
	uint32_t task_count;
} rzyOS_sem_info_s;

//信号量初始化函数
void rzyOS_sem_init(rzyOS_sem_s *sem, uint32_t start_count, uint32_t max_count);

//信号量阻塞等待函数
uint32_t rzyOS_sem_wait(rzyOS_sem_s *sem, uint32_t wait_ticks);

//信号量非阻塞等待函数
uint32_t rzyOS_sem_no_wait(rzyOS_sem_s *sem);

//信号量释放函数
void rzyOS_sem_post(rzyOS_sem_s *sem);

//信号量销毁
uint32_t rzyOS_sem_destroy(rzyOS_sem_s *sem);

//信号量信息获得函数
void rzyOS_sem_get_info(rzyOS_sem_s *sem, rzyOS_sem_info_s *sem_info);


#endif
