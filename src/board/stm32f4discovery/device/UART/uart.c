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




	// // #if ENABLE_USART3_RX == 1
	// //开启相关中断
	// USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	// //Usart3 NVIC 配置
	// //串口3中断通道
	// NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	// //抢占优先级3
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	// //子优先级3
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	// //IRQ通道使能
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);
	// // #endif
}


//串口3中断服务程序
void USART3_IRQHandler(void)
{

}
