#include "rzyOS_semaphore.h"

//信号量初始化函数
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

//信号量阻塞等待函数
//wait_ticks ： 超时等待时间
uint32_t rzyOS_sem_wait(rzyOS_sem_s *sem, uint32_t wait_ticks)
{
	uint32_t status = task_enter_critical();

	//若信号量>0， 说明有资源， 不需要等待， 则信号量减1, return no error， 继续回任务执行
	if (sem -> count > 0)
	{
		sem -> count --;
		task_exit_critical(status);
		return error_no_error;
	}
	else
	{
		//信号量=0, 则把当前任务加入到信号量的等待队列， 并标注事件为sem
		rzyOS_event_wait(&(sem -> rzyOS_ecb), currentTask, (void *)0, event_type_semaphore, wait_ticks);
		task_exit_critical(status);

		//切换其他任务
		task_schedule();

		return currentTask -> wait_event_result;
	}
}

//信号量非阻塞等待函数
uint32_t rzyOS_sem_no_wait(rzyOS_sem_s *sem)
{
	uint32_t status = task_enter_critical();

	if (sem -> count > 0)
	{
		sem -> count --;
		task_exit_critical(status);

		return error_no_error;
	}
	else
	{
		//非阻塞， 直接返回资源不可用的flag， 让应用自己去选择处理
		task_exit_critical(status);
		
		return error_resource_unvaliable;
	}
}

//信号量释放函数
void rzyOS_sem_post(rzyOS_sem_s *sem)
{
	uint32_t status = task_enter_critical();

	//如果信号量内部有等待事件， 即大于0
	if (rzyOS_event_wait_count(&(sem -> rzyOS_ecb)) > 0)
	{
		//则对信号量中事件控制块管理的任务列表的第一个任务做唤醒
		task_tcb_s *task_tcb = rzyOS_event_wakeup(&(sem -> rzyOS_ecb), (void *)0, error_no_error);
		
		//若被唤醒任务的优先级高于当前任务， 则立即切换
		if (task_tcb -> prio < currentTask -> prio)
		{
			task_schedule();
		}
	}
	else
	{
		sem -> count ++;
		//若max_count有限制且当前sem的count已经大于max_count的值, 则直接赋值最大值max_count
		if ((sem -> max_count != 0) && (sem -> count > sem -> max_count))
		{
			sem ->count = sem -> max_count;
		}
	}

	task_exit_critical(status);
}

//信号量信息获得函数
void rzyOS_sem_get_info(rzyOS_sem_s *sem, rzyOS_sen_info *sem_info)
{
	uint32_t status = task_enter_critical();

	sem_info -> sem_count = sem -> count;
	sem_info -> max_count = sem -> max_count;
	sem_info -> task_count = rzyOS_event_wait_count(&(sem -> rzyOS_ecb));

	task_exit_critical(status);
}

//信号量销毁
//在rzyOS_event_remove_all()函数中会把任务插入到就绪队列
uint32_t rzyOS_sem_destroy(rzyOS_sem_s *sem)
{
	uint32_t status = task_enter_critical();

	//删除等待该信号量的任务， 获取移除的个数
	uint32_t count = rzyOS_event_remove_all(&(sem -> rzyOS_ecb), (void *)0, error_delete);
	//信号量计数值清零
	sem -> count = 0;

	task_exit_critical(status);

	if (count > 0)
	{
		//若获取移除的个数大于0, 则进行一次调度， 看看是否有更高优先级的任务处于刚才的等待状态
		task_schedule();
	}

	return count;
}
