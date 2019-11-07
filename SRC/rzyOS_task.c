#include "rzyOS.h"

void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack)
{
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
	
	task -> stack = stack;
	node_init(&(task -> link_node));
	task -> delayTicks = 0;
	task -> prio = prio;
	task -> ready_status = RZYOS_TASK_STATUS_READY;
	task -> slice = RZYOS_SLICE_MAX;
	node_init(&(task -> delay_node));
	
	task_insert_ready_list(task);
}

void rzyOS_task_suspend(task_tcb_s *task)
{
	uint32_t status = task_enter_critical();
	
	//如果任务处于延时队列, 说明不在就绪队列, 则不进行挂起操作
	if (!(task -> ready_status & RZYOS_TASK_STATUS_DELAY))
	{
		//每次被调用挂起操作,都要对suspend_count加1
		task -> suspend_count ++;
		//如果suspend_count是1, 说明周期内第一次挂起, 则要进行切换操作 , 大于等于2ze不再需要切换操作
		if (1 == task -> suspend_count)
		{
			//设置挂起标志
			task -> ready_status |= RZYOS_TASK_STATUS_SUSPEND;
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

void rzyOS_task_wakeup(task_tcb_s *task)
{
	uint32_t status = task_enter_critical();
	
	if (task -> ready_status & RZYOS_TASK_STATUS_SUSPEND)
	{
		//每次调用解挂函数,都要减1
		task -> suspend_count --;
		//当suspend_count为0时说明需要插入就绪列表了
		if (0 == task -> suspend_count)
		{
			task -> ready_status &= ~RZYOS_TASK_STATUS_SUSPEND;
			task_insert_ready_list(task);
			//有可能被唤醒的任务优先级最高, 进入调度裁决是否需要切换
			task_schedule();
		}
	}
	
	task_exit_critical(status);
}
