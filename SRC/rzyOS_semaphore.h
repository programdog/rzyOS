#ifndef __RZYOS_SEMAPHORE_H
#define __RZYOS_SEMAPHORE_H

#include <stdint.h>
#include "rzyOS_event.h"

//不限制最大计数信号值
#define NOLIMITE_MAX_COUNT 0

typedef struct rzyOS_sem_s
{
	rzyOS_ecb_s rzyOS_ecb;
	//计数值
	uint32_t count;
	//最大支持计数值
	uint32_t max_count;
} rzyOS_sem_s;

//信号初始化
void rzyOS_sem_init(rzyOS_sem_s *sem, uint32_t start_count, uint32_t max_count);

#endif
