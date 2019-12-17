#include "rzyOS_semaphore.h"

void rzyOS_sem_init(rzyOS_sem_s *sem, uint32_t start_count, uint32_t max_count)
{
	rzyOS_event_init(&(sem -> rzyOS_ecb), event_type_semaphore);
	
	sem -> max_count = max_count;
	
	//不限制最大信号值
	if (NOLIMITE_MAX_COUNT == max_count)
	{
		sem -> count = start_count;
	}
	else
	{
		sem -> count = (start_count > max_count) ? max_count : start_count;
	}
}

//信号量等待函数
//wait_ticks ： 超时等待时间
uint32_t rzyOS_sem_wait(rzyOS_sem_s *sem, uint32_t wait_ticks)
{
	uint32_t status = task_enter_critical();

	if (sem -> count > 0)
	{
		sem --;
		task_exit_critical(status);
		return error_no_error;
	}
	else
	{
		rzyOS_event_wait(&(sem -> rzyOS_ecb), currentTask, (void *)0, event_type_semaphore, wait_ticks);
		task_exit_critical(status);

		task_schedule();

		return currentTask -> wait_event_result;
	}
}
