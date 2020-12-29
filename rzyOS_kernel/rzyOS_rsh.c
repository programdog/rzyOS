#include <ctype.h>
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


//isdigit() : 检查参数是否为阿拉伯数字0到9, 参数字符
static int32_t string_to_dec(char *buf, uint32_t len)
{
	uint32_t i = 0;
	uint32_t base = 10;
	int32_t neg = 1;
	int32_t result = 0;

	if (('0' == buf[0]) && ('x' == buf[1]))
	{
		base = 16;
		neg = 1;
		i = 2;
	}
	else if ('-' == buf[0])
	{
		base = 10;
		neg = -1;
		i = 1;
	}
	for (; i < len; i ++)
	{
		if ((0x20 == buf[i]) || (0x0d == buf[i]))
		{
			break ;
		}

		result *= base;
		if (isdigit(buf[i]))
		{
			result += buf[i] - '0';
		}
		else if (isxdigit(buf[i]))
		{
			result += tolower(buf[i]) - 87;
		}
		else
		{
			result += buf[i] - '0';
		}
	}

	result *= neg;

	return result;
}

void rzyOS_rsh_cmd_analyse()
{

}
