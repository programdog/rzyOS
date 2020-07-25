#include <stdio.h>
// #include "stdio.h"

#include "rzyOS_api.h"

#include "led.h"
#include "uart.h"



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
	float test = 0.01;

	void *ptr = (void *)malloc(924);

	for (;;)
	{
		usage = rzyOS_get_cpu_usage();

		test += 0.01f;

		if (test > 100)
		{
			test = 0;
		}
		
		printf("task1 float test = %0.2f\n", test);
		printf("cpu usage : %0.2f%%\n", usage);

		GPIO_SetBits(GPIOD, GPIO_Pin_12);
		task_delay(10);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		task_delay(20);
	}
}

int task2Flag;
void task2_entry(void *param)
{
	for (;;)
	{
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		task_delay(5);
		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		task_delay(50);
		printf("i am task2\n");
	}
}

int task3Flag;
void task3_entry(void *param)
{
	for (;;)
	{
		GPIO_SetBits(GPIOD, GPIO_Pin_14);
		task_delay(30);
		GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		task_delay(100);
		printf("i am task3\n");
	}
}

int task4Flag;
void task4_entry(void *param)
{
	for (;;)
	{
		GPIO_SetBits(GPIOD, GPIO_Pin_15);
		task_delay(20);
		GPIO_ResetBits(GPIOD, GPIO_Pin_15);
		task_delay(200);
		printf("i am task4\n");
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
	LED_Init();

	//串口打印波特率115200
	USART3_Init(115200);

	//设定systick中断时间周期
	rzyOS_systick_init(168);

	rzyOS_kernel_init();


	//app任务初始化
	rzyOS_app_init();


	rzyOS_start();

	return 0;
}
