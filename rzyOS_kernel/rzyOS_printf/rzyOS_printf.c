#include <stdarg.h>
#include "rzyOS_printf.h"
#include "rzyOS_mm1.h"
#include "rzyOS_semaphore.h"
#include "stm32f4xx.h"

static char *buffer = NULL;
// static rzyOS_sem_s r_printf_protect_sem;
extern rzyOS_sem_s printf_protect_sem;

void write_uart3(char *buffer, int count);

int r_printf(char *fmt, ...)
{
	if (NULL == buffer)
	{
		buffer = malloc(R_PRINTF_BUFFER_SIZE);
		// rzyOS_sem_init(&r_printf_protect_sem, 1, 1);
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

	write_uart3(buffer, ret);

	rzyOS_sem_post(&printf_protect_sem);

	return ret;
}

void write_uart3(char *buffer, int count)
{
	for (int i = 0; i < count; i ++)
	{
		// USART_SendData(USART3, buffer[i]);
		while((USART3 -> SR & 0X40) == 0);//循环发送,直到发送完毕
		USART3 -> DR = (uint8_t)buffer[i];
	}
}
