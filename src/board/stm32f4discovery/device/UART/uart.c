#include <stdio.h>
#include "uart.h"

//Configure USART3(PB10, PB11) to redirect printf data to host PC.
void USART3_Init(uint32_t boundrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	//复用功能
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	//推挽
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//上拉
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




	// #if ENABLE_USART3_RX == 1
	//开启相关中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	//Usart3 NVIC 配置
	//串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//IRQ通道使能
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// #endif
}




//串口3中断服务程序
// void USART3_IRQHandler(void)
// {
// 	if (SET == USART_GetFlagStatus(USART3, USART_FLAG_RXNE))
// 	{
// 		uint8_t rec = USART_ReceiveData(USART3);

// 		// USART_SendData(USART3, rec);

// 		// uart3_rx_buffer_write(rec);
// 		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
// 	}
// }

//串口3中断服务程序
void USART3_IRQHandler(void)
{
	if (USART_GetFlagStatus(USART3, USART_FLAG_PE) != RESET)
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_PE);
	}
	//溢出中断
	//溢出中断的产生是当RXNE=1时（也就是上次数据还没被读取），
	//串口接收寄存器又接收好了一个字节的数据并准备往USART_RDR寄存器去转移的时候，
	//ORE标志位会被置1，而ORE标志位必须软件清零
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_ORE);
	}

	if (USART_GetFlagStatus(USART3, USART_FLAG_FE) != RESET)
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_FE);
	}

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		uint8_t rec = USART_ReceiveData(USART3);
		uart3_rx_buffer_write(rec);

		USART_ClearFlag(USART3, USART_FLAG_RXNE);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

