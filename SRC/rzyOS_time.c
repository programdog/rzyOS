#include "rzyOS.h"

//task中的延时函数,使用延时队列进行处理
//param: delay--systick周期计数
void task_delay(uint32_t delay)
{
	uint32_t status = task_enter_critical();

	delay_list_insert_time_node(currentTask, delay);

	task_remove_ready_list(currentTask);

	task_exit_critical(status);

	task_schedule();
}
