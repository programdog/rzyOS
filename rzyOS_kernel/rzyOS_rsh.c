#include "rzyOS_rsh.h"

static char rzh_show_1[300] = "\r\n\r\n                 ____   _____ \r\n                / __ \\ / ____|\r\n _ __ _____   _| |  | | (___  \r\n| '__|_  / | | | |  | |\\___ \\ \r\n| |   / /| |_| | |__| |____) | \r\n|_|  /___|\\__, |\\____/|_____/ \r\n           __/ |              \r\n          |___/               \r\n\r\n";

// static char copyright_1[] = "";

void rzyOS_rzh_show(void)
{
	printf("%s", rzh_show_1);
}

//rsh任务控制块
static task_tcb_s rzyOS_rsh_task_tcb;
//rsh任务堆栈
static tTaskStack rzyOS_rsh_task_stack[RZYOS_RSH_STACK_SIZE];

uint8_t ch = 0;
//rzyOS终端任务
void rzyOS_rsh_task(void *param)
{
	while(1)
	{
		uint16_t used = uart3_rx_buffer_used();
		
		uart3_rx_buffer_read(&ch);


	}
}

//rzyOS终端任务初始化
void rzyOS_rsh_task_init(void)
{
	task_init(&rzyOS_rsh_task_tcb, rzyOS_rsh_task, (void *)0, RZYOS_RSH_PRIO, rzyOS_rsh_task_stack, sizeof(rzyOS_rsh_task_stack));
}
