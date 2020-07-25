#include "uart.h"

//Configure USART3(PB10, PB11) to redirect printf data to host PC.
void USART3_Init(uint32_t boundrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	//波特率设置
	USART_InitStruct.USART_BaudRate = boundrate;
	//字长为8位数据格式
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	//一个停止位
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	//无奇偶校验位
	USART_InitStruct.USART_Parity = USART_Parity_No;
	//无硬件数据流控制
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//收发模式
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	//初始化串口
	USART_Init(USART3, &USART_InitStruct);
	//使能串口
	USART_Cmd(USART3, ENABLE);
}
