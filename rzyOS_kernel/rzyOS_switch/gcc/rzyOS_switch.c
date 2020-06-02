#include "rzyOS_schedule.h"
#include "rzyOSarch.h"


// void PendSV_Handler(void) __attribute__ (( naked ));

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

uint32_t saveAndLoadStackAddr(uint32_t stackAddr)
{
	//第一次切换时， 当前任务tcb为0， 所以不会保存
	if (currentTask != (task_tcb_s *)0)
	{
		//保存堆栈地址
		currentTask -> stack = (uint32_t *)stackAddr;
	}

	currentTask = nextTask;

	//取下一任务堆栈地址
	return (uint32_t)currentTask -> stack;
}

void PendSV_Handler(void)
{
	__asm volatile
	(

	// 切换第一个任务时,由于设置了PSP=MSP，所以下面的STMDB保存会将R4~R11
	// 保存到系统启动时默认的MSP堆栈中，而不是某个任务
	"MRS     R0, PSP\n"

	"STMDB   R0!, {R4-R11}\n"				// 将R4~R11保存到当前任务栈，也就是PSP指向的堆栈
	"it eq								\n"
	"vstmdbeq  R0!, {S16-S31}\n"				// 保存浮点S16-31
	"BL      saveAndLoadStackAddr\n"		// 调用函数：参数通过R0传递，返回值也通过R0传递
	"it eq								\n"
	"vldmiaeq  R0!, {S16-S31}\n"				// 恢复浮点S16-31
	"LDMIA   R0!, {R4-R11}\n"				// 从下一任务的堆栈中，恢复R4~R11

	"MSR     PSP, R0\n"
	"MOV     LR, #0xFFFFFFED\n"				// 指明返回异常时使用PSP。注意，这时LR不是程序返回地址
	"BX      LR\n"
	);
}

// void PendSV_Handler(void)
// {
// 	__asm volatile
// 	(

// 	// 切换第一个任务时,由于设置了PSP=MSP，所以下面的STMDB保存会将R4~R11
// 	// 保存到系统启动时默认的MSP堆栈中，而不是某个任务
// 	"MRS     R0, PSP\n"

// 	"STMDB   R0!, {R4-R11}\n"				// 将R4~R11保存到当前任务栈，也就是PSP指向的堆栈
// 	"it eq								\n"
// 	"vstmdbeq  R0!, {S16-S31}\n"				// 保存浮点S16-31
// 	"BL      saveAndLoadStackAddr\n"		// 调用函数：参数通过R0传递，返回值也通过R0传递
// 	"it eq								\n"
// 	"vldmiaeq  R0!, {S16-S31}\n"				// 恢复浮点S16-31
// 	"LDMIA   R0!, {R4-R11}\n"				// 从下一任务的堆栈中，恢复R4~R11

// 	"MSR     PSP, R0\n"
// 	"MOV     LR, #0xFFFFFFED\n"				// 指明返回异常时使用PSP。注意，这时LR不是程序返回地址
// 	"BX      LR\n"
// 	);
// }
