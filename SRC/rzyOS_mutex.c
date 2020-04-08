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

//互斥锁等待函数(阻塞)
uint32_t rzyOS_mutex_wait(rzyOS_mutex_s *rzyOS_mutex, uint32_t wait_time)
{
	uint32_t status = task_enter_critical();

	//互斥锁是否被锁定
	//如果未被锁定， 当前任务可以使用互斥锁
	if (rzyOS_mutex -> lock_count <= 0)
	{
		//当前任务使用互斥锁
		rzyOS_mutex -> lock_count ++;
		rzyOS_mutex -> owner = currentTask;
		rzyOS_mutex -> owner_original_prio = currentTask -> prio;

		task_exit_critical(status);

		//获取互斥锁，直接返回
		return error_no_error;
	}
	//如果被锁定
	else
	{
		//如果是被自己锁定
		if (currentTask == rzyOS_mutex -> owner)
		{
			//锁定计数加一， 直接退出
			rzyOS_mutex -> lock_count ++;

			task_exit_critical(status);

			return error_no_error;
		}
		//如果不是被自己锁定，有可能要做优先级继承操作
		else
		{
			//比较当前任务的优先级与互斥锁的优先级
			//如果当前任务优先级更高， 则需要做优先级继承的操作
			if (currentTask -> prio < rzyOS_mutex -> owner -> prio)
			{
				//如果占有互斥锁的任务为就绪状态
				if (RZYOS_TASK_STATUS_READY == rzyOS_mutex -> owner -> task_status)
				{
					//重新更新优先级
					task_remove_ready_list(rzyOS_mutex -> owner);
					//优先级继承
					rzyOS_mutex -> owner -> prio = currentTask -> prio;
					task_insert_ready_list(rzyOS_mutex -> owner);
				}
				else
				{
					//优先级继承
					rzyOS_mutex -> owner -> prio = currentTask -> prio;
				}
			}

			//把当前的任务插入到事件控制块，等待互斥锁
			rzyOS_event_wait(&(rzyOS_mutex -> rzyOS_ecb), currentTask, (void *)0, event_type_mutex, wait_time);
			task_exit_critical(status);

			task_schedule();

			return currentTask -> wait_event_result;
		}
	}
}

//互斥锁等待函数(非阻塞)
uint32_t rzyOS_mutex_no_wait(rzyOS_mutex_s *rzyOS_mutex)
{
	uint32_t status = task_enter_critical();

	//互斥锁是否被锁定
	//如果未被锁定， 当前任务可以使用互斥锁
	if (rzyOS_mutex -> lock_count <= 0)
	{
		rzyOS_mutex -> lock_count ++;
		rzyOS_mutex -> owner = currentTask;
		rzyOS_mutex -> owner_original_prio = currentTask -> prio;

		task_exit_critical(status);

		return error_no_error;
	}
	//如果被锁定
	else
	{
		//如果是被自己锁定
		if (currentTask == rzyOS_mutex -> owner)
		{
			//锁定计数加一
			rzyOS_mutex -> lock_count ++;

			task_exit_critical(status);

			return error_no_error;
		}

		task_exit_critical(status);

		//如果不是被自己锁定， 则无资源
		return error_resource_unvaliable;
	}
}

//互斥锁释放函数
uint32_t rzyOS_mutex_post(rzyOS_mutex_s *rzyOS_mutex)
{
	uint32_t status = task_enter_critical();

	//互斥锁是否被锁定
	//如果未被锁定， 直接返回
	//否则说明互斥锁被使用
	if (rzyOS_mutex -> lock_count <= 0)
	{
		task_exit_critical(status);

		return error_no_error;
	}

	//互斥锁被使用， 判断互斥锁所有者是否是当前任务占用
	//如果不是， 则发生错误， 因为只能拥有者才能释放
	if (currentTask != rzyOS_mutex -> owner)
	{
		task_exit_critical(status);

		return error_not_owner;
	}

	//互斥锁被使用， 且互斥锁所有者是当前任务
	//锁定计数减1
	rzyOS_mutex -> lock_count --;

	//如果锁定计数还大于0, 则说明处于嵌套状态，还未到释放互斥锁阶段， 直接退出
	if (rzyOS_mutex -> lock_count > 0)
	{
		task_exit_critical(status);

		return error_no_error;
	}
	
	//如果锁定计数还等于0， 则说明要释放互斥锁
	//判断是否发生优先级继承
	if (rzyOS_mutex -> owner_original_prio != rzyOS_mutex -> owner -> prio)
	{
		//如果任务在就绪状态
		if (RZYOS_TASK_STATUS_READY == rzyOS_mutex -> owner -> task_status)
		{
			//则修改在就绪列表的位置
			task_remove_ready_list(rzyOS_mutex -> owner);
			//优先级恢复
			currentTask -> prio = rzyOS_mutex -> owner_original_prio;
			task_insert_ready_list(rzyOS_mutex -> owner);
		}
		//如果任务不在就绪状态， 直接恢复优先级
		else
		{
			currentTask -> prio = rzyOS_mutex -> owner_original_prio;
		}
	}

	//判断当前事件控制块中是否有任务等待互斥锁
	if (rzyOS_event_wait_count(&(rzyOS_mutex -> rzyOS_ecb)) > 0)
	{
		//任务唤醒
		task_tcb_s *task_tcb = rzyOS_event_wakeup(&(rzyOS_mutex -> rzyOS_ecb), (void *)0, error_no_error);
		//标记互斥锁
		rzyOS_mutex -> lock_count ++;
		rzyOS_mutex -> owner = task_tcb;
		rzyOS_mutex -> owner_original_prio = task_tcb -> prio;

		//即将激活的任务优先级是否更高
		if (task_tcb -> prio < currentTask -> prio)
		{
			task_schedule();
		}
	}

	task_exit_critical(status);

	return error_no_error;
}
