#include "rzyOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL	0xe000ed04
#define	NVIC_PENDSVSET	0X10000000
#define	NVIC_SYSPRI2	0xe000ed22
#define	NVIC_PENDSV_PRI	0x000000ff

#define MEM32(addr)	*(volatile unsigned long *)(addr)
#define MEM8(addr)	*(volatile unsigned char *)(addr)

__asm void PendSV_Handler(void)
{
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP
	CBZ R0, PendSVHander_nosave
	NOP
	
PendSVHander_nosave
	LDR R0, =currentTask
	LDR R1, =nextTask
	LDR R2, [R1]
	STR R2, [R0]
	
	LDR R0, [R2]
	LDMIA R0!, {R4-R11}
	
	MSR PSP, R0
	ORR LR, LR, #0X04
	BX LR
}

void tTaskRunFirst(void)
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
void tTaskSwitch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
