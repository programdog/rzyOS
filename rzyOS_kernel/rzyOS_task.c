#include "rzyOS_schedule.h"

void rzyOS_memset(void *mem, int val, unsigned long size)
{
	char *p = (char *)mem;
	char v = (char)val;
	for (unsigned long i = 0; i < size; i++)
	{
		p[i] = v;
	}
}

#ifdef ARMCM3_SIM
//task初始化
void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack_bottom, uint32_t task_size)
{
	//定义栈顶(高地址)
	uint32_t *stack_top;
	//栈底赋值(低地址)
	task -> task_bottom_base = stack_bottom;
	//栈大小
	task -> task_stack_size = task_size;
	//栈清零
	memset(stack_bottom, 0, task_size);

	//计算栈顶地址
	stack_top = stack_bottom + task_size / sizeof(tTaskStack);

	//初始化任务堆栈，对应到通用寄存器
	*(--stack_top) = (unsigned long)(1 << 24);
	*(--stack_top) = (unsigned long)entry;
	*(--stack_top) = (unsigned long)0x14;
	*(--stack_top) = (unsigned long)0x12;
	*(--stack_top) = (unsigned long)0x03;
	*(--stack_top) = (unsigned long)0x02;
	*(--stack_top) = (unsigned long)0x01;
	*(--stack_top) = (unsigned long)param;
	
	*(--stack_top) = (unsigned long)0x11;
	*(--stack_top) = (unsigned long)0x10;
	*(--stack_top) = (unsigned long)0x09;
	*(--stack_top) = (unsigned long)0x08;
	*(--stack_top) = (unsigned long)0x07;
	*(--stack_top) = (unsigned long)0x06;
	*(--stack_top) = (unsigned long)0x05;
	*(--stack_top) = (unsigned long)0x04;
	
	//任务堆栈动态指针传递
	task -> stack = stack_top;
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
	
	//初始化结束后将任务插入就绪队列等待调度
	task_insert_ready_list(task);
}
#endif


#if defined(STM32F40_41xxx) || defined(STM32F40XX)
//task初始化
void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack_bottom, uint32_t task_size)
{
	//定义栈顶(高地址)
	uint32_t *stack_top;
	//栈底赋值(低地址)
	task -> task_bottom_base = stack_bottom;
	//栈大小
	task -> task_stack_size = task_size;
	//栈清零
	rzyOS_memset(stack_bottom, 0, task_size);

	//计算栈顶地址
	stack_top = stack_bottom + task_size / sizeof(tTaskStack);

	*(--stack_top) = (unsigned long)(0);					// 空,未用
	*(--stack_top) = (unsigned long)(0);					// FPSCR
	*(--stack_top) = (unsigned long)(0x15);					// S15
	*(--stack_top) = (unsigned long)(0x14);					// S14
	*(--stack_top) = (unsigned long)(0x13);					// S13
	*(--stack_top) = (unsigned long)(0x12);					// S12
	*(--stack_top) = (unsigned long)(0x11);					// S11
	*(--stack_top) = (unsigned long)(0x10);					// S10
	*(--stack_top) = (unsigned long)(0x9);					// S9
	*(--stack_top) = (unsigned long)(0x8);					// S8
	*(--stack_top) = (unsigned long)(0x7);					// S7
	*(--stack_top) = (unsigned long)(0x6);					// S6
	*(--stack_top) = (unsigned long)(0x5);					// S5
	*(--stack_top) = (unsigned long)(0x4);					// S4
	*(--stack_top) = (unsigned long)(0x3);					// S3
	*(--stack_top) = (unsigned long)(0x2);					// S2
	*(--stack_top) = (unsigned long)(0x1);					// S1
	*(--stack_top) = (unsigned long)(0x0);					// S0

	*(--stack_top) = (unsigned long)(1 << 24);				// XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
	*(--stack_top) = (unsigned long)entry;					// 程序的入口地址
	*(--stack_top) = (unsigned long)0x14;					// R14(LR), 任务不会通过return xxx结束自己，所以未用
	*(--stack_top) = (unsigned long)0x12;					// R12, 未用
	*(--stack_top) = (unsigned long)0x3;					// R3, 未用
	*(--stack_top) = (unsigned long)0x2;					// R2, 未用
	*(--stack_top) = (unsigned long)0x1;					// R1, 未用
	*(--stack_top) = (unsigned long)param;					// R0 = param, 传给任务的入口函数
	*(--stack_top) = (unsigned long)0x11;					// R11, 未用
	*(--stack_top) = (unsigned long)0x10;					// R10, 未用
	*(--stack_top) = (unsigned long)0x9;					// R9, 未用
	*(--stack_top) = (unsigned long)0x8;					// R8, 未用
	*(--stack_top) = (unsigned long)0x7;					// R7, 未用
	*(--stack_top) = (unsigned long)0x6;					// R6, 未用
	*(--stack_top) = (unsigned long)0x5;					// R5, 未用
	*(--stack_top) = (unsigned long)0x4;					// R4, 未用

	*(--stack_top) = (unsigned long)(0x31);					// S31
	*(--stack_top) = (unsigned long)(0x30);					// S30
	*(--stack_top) = (unsigned long)(0x29);					// S29
	*(--stack_top) = (unsigned long)(0x28);					// S28
	*(--stack_top) = (unsigned long)(0x27);					// S27
	*(--stack_top) = (unsigned long)(0x26);					// S26
	*(--stack_top) = (unsigned long)(0x25);					// S25
	*(--stack_top) = (unsigned long)(0x24);					// S24
	*(--stack_top) = (unsigned long)(0x23);					// S23
	*(--stack_top) = (unsigned long)(0x22);					// S22
	*(--stack_top) = (unsigned long)(0x21);					// S21
	*(--stack_top) = (unsigned long)(0x20);					// S20
	*(--stack_top) = (unsigned long)(0x19);					// S19
	*(--stack_top) = (unsigned long)(0x18);					// S18
	*(--stack_top) = (unsigned long)(0x17);					// S17
	*(--stack_top) = (unsigned long)(0x16);					// S16

	
	//任务堆栈动态指针传递
	task -> stack = stack_top;
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
#endif


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
	//定义栈尾指针
	uint32_t *task_stack_end;

	uint32_t status = task_enter_critical();
	
	//任务基础信息
	info -> delay_ticks = task -> delayTicks;
	info -> prio = task -> prio;
	info -> slice = task -> slice;
	info -> suspend_count = task -> suspend_count;
	info -> task_status = task -> task_status;

	//任务堆栈大小
	info -> task_stack_size = task -> task_stack_size;
	//空闲堆栈大小清零，进行统计
	info -> task_stack_free_size = 0;
	//获取栈尾地址
	task_stack_end = task -> task_bottom_base;

	//计算栈顶地址
	//按照4字节偏移
	uint32_t *stack_top = task -> task_bottom_base + (task -> task_stack_size) / sizeof(tTaskStack);

	//判断为0的uint32_t个数
	while ((0 == *task_stack_end ++) && (task_stack_end <= stack_top))
	{
		info -> task_stack_free_size ++;
	}
	//按照4字节计算空闲的栈空间
	info -> task_stack_free_size *= sizeof(tTaskStack);
	
	task_exit_critical(status);
}
