#include "rzyOS.h"

//task初始化
void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack)
{
	//初始化任务堆栈，对应到通用寄存器
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)entry;
	*(--stack) = (unsigned long)0x14;
	*(--stack) = (unsigned long)0x12;
	*(--stack) = (unsigned long)0x03;
	*(--stack) = (unsigned long)0x02;
	*(--stack) = (unsigned long)0x01;
	*(--stack) = (unsigned long)param;
	
	*(--stack) = (unsigned long)0x11;
	*(--stack) = (unsigned long)0x10;
	*(--stack) = (unsigned long)0x09;
	*(--stack) = (unsigned long)0x08;
	*(--stack) = (unsigned long)0x07;
	*(--stack) = (unsigned long)0x06;
	*(--stack) = (unsigned long)0x05;
	*(--stack) = (unsigned long)0x04;
	
	//任务堆栈指针传递
	task -> stack = stack;
	//初始化任务tcb
	node_init(&(task -> link_node));
	task -> delayTicks = 0;
	task -> prio = prio;
	task -> task_status = RZYOS_TASK_STATUS_READY;
	task -> slice = RZYOS_SLICE_MAX;
	node_init(&(task -> delay_node));
	task -> suspend_count = 0;
	task -> clean = (void (*) (void *))0;
	task -> clean_param = (void *)0;
	task -> request_delete_flag = 0;
	
	//初始化结束后将任务插入延时队列等待调度
	task_insert_ready_list(task);
}

//任务的挂起
void rzyOS_task_suspend(task_tcb_s *task)
{
	uint32_t status = task_enter_critical();
	
	//如果任务处于延时队列, 说明不在就绪队列, 则不进行挂起操作（目前不对处于延时队列的任务进行挂起）
	if (!(task -> task_status & RZYOS_TASK_STATUS_DELAY))
	{
		//每次被调用挂起操作,都要对suspend_count加1
		task -> suspend_count ++;
		//如果suspend_count是1, 说明周期内第一次挂起, 则要进行切换操作 , 大于等于2则不再需要切换操作
		if (1 == task -> suspend_count)
		{
			//设置挂起标志
			task -> task_status |= RZYOS_TASK_STATUS_SUSPEND;
			//从就绪列表中移出
			task_remove_ready_list(task);
			//如果当前任务是自己的话, 则需要立即调度切换
			if (task == currentTask)
			{
				task_schedule();
			}
		}
	}
	
	task_exit_critical(status);
}

//任务的唤醒
void rzyOS_task_wakeup(task_tcb_s *task)
{
	uint32_t status = task_enter_critical();
	
	//如果任务处于挂起状态
	if (task -> task_status & RZYOS_TASK_STATUS_SUSPEND)
	{
		//每次调用解挂函数,都要减1
		task -> suspend_count --;
		//当suspend_count为0时说明需要插入就绪列表了
		if (0 == task -> suspend_count)
		{
			task -> task_status &= ~RZYOS_TASK_STATUS_SUSPEND;
			task_insert_ready_list(task);
			//被唤醒的任务优先级有可能最高, 则进入调度裁决是否需要切换
			task_schedule();
		}
	}
	
	task_exit_critical(status);
}

//任务清除回调函数
void rzyOS_task_clean_callback(task_tcb_s *task, void (*clean)(void *param), void *param)
{
	//任务清除callback函数
	task -> clean = clean;
	//任务清除callback函数的参数
	task -> clean_param = param;
}

//任务强制删除函数
void rzyOS_task_force_delete(task_tcb_s *task)
{
	uint32_t status = task_enter_critical();
	
	if (task -> task_status & RZYOS_TASK_STATUS_DELAY)
	{
		rzyOS_task_delay_list_remove(task);
	}
	else if (!(task -> task_status & RZYOS_TASK_STATUS_SUSPEND))
	{
		rzyOS_task_ready_list_remove(task);
	}
	
	//若该task有清理函数则调用
	if (task -> clean)
	{
		task -> clean(task -> clean_param);
	}
	
	//若删除的是自己，则进行调度
	if (currentTask == task)
	{
		task_schedule();
	}
	
	task_exit_critical(status);
}

//任务请求删除函数
//parameter
//task_tcb_s *task ： 希望删除的任务
void rzyOS_task_request_delete(task_tcb_s *task)
{
	uint32_t status = task_enter_critical();
	
	//标注请求删除状态
	task -> request_delete_flag = 1;
	
	task_exit_critical(status);
}

//对于任务请求删除的检测函数
//return
//返回请求删除状态
uint8_t rzyOS_task_request_delete_check(void)
{
	uint8_t delete_status;
	
	uint32_t status = task_enter_critical();
	
	delete_status = currentTask -> request_delete_flag;
	
	task_exit_critical(status);
	
	return delete_status;
}

//任务自我删除函数
void rzyOS_task_delete_self(void)
{
	uint32_t status = task_enter_critical();
	
	//运行任务本身， 即currentTask == task， 当前任务仍然处于就绪列表
	rzyOS_task_ready_list_remove(currentTask);
	
	if (currentTask -> clean)
	{
		currentTask -> clean(currentTask -> clean_param);
	}
	
	task_schedule();
	
	task_exit_critical(status);
}

//获取任务信息
void rzyOS_task_get_info(task_tcb_s *task, rzyOS_task_info_s *info)
{
	uint32_t status = task_enter_critical();
	
	info -> delay_ticks = task -> delayTicks;
	info -> prio = task -> prio;
	info -> slice = task -> slice;
	info -> suspend_count = task -> suspend_count;
	info -> task_status = task -> task_status;
	
	task_exit_critical(status);
}
