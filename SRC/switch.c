#include "rzyOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL	0xe000ed04
#define	NVIC_PENDSVSET	0X10000000
#define	NVIC_SYSPRI2	0xe000ed22
#define	NVIC_PENDSV_PRI	0x000000ff

#define MEM32(addr)	*(volatile unsigned long *)(addr)
#define MEM8(addr)	*(volatile unsigned char *)(addr)

uint32_t task_enter_critical(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

void task_exit_critical(uint32_t status)
{
	__set_PRIMASK(status);
}

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

void task_run_first(void)
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
void task_switch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
