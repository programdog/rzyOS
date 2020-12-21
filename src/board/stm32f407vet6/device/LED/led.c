#include "led.h" 



//初始化PC13.并使能GPIOC的时钟
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//使能GPIOC时钟

	//PC13初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	//LED对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//50MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);					//初始化GPIO
	// GPIO_ResetBits(GPIOC, GPIO_Pin_13);	//设置0，灯亮
	GPIO_SetBits(GPIOC, GPIO_Pin_13);	//设置1，灯灭
}
