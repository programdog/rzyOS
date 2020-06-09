#include "rzyOS_schedule.h"
#include "rzyOSarch.h"

//CM3 模拟模式
//systick中断周期配置
//modify system_ARMCM3.C to change XTAL and SYSTEM_CLOCK
//#define  XTAL            (12000000UL)     /* Oscillator frequency */
//#define  SYSTEM_CLOCK    (1 * XTAL)
void set_systick_period(uint32_t ms)
{
	SysTick -> LOAD = ms * SystemCoreClock / 1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick -> VAL = 0;
	SysTick -> CTRL = SysTick_CTRL_CLKSOURCE_Msk |
						SysTick_CTRL_TICKINT_Msk |		//中断使能
						SysTick_CTRL_ENABLE_Msk;
}

//stm32
//初始化systick函数
//SYSTICK的时钟固定为AHB时钟，SYSTICK的时钟频率为AHB的频率
//parameter ：
//uint8_t sysclk ： 系统时钟频率
void rzyOS_systick_init(uint8_t sysclk)
{
	uint32_t reload;
	SysTick -> CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

//	fac_us = sysclk;

//	//每秒钟的计数次数 单位为M
	reload = sysclk;

	//reload为24位寄存器,最大值:16777216,在168M下,约合0.0998s左右
	//（RZYOS_TICK_HZ = 1000 --> 1ms）
	reload *= 1000000 / RZYOS_TICK_HZ;

//	fac_ms = 1000 / RZYOS_TICK_HZ;			//代表OS可以延时的最少单位

	//使能SYSTICK中断
	SysTick -> CTRL |= SysTick_CTRL_TICKINT_Msk;
	//每1/RZYOS_TICK_HZ (S)中断一次
	SysTick -> LOAD = reload;
	//开启SYSTICK
	// SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk;
}

//systick中断函数
void SysTick_Handler()
{
	task_systemtick_handler();
}

//task中的延时函数,使用延时队列进行处理
//param: delay--systick周期计数
void task_delay(uint32_t delay)
{
	uint32_t status = task_enter_critical();

	//将当前任务插入延时等待列表，并传递延时时长
	delay_list_insert_time_node(currentTask, delay);

	//上一状态处于就绪列表，当前已插入延时等待列表，所以从就绪列表中移除
	task_remove_ready_list(currentTask);

	task_exit_critical(status);

	task_schedule();
}
