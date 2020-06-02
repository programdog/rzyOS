#include "rzyOS_schedule.h"


uint32_t saveAndLoadStackAddr(uint32_t stackAddr)
{
	//第一次切换时， 当前任务tcb为0， 所以不会保存
	if (currentTask != (task_tcb_s *)0)
	{
		//保存堆栈地址
		currentTask -> stack = (uint32_t *)stackAddr;
	}

	currentTask = nextTask;

	//取下一任务堆栈地址
	return (uint32_t)currentTask -> stack;
}
