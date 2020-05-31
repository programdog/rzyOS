#include "rzyOS_schedule.h"
#include "rzyOSarch.h"

uint32_t saveAndLoadStackAddr(uint32_t stackAddr);

//中断控制器地址
#define NVIC_INT_CTRL	0xe000ed04
//触发PendSV
#define	NVIC_PENDSVSET	0X10000000
//PendSV优先级控制地址
#define	NVIC_SYSPRI2	0xe000ed22
//PendSV设置为最低优先值255
#define	NVIC_PENDSV_PRI	0x000000ff



#define MEM32(addr)	*(volatile unsigned long *)(addr)
#define MEM8(addr)	*(volatile unsigned char *)(addr)

//进入临界区保护
uint32_t task_enter_critical(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

//退出临界区保护
void task_exit_critical(uint32_t status)
{
	__set_PRIMASK(status);
}


//从MSP转换PSP,配置pendsv优先级
//change MSP to PSP , then setup pendSV priority and trigger pendSV
void task_run_first(void)
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

//触发pendsv中断
//trigger pendSV
void task_switch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
