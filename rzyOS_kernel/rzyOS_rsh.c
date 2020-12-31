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
//isxdigit() : 检查参数是否为16进制数字， 16进制数字:0123456789abcdefABCDEF
//tolower() : 字母字符转换成小写，非字母字符不做出处理

//字符串转数字
static int32_t string_to_dec(char *buf, uint32_t len)
{
	uint32_t index = 0; //buffer index
	uint32_t base = 10; //进制
	int32_t neg = 1; //positive or negative
	int32_t result = 0; //转换数字

	//16 base
	if (('0' == buf[0]) && ('x' == buf[1]))
	{
		base = 16;
		neg = 1;
		index = 2;
	}
	//negative & 10base
	else if ('-' == buf[0])
	{
		base = 10;
		neg = -1;
		index = 1;
	}

	// pasre buffer
	for (; index < len; index ++)
	{
		// 0x20 -> space
		// 0x0d -> CR -> return
		if ((0x20 == buf[index]) || (0x0d == buf[index])) // space or return
		{
			break ;
		}

		result *= base;
		//如果字符为数字
		if (isdigit((int)buf[index]))
		{
			//转换为int 数字
			result += buf[index] - '0';
		}
		//如果为字母
		else if (isxdigit((int)buf[index]))
		{
			//变换小写字母后 -87 转化为int数字
			result += tolower((int)buf[index]) - 87;
		}
		else
		{
			result += buf[index] - '0';
		}
	}

	//加上正负
	result *= neg;

	return result;
}

void rzyOS_rsh_cmd_analyse()
{

}
