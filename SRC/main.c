#include "rzyOS.h"

#define NVIC_INT_CTRL	0xe000ed04
#define	NVIC_PENDSVSET	0X10000000
#define	NVIC_SYSPRI2	0xe000ed22
#define	NVIC_PENDSV_PRI	0x000000ff

#define MEM32(addr)	*(volatile unsigned long *)(addr)
#define MEM8(addr)	*(volatile unsigned char *)(addr)

void triggerPendSV(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void tTaskInit(tTask *task, void (*entry)(void *), void *param, tTaskStack *stack)
{
	task -> stack = stack;
}

typedef struct Blocktype_t
{
	unsigned long *stackptr;
} Blocktype_t;

Blocktype_t *blockPtr; 

void delay(int count)
{
	while(--count > 0);
}

int flag;
unsigned long stack_bufffer[1024];
Blocktype_t block;

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

void task1(void *param)
{
	for (;;)
	{
		
	}
}

void task2(void *param)
{
	for (;;)
	{
		
	}
}

int main()
{
	tTaskInit(&tTask1, task1, (void *)0x11111111, &task2Env[1023]);
	tTaskInit(&tTask1, task1, (void *)0x22222222, &task2Env[1023]);
	
	block.stackptr = &stack_bufffer[1024];
	blockPtr = &block;
	for (;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
//		triggerPendSV();
	}
}
