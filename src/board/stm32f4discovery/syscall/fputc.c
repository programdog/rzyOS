#if 1

#include "stdio.h"
#include "stm32f4xx.h"

/* 告知连接器不从C库链接使用半主机的函数 */
#pragma import(__use_no_semihosting)

//标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}

//重定义fputc函数
int fputc(int ch, FILE *f)
{
	while((USART3 -> SR & 0X40) == 0);//循环发送,直到发送完毕

	USART3 -> DR = (uint8_t)ch;

	return ch;
}
#endif
