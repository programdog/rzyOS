#include "rzyOS_schedule.h"
#include "rzyOSarch.h"
#include "r_string.h"


void PendSV_Handler(void) __attribute__ (( naked ));

void rzyOS_swap_taskTCB(uint32_t stackAddr);

void rzyOS_EnableVFP(void) __attribute__ (( naked ));

void rzyOS_StartFirstTask(void) __attribute__ (( naked ));

void SVC_Handler(void) __attribute__ (( naked ));

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
	// __set_PSP(0);
	
	// MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	// MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
	rzyOS_StartFirstTask();
}

//触发pendsv中断
//trigger pendSV
void task_switch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void rzyOS_swap_taskTCB(uint32_t stackAddr)
{


	currentTask = nextTask;


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

// void PendSV_Handler(void)
// {
// 	__asm volatile
// 	(
// 	/* 切换第一个任务时,由于设置了PSP=MSP，所以下面的STMDB保存会将R4~R11 */
// 	/* 保存到系统启动时默认的MSP堆栈中，而不是某个任务 */
// 	"MRS     R0, PSP\n"
// 	"isb									\n"
// 	"STMDB   R0!, {R4-R11}\n"				/* 将R4~R11保存到当前任务栈，也就是PSP指向的堆栈 */
// 	"it eq								\n"
// 	"vstmdbeq  R0!, {S16-S31}\n"				/* 保存浮点S16-31 */
// 	"	dsb									\n"
// 	"	isb									\n"
// 	"BL      saveAndLoadStackAddr\n"		/* 调用函数：参数通过R0传递，返回值也通过R0传递 */
// 	"it eq								\n"
// 	"vldmiaeq  R0!, {S16-S31}\n"				/* 恢复浮点S16-31 */
// 	"LDMIA   R0!, {R4-R11}\n"				/* 从下一任务的堆栈中，恢复R4~R11 */

// 	"MSR     PSP, R0\n"
// 	"	isb									\n"
// 	"MOV     LR, #0xFFFFFFED\n"				/* 指明返回异常时使用PSP。注意，这时LR不是程序返回地址 */
// 	"BX      LR\n"
// 	);
// }


void rzyOS_EnableVFP(void)
{
	__asm volatile
	(
		"	ldr.w r0, =0xE000ED88		\n" /* The FPU enable bits are in the CPACR. */
		"	ldr r1, [r0]				\n"
		"								\n"
		"	orr r1, r1, #( 0xf << 20 )	\n" /* Enable CP10 and CP11 coprocessors, then save back. */
		"	str r1, [r0]				\n"
		"	bx r14						"
	);
}


void SVC_Handler( void )
{
	
	__asm volatile (
					"	ldr	r3, pxCurrentTCBConst2		\n" /* Restore the context. */
					"	ldr r1, [r3]					\n" /* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
					"	ldr r0, [r1]					\n" /* The first item in pxCurrentTCB is the task top of stack. */
					"	ldmia r0!, {r4-r11, r14}		\n" /* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
					"	msr psp, r0						\n" /* Restore the task stack pointer. */
					"	isb								\n"
					"	mov r0, #0 						\n"
					"	msr	basepri, r0					\n"
					"	bx r14							\n"
					"									\n"
					"	.align 4						\n"
					"pxCurrentTCBConst2: .word currentTask	\n"
				);
}

void rzyOS_StartFirstTask(void)
{

	__asm volatile(
					" ldr r0, =0xE000ED08 	\n" /* Use the NVIC offset register to locate the stack. */
					" ldr r0, [r0] 			\n"
					" ldr r0, [r0] 			\n"
					" msr msp, r0			\n" /* Set the msp back to the start of the stack. */
					" cpsie i				\n" /* Globally enable interrupts. */
					" cpsie f				\n"
					" dsb					\n"
					" isb					\n"
					" svc 0					\n" /* System call to start first task. */
					" nop					\n"
				);
}


void PendSV_Handler(void)
{
	/* This is a naked function. */

	__asm volatile
	(
	"	mrs r0, psp							\n"
	"	isb									\n"
	"										\n"
	"	ldr	r3, pxCurrentTCBConst			\n" /* Get the location of the current TCB. */
	"	ldr	r2, [r3]						\n"
	"										\n"
	"	tst r14, #0x10						\n" /* Is the task using the FPU context?  If so, push high vfp registers. */
	"	it eq								\n"
	"	vstmdbeq r0!, {s16-s31}				\n"
	"										\n"
	"	stmdb r0!, {r4-r11, r14}			\n" /* Save the core registers. */
	"										\n"
	"	str r0, [r2]						\n" /* Save the new top of stack into the first member of the TCB. */
	"										\n"
	"	stmdb sp!, {r3}						\n"
	"	mov r0, #0 							\n"
	"	msr basepri, r0						\n"
	"	dsb									\n"
	"	isb									\n"
	"	bl rzyOS_swap_taskTCB				\n"
	"	mov r0, #0							\n"
	"	msr basepri, r0						\n"
	"	ldmia sp!, {r3}						\n"
	"										\n"
	"	ldr r1, [r3]						\n" /* The first item in pxCurrentTCB is the task top of stack. */
	"	ldr r0, [r1]						\n"
	"										\n"
	"	ldmia r0!, {r4-r11, r14}			\n" /* Pop the core registers. */
	"										\n"
	"	tst r14, #0x10						\n" /* Is the task using the FPU context?  If so, pop the high vfp registers too. */
	"	it eq								\n"
	"	vldmiaeq r0!, {s16-s31}				\n"
	"										\n"
	"	msr psp, r0							\n"
	"	isb									\n"
	"										\n"
	"										\n"
	"	bx r14								\n"
	"pxCurrentTCBConst: .word currentTask	\n"
	);
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
//	stack_top = stack_bottom + (task_size / sizeof(tTaskStack) - 1);
	stack_top = stack_bottom + task_size / sizeof(tTaskStack);
	// stack_top = (uint32_t *)((uint32_t)stack_top & (~ ((uint32_t)0x0003)));

	// *(--stack_top) = (unsigned long)(0);					// 空,未用
	// *(--stack_top) = (unsigned long)(0);					// FPSCR
	// *(--stack_top) = (unsigned long)(0x15);					// S15
	// *(--stack_top) = (unsigned long)(0x14);					// S14
	// *(--stack_top) = (unsigned long)(0x13);					// S13
	// *(--stack_top) = (unsigned long)(0x12);					// S12
	// *(--stack_top) = (unsigned long)(0x11);					// S11
	// *(--stack_top) = (unsigned long)(0x10);					// S10
	// *(--stack_top) = (unsigned long)(0x9);					// S9
	// *(--stack_top) = (unsigned long)(0x8);					// S8
	// *(--stack_top) = (unsigned long)(0x7);					// S7
	// *(--stack_top) = (unsigned long)(0x6);					// S6
	// *(--stack_top) = (unsigned long)(0x5);					// S5
	// *(--stack_top) = (unsigned long)(0x4);					// S4
	// *(--stack_top) = (unsigned long)(0x3);					// S3
	// *(--stack_top) = (unsigned long)(0x2);					// S2
	// *(--stack_top) = (unsigned long)(0x1);					// S1
	// *(--stack_top) = (unsigned long)(0x0);					// S0

	*(--stack_top) = (unsigned long)(1 << 24);				// XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
	*(--stack_top) = ((unsigned long)entry) & ((unsigned long)0xfffffffeUL);					// 程序的入口地址
	*(--stack_top) = (unsigned long)0x14;					// R14(LR), 任务不会通过return xxx结束自己，所以未用
	*(--stack_top) = (unsigned long)0x12;					// R12, 未用
	*(--stack_top) = (unsigned long)0x3;					// R3, 未用
	*(--stack_top) = (unsigned long)0x2;					// R2, 未用
	*(--stack_top) = (unsigned long)0x1;					// R1, 未用
	*(--stack_top) = (unsigned long)param;					// R0 = param, 传给任务的入口函数
	*(--stack_top) = (unsigned long)0xfffffffd;				// A save method is being used that requires each task to maintain its own exec return value
	*(--stack_top) = (unsigned long)0x11;					// R11, 未用
	*(--stack_top) = (unsigned long)0x10;					// R10, 未用
	*(--stack_top) = (unsigned long)0x9;					// R9, 未用
	*(--stack_top) = (unsigned long)0x8;					// R8, 未用
	*(--stack_top) = (unsigned long)0x7;					// R7, 未用
	*(--stack_top) = (unsigned long)0x6;					// R6, 未用
	*(--stack_top) = (unsigned long)0x5;					// R5, 未用
	*(--stack_top) = (unsigned long)0x4;					// R4, 未用

	// *(--stack_top) = (unsigned long)(0x31);					// S31
	// *(--stack_top) = (unsigned long)(0x30);					// S30
	// *(--stack_top) = (unsigned long)(0x29);					// S29
	// *(--stack_top) = (unsigned long)(0x28);					// S28
	// *(--stack_top) = (unsigned long)(0x27);					// S27
	// *(--stack_top) = (unsigned long)(0x26);					// S26
	// *(--stack_top) = (unsigned long)(0x25);					// S25
	// *(--stack_top) = (unsigned long)(0x24);					// S24
	// *(--stack_top) = (unsigned long)(0x23);					// S23
	// *(--stack_top) = (unsigned long)(0x22);					// S22
	// *(--stack_top) = (unsigned long)(0x21);					// S21
	// *(--stack_top) = (unsigned long)(0x20);					// S20
	// *(--stack_top) = (unsigned long)(0x19);					// S19
	// *(--stack_top) = (unsigned long)(0x18);					// S18
	// *(--stack_top) = (unsigned long)(0x17);					// S17
	// *(--stack_top) = (unsigned long)(0x16);					// S16

	
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
	rzyOS_EnableVFP();

	*(( volatile uint32_t * ) 0xe000ef34) |= (0x3UL << 30UL);

	nextTask = task_highest_ready();
	
	currentTask = nextTask;

	SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk;

	task_run_first();
}
