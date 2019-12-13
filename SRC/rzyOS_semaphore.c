#include "rzyOS_semaphore.h"

void rzyOS_sem_init(rzyOS_sem_s *sem, uint32_t start_count, uint32_t max_count)
{
	rzyOS_event_init(&(sem -> rzyOS_ecb), event_type_semaphore);
	
	sem -> max_count = max_count;
	
	if (NOLIMITE_MAX_COUNT == max_count)
	{
		sem -> count = start_count;
	}
	else
	{
		sem -> count = (start_count > max_count) ? max_count : start_count;
	}
}
