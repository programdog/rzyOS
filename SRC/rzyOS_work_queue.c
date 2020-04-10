#include "rzyOS_work_queue.h"

void rzyOS_queue_init(rzyOS_wqueue_s *rzyOS_wqueue, uint32_t start_delay_tick, uint32_t period_tick, worker_t worker, void *arg, uint32_t wqueue_config)
{
	node_init(&(rzyOS_wqueue -> node));
	rzyOS_wqueue -> start_delay_tick = start_delay_tick;
	rzyOS_wqueue -> period_tick = period_tick;

	if (0 == start_delay_tick)
	{
		rzyOS_wqueue -> count_tick = period_tick;
	}
	else
	{
		rzyOS_wqueue -> count_tick = start_delay_tick;
	}
	
	rzyOS_wqueue -> worker = worker;
	rzyOS_wqueue -> arg = arg;
	rzyOS_wqueue -> wqueue_config = wqueue_config;
	rzyOS_wqueue -> rzyOS_wqueue_status = wqueue_create;
}
