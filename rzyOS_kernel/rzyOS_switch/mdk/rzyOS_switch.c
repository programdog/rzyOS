#include "rzyOS_schedule.h"
#include "rzyOSarch.h"
#include "r_string.h"


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


#ifdef ARMCM3_SIM
//pendsv中断服务函数
__asm void PendSV_Handler(void)
{
	IMPORT currentTask					// IMPORT后面跟随函数名或变量名，作用相当于C中的extern关键字，指出这些全局符号是在其它源文件中定义的
	IMPORT nextTask
	
	MRS R0, PSP							// MRS <gp_reg>, <special_reg> ;读特殊功能寄存器的值到通用寄存器
	CBZ R0, PendSVHander_nosave			// 比较，如果结果为 0 就转移
	STMDB R0!, {R4-R11}					// 存储R4-R11到 R0 地址处。每存一个字后(!) Rd 自增一次，32位宽度
	LDR R1, =currentTask				// 取currentTask这个变量符号的地址写到R1！注意，不是取currentTask的值
	LDR R1, [R1]						// R1= *R1
	STR R0, [R1]						// 把R0存储到R1地址处
	
PendSVHander_nosave
	LDR R0, =currentTask
	LDR R1, =nextTask
	LDR R2, [R1]
	STR R2, [R0]
	
	LDR R0, [R2]
	LDMIA R0!, {R4-R11}					// 从 R0 处读取多个字。写入R4-R11中, 每读一个字后 R0 自增一次，16位宽度
	
	MSR PSP, R0							// MRS <special_reg>,  <gp_reg> ;存储gp_reg的值到特殊功能寄存器
	ORR LR, LR, #0X04					// ORR按位或 标记使用PSP
	BX LR								// 最后返回，此时任务就会从堆栈中取出LR值，恢复到上次运行的位
}
#endif


#if defined(STM32F40_41xxx) || defined(STM32F40XX)
//pendsv中断服务函数
__asm void PendSV_Handler(void)
{
	IMPORT saveAndLoadStackAddr

	// 切换第一个任务时,由于设置了PSP=MSP，所以下面的STMDB保存会将R4~R11
	// 保存到系统启动时默认的MSP堆栈中，而不是某个任务
	MRS     R0, PSP

	STMDB   R0!, {R4-R11}				// 将R4~R11保存到当前任务栈，也就是PSP指向的堆栈
	VSTMDB  R0!, {S16-S31}				// 保存浮点S16-31
	BL      saveAndLoadStackAddr		// 调用函数：参数通过R0传递，返回值也通过R0传递 
	VLDMIA  R0!, {S16-S31}				// 恢复浮点S16-31
	LDMIA   R0!, {R4-R11}				// 从下一任务的堆栈中，恢复R4~R11

	MSR     PSP, R0
	MOV     LR, #0xFFFFFFED				// 指明返回异常时使用PSP。注意，这时LR不是程序返回地址
	BX      LR
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
#endif


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

#ifdef ARMCM3_SIM
//task初始化
void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack_bottom, uint32_t task_size)
{
	//定义栈顶(高地址)
	uint32_t *stack_top;
	//栈底赋值(低地址)
	task -> task_bottom_base = stack_bottom;
	//栈大小
	task -> task_stack_size = task_size;
	//栈清零
	memset(stack_bottom, 0, task_size);

	//计算栈顶地址
	stack_top = stack_bottom + task_size / sizeof(tTaskStack);

	//初始化任务堆栈，对应到通用寄存器
	*(--stack_top) = (unsigned long)(1 << 24);
	*(--stack_top) = (unsigned long)entry;
	*(--stack_top) = (unsigned long)0x14;
	*(--stack_top) = (unsigned long)0x12;
	*(--stack_top) = (unsigned long)0x03;
	*(--stack_top) = (unsigned long)0x02;
	*(--stack_top) = (unsigned long)0x01;
	*(--stack_top) = (unsigned long)param;
	
	*(--stack_top) = (unsigned long)0x11;
	*(--stack_top) = (unsigned long)0x10;
	*(--stack_top) = (unsigned long)0x09;
	*(--stack_top) = (unsigned long)0x08;
	*(--stack_top) = (unsigned long)0x07;
	*(--stack_top) = (unsigned long)0x06;
	*(--stack_top) = (unsigned long)0x05;
	*(--stack_top) = (unsigned long)0x04;
	
	//任务堆栈动态指针传递
	task -> stack = stack_top;
	//初始化任务tcb
	node_init(&(task -> link_node));
	task -> delayTicks = 0;
	task -> prio = prio;
	task -> task_status = RZYOS_TASK_STATUS_READY;
	task -> slice = RZYOS_SLICE_MAX;
	node_init(&(task -> delay_node));
	task -> suspend_count = 0;
	task -> clean = (void (*) (void *))0;
	task -> clean_param = (void *)0;
	task -> request_delete_flag = 0;
	
	//初始化结束后将任务插入就绪队列等待调度
	task_insert_ready_list(task);
}
#endif


#if defined(STM32F40_41xxx) || defined(STM32F40XX)
//task初始化
void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack_bottom, uint32_t task_size)
{
	//定义栈顶(高地址)
	uint32_t *stack_top;
	//栈底赋值(低地址)
	task -> task_bottom_base = stack_bottom;
	//栈大小
	task -> task_stack_size = task_size;
	//栈清零
	r_memset(stack_bottom, 0, task_size);

	//计算栈顶地址
	stack_top = stack_bottom + task_size / sizeof(tTaskStack);

	*(--stack_top) = (unsigned long)(0);					// 空,未用
	*(--stack_top) = (unsigned long)(0);					// FPSCR
	*(--stack_top) = (unsigned long)(0x15);					// S15
	*(--stack_top) = (unsigned long)(0x14);					// S14
	*(--stack_top) = (unsigned long)(0x13);					// S13
	*(--stack_top) = (unsigned long)(0x12);					// S12
	*(--stack_top) = (unsigned long)(0x11);					// S11
	*(--stack_top) = (unsigned long)(0x10);					// S10
	*(--stack_top) = (unsigned long)(0x9);					// S9
	*(--stack_top) = (unsigned long)(0x8);					// S8
	*(--stack_top) = (unsigned long)(0x7);					// S7
	*(--stack_top) = (unsigned long)(0x6);					// S6
	*(--stack_top) = (unsigned long)(0x5);					// S5
	*(--stack_top) = (unsigned long)(0x4);					// S4
	*(--stack_top) = (unsigned long)(0x3);					// S3
	*(--stack_top) = (unsigned long)(0x2);					// S2
	*(--stack_top) = (unsigned long)(0x1);					// S1
	*(--stack_top) = (unsigned long)(0x0);					// S0

	*(--stack_top) = (unsigned long)(1 << 24);				// XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
	*(--stack_top) = (unsigned long)entry;					// 程序的入口地址
	*(--stack_top) = (unsigned long)0x14;					// R14(LR), 任务不会通过return xxx结束自己，所以未用
	*(--stack_top) = (unsigned long)0x12;					// R12, 未用
	*(--stack_top) = (unsigned long)0x3;					// R3, 未用
	*(--stack_top) = (unsigned long)0x2;					// R2, 未用
	*(--stack_top) = (unsigned long)0x1;					// R1, 未用
	*(--stack_top) = (unsigned long)param;					// R0 = param, 传给任务的入口函数
	*(--stack_top) = (unsigned long)0x11;					// R11, 未用
	*(--stack_top) = (unsigned long)0x10;					// R10, 未用
	*(--stack_top) = (unsigned long)0x9;					// R9, 未用
	*(--stack_top) = (unsigned long)0x8;					// R8, 未用
	*(--stack_top) = (unsigned long)0x7;					// R7, 未用
	*(--stack_top) = (unsigned long)0x6;					// R6, 未用
	*(--stack_top) = (unsigned long)0x5;					// R5, 未用
	*(--stack_top) = (unsigned long)0x4;					// R4, 未用

	*(--stack_top) = (unsigned long)(0x31);					// S31
	*(--stack_top) = (unsigned long)(0x30);					// S30
	*(--stack_top) = (unsigned long)(0x29);					// S29
	*(--stack_top) = (unsigned long)(0x28);					// S28
	*(--stack_top) = (unsigned long)(0x27);					// S27
	*(--stack_top) = (unsigned long)(0x26);					// S26
	*(--stack_top) = (unsigned long)(0x25);					// S25
	*(--stack_top) = (unsigned long)(0x24);					// S24
	*(--stack_top) = (unsigned long)(0x23);					// S23
	*(--stack_top) = (unsigned long)(0x22);					// S22
	*(--stack_top) = (unsigned long)(0x21);					// S21
	*(--stack_top) = (unsigned long)(0x20);					// S20
	*(--stack_top) = (unsigned long)(0x19);					// S19
	*(--stack_top) = (unsigned long)(0x18);					// S18
	*(--stack_top) = (unsigned long)(0x17);					// S17
	*(--stack_top) = (unsigned long)(0x16);					// S16

	
	//任务堆栈动态指针传递
	task -> stack = stack_top;
	//初始化任务tcb
	node_init(&(task -> link_node));
	task -> delayTicks = 0;
	task -> prio = prio;
	task -> task_status = RZYOS_TASK_STATUS_READY;
	task -> slice = RZYOS_SLICE_MAX;
	node_init(&(task -> delay_node));
	task -> suspend_count = 0;
	task -> clean = (void (*) (void *))0;
	task -> clean_param = (void *)0;
	task -> request_delete_flag = 0;
	
	//初始化结束后将任务插入延时队列等待调度
	task_insert_ready_list(task);
}
#endif

void rzyOS_start(void)
{
	nextTask = task_highest_ready();

	SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk;

	task_run_first();
}

