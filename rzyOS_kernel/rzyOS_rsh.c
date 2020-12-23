#include "rzyOS_rsh.h"

static char rzh_show_1[300] = "\r\n\r\n                 ____   _____ \r\n                / __ \\ / ____|\r\n _ __ _____   _| |  | | (___  \r\n| '__|_  / | | | |  | |\\___ \\ \r\n| |   / /| |_| | |__| |____) | \r\n|_|  /___|\\__, |\\____/|_____/ \r\n           __/ |              \r\n          |___/               \r\n\r\n";

// static char copyright_1[] = "";

void rzyOS_rsh_show(void)
{
	printf("%s", rzh_show_1);
}

//rsh任务控制块
static task_tcb_s rzyOS_rsh_task_tcb;
//rsh任务堆栈
static tTaskStack rzyOS_rsh_task_stack[RZYOS_RSH_STACK_SIZE];


//rzyOS终端任务
void rzyOS_rsh_task(void *param)
{
	uint8_t ch = 0;
	
	while(1)
	{
		uint16_t used = uart3_rx_buffer_used();
		
		// uart3_rx_buffer_read(&ch);
		if (used > 0)
		{
			for (int i = 0; i < used; i ++)
			{
				uart3_rx_buffer_read(&ch);
				printf("%c", ch);
			}
		}

		task_delay(100);
	}
}

//rzyOS终端任务初始化
void rzyOS_rsh_task_init(void)
{
	task_init(&rzyOS_rsh_task_tcb, rzyOS_rsh_task, (void *)0, RZYOS_RSH_PRIO, rzyOS_rsh_task_stack, sizeof(rzyOS_rsh_task_stack));
}

void rzyOS_rsh_cmd_analyse()
{

}

void printf_hello(int argc, void *argv)
{
	printf("HelloWorld!\r\n");
}
