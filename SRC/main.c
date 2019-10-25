#include "rzyOS.h"
#include "ARMCM3.h"

tTask *currentTask;
tTask *nextTask;
tTask *idleTask;

bitmap_s bitmap_taskprio;
tTask *task_ready_table[RZYOS_PRIO_COUNT];

uint8_t schedLockCount;


void tTaskInit(tTask *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack)
{
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)entry;
	*(--stack) = (unsigned long)0x14;
	*(--stack) = (unsigned long)0x12;
	*(--stack) = (unsigned long)0x03;
	*(--stack) = (unsigned long)0x02;
	*(--stack) = (unsigned long)0x01;
	*(--stack) = (unsigned long)param;
	
	*(--stack) = (unsigned long)0x11;
	*(--stack) = (unsigned long)0x10;
	*(--stack) = (unsigned long)0x09;
	*(--stack) = (unsigned long)0x08;
	*(--stack) = (unsigned long)0x07;
	*(--stack) = (unsigned long)0x06;
	*(--stack) = (unsigned long)0x05;
	*(--stack) = (unsigned long)0x04;
	
	task -> stack = stack;
	task -> delayTicks = 0;
	task -> prio = prio;
	
	task_ready_table[prio] = task;
	bitmap_set(&bitmap_taskprio, prio);
}

tTask *tTaskHightestReady(void)
{
	uint32_t highest_prio = bitmap_get_first_set(&bitmap_taskprio);
	return task_ready_table[highest_prio];
}
void tTaskSched(void)
{
	tTask *tempTask;
	uint32_t status = tTaskEnterCritical();
	
	if (schedLockCount > 0)
	{
		tTaskExitCritical(status);
		return ;
	}
	
	tempTask = tTaskHightestReady();
	if (tempTask != currentTask)
	{
		nextTask = tempTask;
		tTaskSwitch();
	}
	
	tTaskExitCritical(status);
}

void tTaskSchedInit(void)
{
	schedLockCount = 0;
	bitmap_init(&bitmap_taskprio);
}

void tTaskSchedDisable(void)
{
	uint32_t status = tTaskEnterCritical();
	
	if (schedLockCount < 0xff)
	{
		schedLockCount ++;
	}
	
	tTaskExitCritical(status);
}

void tTaskschedEnable(void)
{
	uint32_t status = tTaskEnterCritical();
	
	if (schedLockCount > 0)
	{
		schedLockCount --;
		if (0 == schedLockCount)
		{
			tTaskSched();
		}
	}
	
	tTaskExitCritical(status);
}

void tTasksystemTickHandler()
{
	uint32_t status = tTaskEnterCritical();
	
	int i;
	for (i = 0; i < RZYOS_PRIO_COUNT; i ++)
	{
		if (task_ready_table[i] -> delayTicks > 0)
		{
			(task_ready_table[i] -> delayTicks) --;
		}
		else
		{
			bitmap_set(&bitmap_taskprio, i);
		}
	}
	
	tTaskExitCritical(status);
	tTaskSched();
}

void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	currentTask -> delayTicks = delay;
	bitmap_clean(&bitmap_taskprio, currentTask -> prio);
	
	tTaskExitCritical(status);
	tTaskSched();
}

//modify system_ARMCM3.C to change XTAL and SYSTEM_CLOCK
//#define  XTAL            (12000000UL)     /* Oscillator frequency */
//#define  SYSTEM_CLOCK    (1 * XTAL)
void SetSysTickPeriod(uint32_t ms)
{
	SysTick -> LOAD = ms * SystemCoreClock / 1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick -> VAL = 0;
	SysTick -> CTRL = SysTick_CTRL_CLKSOURCE_Msk |
						SysTick_CTRL_TICKINT_Msk |		//ÖÐ¶ÏÊ¹ÄÜ
						SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler()
{
	tTasksystemTickHandler();
}

void delay(int count)
{
	while(-- count > 0);
}

int task1Flag;
void task1Entry(void *param)
{
	SetSysTickPeriod(10);
	
	for (;;)
	{
		
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
	}
}

int task2Flag;
void task2Entry(void *param)
{
	for (;;)
	{
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
	}
}

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

tTask tTaskIdle;
tTaskStack idleTaskEnv[1024];
void idleTaskEntry(void *param)
{
	for (;;)
	{
	}
}

int main()
{
	tTaskSchedInit();
	
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
	
	tTaskInit(&tTaskIdle, idleTaskEntry, (void *)0, RZYOS_PRIO_COUNT - 1, &idleTaskEnv[1024]);
	idleTask = &tTaskIdle;
	
	task_ready_table[0] = &tTask1;
	task_ready_table[1] = &tTask2;
	
	nextTask = tTaskHightestReady();
	
	tTaskRunFirst();
	
	return 0;
}
