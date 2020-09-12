#include <stdarg.h>
#include "rzyOS_printf.h"
#include "rzyOS_mm1.h"
#include "rzyOS_semaphore.h"
#include "stm32f4xx.h"

static char *buffer = NULL;

//printf & r_printf 保护
extern rzyOS_sem_s printf_protect_sem;

void write_uart3(char *buffer, int count);

//rzyOS printf 函数
int r_printf(char *fmt, ...)
{
	//初始化申请空间
	if (NULL == buffer)
	{
		buffer = malloc(R_PRINTF_BUFFER_SIZE);
	}

	if (NULL == buffer)
	{
		return -1;
	}

	rzyOS_sem_wait(&printf_protect_sem, 0);

	va_list ap;
	va_start(ap, fmt);
	int ret = vsnprintf(buffer, R_PRINTF_BUFFER_SIZE, fmt, ap);
	va_end(ap);

	//uart3输出到uart3
	write_uart3(buffer, ret);

	rzyOS_sem_post(&printf_protect_sem);

	return ret;
}

void write_uart3(char *buffer, int count)
{
	for (int i = 0; i < count; i ++)
	{
		//循环发送,直到发送完毕
		while((USART3 -> SR & 0X40) == 0);
		USART3 -> DR = (uint8_t)buffer[i];
	}
}
