#include "rzyOS_api.h"

task_tcb_s tcb_task1;
task_tcb_s tcb_task2;
task_tcb_s tcb_task3;
task_tcb_s tcb_task4;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
tTaskStack task3Env[1024];
tTaskStack task4Env[1024];


float usage = 0;

int task1Flag;
void task1_entry(void *param)
{

	for (;;)
	{
//		usage = rzyOS_get_cpu_usage();
		task1Flag = 0;
		task_delay(10);
		task1Flag = 1;
		task_delay(10);
	}
}

int task2Flag;
void task2_entry(void *param)
{
	
	for (;;)
	{
		task2Flag = 0;
		task_delay(10);
		task2Flag = 1;
		task_delay(10);

	}
}

int task3Flag;
void task3_entry(void *param)
{
	for (;;)
	{
		task3Flag = 0;
		task_delay(10);
		task3Flag = 1;
		task_delay(10);
	}
}

int task4Flag;
void task4_entry(void *param)
{

	for (;;)
	{
		task4Flag = 0;
		task_delay(10);
		task4Flag = 1;
		task_delay(10);
	}
}

//任务初始化
void rzyOS_app_init(void)
{
	task_init(&tcb_task1, task1_entry, (void *)0x11111111, 0, task1Env, sizeof(task1Env));
	task_init(&tcb_task2, task2_entry, (void *)0x22222222, 1, task2Env, sizeof(task2Env));
	task_init(&tcb_task3, task3_entry, (void *)0x33333333, 0, task3Env, sizeof(task3Env));
	task_init(&tcb_task4, task4_entry, (void *)0x44444444, 1, task4Env, sizeof(task4Env));
}

int main()
{
	//设定systick中断时间周期
	set_systick_period(RZYOS_TICK_MS);

	rzyOS_kernel_init();


	//app任务初始化
	rzyOS_app_init();
	rzyOS_start();


	return 0;
}
