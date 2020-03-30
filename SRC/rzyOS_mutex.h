#ifndef RZYOS_MUTEX_H
#define RZYOS_MUTEX_H

#include "rzyOS_event.h"
#include "rzyOS.h"

typedef struct rzyOS_mutex_s
{
	//事件控制块
	rzyOS_ecb_s rzyOS_ecb;
	uint32_t lock_count;
	task_tcb_s *owner;
	uint32_t owner_original_prio;
} rzyOS_mutex_s;

void rzyOS_mutex_init(rzyOS_mutex_s *rzyOS_mutex);

#endif
