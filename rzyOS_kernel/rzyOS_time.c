#include "rzyOS_schedule.h"

//task中的延时函数,使用延时队列进行处理
//param: delay--systick周期计数
void task_delay(uint32_t delay)
{
	uint32_t status = task_enter_critical();

	//将当前任务插入延时等待列表，并传递延时时长
	delay_list_insert_time_node(currentTask, delay);

	//上一状态处于就绪列表，当前已插入延时等待列表，所以从就绪列表中移除
	task_remove_ready_list(currentTask);

	task_exit_critical(status);

	task_schedule();
}
