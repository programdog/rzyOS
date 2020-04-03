#include "rzyOS_mutex.h"


//互斥锁初始化函数
void rzyOS_mutex_init(rzyOS_mutex_s *rzyOS_mutex)
{
	//互斥锁事件结构初始化
	rzyOS_event_init(&(rzyOS_mutex -> rzyOS_ecb), event_type_mutex);

	//互斥锁事件结构字段初始化
	rzyOS_mutex -> lock_count = 0;
	rzyOS_mutex -> owner = (task_tcb_s *)0;
	rzyOS_mutex -> owner_original_prio = RZYOS_PRIO_COUNT;
}

uint32_t rzyOS_mutex_wait(rzyOS_mutex_s *rzyOS_mutex, uint32_t wait_time)
{
	uint32_t status = task_enter_critical();

	if (rzyOS_mutex -> lock_count <= 0)
	{
		rzyOS_mutex -> lock_count ++;
		rzyOS_mutex -> owner = currentTask;
		rzyOS_mutex -> owner_original_prio = currentTask -> prio;

		task_exit_critical(status);

		return error_no_error;
	}
	else
	{
		if (currentTask == rzyOS_mutex -> owner)
		{
			rzyOS_mutex -> lock_count ++;

			task_exit_critical(status);

			return error_no_error;
		}
		else
		{
			if (currentTask -> prio < rzyOS_mutex -> owner -> prio)
			{
				if (RZYOS_TASK_STATUS_READY == rzyOS_mutex -> owner -> task_status)
				{
					task_remove_ready_list(rzyOS_mutex -> owner);
					rzyOS_mutex -> owner -> prio = currentTask -> prio;
					task_insert_ready_list(rzyOS_mutex -> owner);
				}
				else
				{
					rzyOS_mutex -> owner -> prio = currentTask -> prio;
				}
			}

			rzyOS_event_wait(&(rzyOS_mutex -> rzyOS_ecb), currentTask, (void *)0, event_type_mutex, wait_time);
			task_exit_critical(status);
			task_schedule();
			return currentTask -> wait_event_result;
		}
	}
}

uint32_t rzyOS_mutex_no_wait(rzyOS_mutex_s *rzyOS_mutex)
{
	uint32_t status = task_enter_critical();

	if (rzyOS_mutex -> lock_count <= 0)
	{
		rzyOS_mutex -> lock_count ++;
		rzyOS_mutex -> owner = currentTask;
		rzyOS_mutex -> owner_original_prio = currentTask -> prio;

		task_exit_critical(status);

		return error_no_error;
	}
	else
	{
		if (currentTask == rzyOS_mutex -> owner)
		{
			rzyOS_mutex -> lock_count ++;

			task_exit_critical(status);

			return error_no_error;
		}

		task_exit_critical(status);

		return error_resource_unvaliable;
	}
}

uint32_t rzyOS_mutex_post(rzyOS_mutex_s *rzyOS_mutex)
{
	uint32_t status = task_enter_critical();

	if (rzyOS_mutex -> lock_count <= 0)
	{
		task_exit_critical(status);

		return error_no_error;
	}

	if (currentTask != rzyOS_mutex -> owner)
	{
		task_exit_critical(status);

		return error_not_owner;
	}

	rzyOS_mutex -> lock_count --;

	if (rzyOS_mutex -> lock_count > 0)
	{
		task_exit_critical(status);

		return error_no_error;
	}
	
	if (rzyOS_mutex -> owner_original_prio != rzyOS_mutex -> owner -> prio)
	{
		if (RZYOS_TASK_STATUS_READY == rzyOS_mutex -> owner -> task_status)
		{
			task_remove_ready_list(rzyOS_mutex -> owner);
			currentTask -> prio = rzyOS_mutex -> owner_original_prio;
			task_insert_ready_list(rzyOS_mutex -> owner);
		}
		else
		{
			currentTask -> prio = rzyOS_mutex -> owner_original_prio;
		}
	}

	if (rzyOS_event_wait_count(&(rzyOS_mutex -> rzyOS_ecb)) > 0)
	{
		task_tcb_s *task_tcb = rzyOS_event_wakeup(&(rzyOS_mutex -> rzyOS_ecb), (void *)0, error_no_error);
		rzyOS_mutex -> lock_count ++;
		rzyOS_mutex -> owner = task_tcb;
		rzyOS_mutex -> owner_original_prio = task_tcb -> prio;

		if (task_tcb -> prio < currentTask -> prio)
		{
			task_schedule();
		}
	}

	task_exit_critical(status);

	return error_no_error;
}
