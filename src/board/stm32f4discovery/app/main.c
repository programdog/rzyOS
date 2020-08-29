#include <stdio.h>
// #include "stdio.h"

#include "rzyOS_api.h"

#include "led.h"
#include "uart.h"



task_tcb_s tcb_task1;
task_tcb_s tcb_task2;
task_tcb_s tcb_task3;
task_tcb_s tcb_task4;
task_tcb_s tcb_task5;


//4 * 1024 = 4k
tTaskStack task1Env[1024] __attribute__ ((aligned (8)));
tTaskStack task2Env[1024] __attribute__ ((aligned (8)));
tTaskStack task3Env[1024] __attribute__ ((aligned (8)));
tTaskStack task4Env[1024] __attribute__ ((aligned (8)));
tTaskStack task5Env[1024] __attribute__ ((aligned (8)));


float usage = 0;

uint8_t ch = 0;


void task1_entry(void *param)
{
	float test = 0.01;

	void *ptr = (void *)malloc(824);

	for (;;)
	{
		usage = rzyOS_get_cpu_usage();

		test += 0.01f;

		if (test > 100)
		{
			test = 0;
		}
		
		r_printf("task1 float test = %0.2f\r\n", test);
		r_printf("cpu usage : %0.2f%%\r\n", usage);

		GPIO_SetBits(GPIOD, GPIO_Pin_12);
		task_delay(10);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		task_delay(20);
	}
}

void task2_entry(void *param)
{
	for (;;)
	{
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		task_delay(5);
		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		task_delay(50);
		r_printf("i am task2\r\n");
	}
}

void task3_entry(void *param)
{
	for (;;)
	{
		GPIO_SetBits(GPIOD, GPIO_Pin_14);
		task_delay(30);
		GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		task_delay(100);
		// r_printf("i am task3\r\n");
	}
}

void task4_entry(void *param)
{
	for (;;)
	{
		GPIO_SetBits(GPIOD, GPIO_Pin_15);
		task_delay(20);
		GPIO_ResetBits(GPIOD, GPIO_Pin_15);
		task_delay(200);
		r_printf("i am task4\r\n");
	}
}

void task5_entry(void *param)
{
	for (;;)
	{
		uint16_t used = uart3_rx_buffer_used();

		if (used > 0)
		{
			for (int i = 0; i < used; i ++)
			{
				uart3_rx_buffer_read(&ch);
				printf("%c", ch);
			}
		}

		task_delay(200);
	}
}

//任务初始化
void rzyOS_app_init(void)
{
	task_init(&tcb_task1, task1_entry, (void *)0x11111111, 0, task1Env, sizeof(task1Env));
	task_init(&tcb_task2, task2_entry, (void *)0x22222222, 1, task2Env, sizeof(task2Env));
	task_init(&tcb_task3, task3_entry, (void *)0x33333333, 0, task3Env, sizeof(task3Env));
	task_init(&tcb_task4, task4_entry, (void *)0x44444444, 1, task4Env, sizeof(task4Env));
	task_init(&tcb_task5, task5_entry, (void *)0x55555555, 30, task5Env, sizeof(task5Env));
}

int main()
{
	//设置系统中断优先级分组4
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	LED_Init();

	//串口打印波特率115200
	USART3_Init(115200);

	//设定systick中断时间周期
	rzyOS_systick_init(168);

	rzyOS_kernel_init();

	vfs_init();
	std_init();


	//app任务初始化
	rzyOS_app_init();


	rzyOS_start();

	return 0;
}
