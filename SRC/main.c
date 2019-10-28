#include "rzyOS.h"
#include "ARMCM3.h"

task_tcb_s *currentTask;
task_tcb_s *nextTask;
task_tcb_s *idleTask;

bitmap_s bitmap_taskprio;
task_tcb_s *task_ready_table[RZYOS_PRIO_COUNT];

uint8_t schedLockCount;

list_t task_delay_list;

void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack)
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
	
	task -> ready_status = RZYOS_TASK_STATUS_READY;
	node_init(&(task -> delay_node));
	
	
	task_ready_table[prio] = task;
	bitmap_set(&bitmap_taskprio, prio);
}

task_tcb_s *task_highest_ready(void)
{
	uint32_t highest_prio = bitmap_get_first_set(&bitmap_taskprio);
	return task_ready_table[highest_prio];
}

void task_insert_ready_list(task_tcb_s *task_tcb)
{
	task_ready_table[task_tcb -> prio] = task_tcb;
	bitmap_set(&bitmap_taskprio, task_tcb -> prio);
}

void task_remove_ready_list(task_tcb_s *task_tcb)
{
	task_ready_table[task_tcb -> prio] = (task_tcb_s *)0;
	bitmap_clean(&bitmap_taskprio, task_tcb -> prio);
}

void task_schedule(void)
{
	task_tcb_s *tempTask;
	uint32_t status = task_enter_critical();
	
	if (schedLockCount > 0)
	{
		task_exit_critical(status);
		return ;
	}
	
	tempTask = task_highest_ready();
	if (tempTask != currentTask)
	{
		nextTask = tempTask;
		task_switch();
	}
	
	task_exit_critical(status);
}

void task_schedule_init(void)
{
	schedLockCount = 0;
	bitmap_init(&bitmap_taskprio);
}

void task_schedule_disable(void)
{
	uint32_t status = task_enter_critical();
	
	if (schedLockCount < 0xff)
	{
		schedLockCount ++;
	}
	
	task_exit_critical(status);
}

void task_schedule_enable(void)
{
	uint32_t status = task_enter_critical();
	
	if (schedLockCount > 0)
	{
		schedLockCount --;
		if (0 == schedLockCount)
		{
			task_schedule();
		}
	}
	
	task_exit_critical(status);
}

void task_delay_list_init(void)
{
	list_init(&task_delay_list);
}

void delay_list_insert_time_node(task_tcb_s *task_tcb, uint32_t ticks)
{
	task_tcb -> delayTicks = ticks;
	list_add_first(&task_delay_list, &(task_tcb -> delay_node));
	task_tcb -> ready_status |= RZYOS_TASK_STATUS_DELAY;
}

void delay_list_remove_time_node(task_tcb_s *task_tcb)
{
	list_remove_pos_node(&task_delay_list, &(task_tcb -> delay_node));
	task_tcb -> ready_status &= ~RZYOS_TASK_STATUS_DELAY;
}

void task_systemtick_handler()
{
	node_t *node;
	
	uint32_t status = task_enter_critical();
	
	for (node = task_delay_list.head_node.next_node; node != &(task_delay_list.head_node); node = node -> next_node)
	{
		task_tcb_s *task_tcb =  (task_tcb_s *)node_parent(node, task_tcb_s, delay_node);
		task_tcb -> delayTicks --;
		if (0 == task_tcb -> delayTicks)
		{
			delay_list_remove_time_node(task_tcb);
			
			task_insert_ready_list(task_tcb);
		}
	}
	
	task_exit_critical(status);
	
	task_schedule();
}

void task_delay(uint32_t delay)
{
	uint32_t status = task_enter_critical();
	
	delay_list_insert_time_node(currentTask, delay);

	task_remove_ready_list(currentTask);
	
	task_exit_critical(status);
	task_schedule();
}

//modify system_ARMCM3.C to change XTAL and SYSTEM_CLOCK
//#define  XTAL            (12000000UL)     /* Oscillator frequency */
//#define  SYSTEM_CLOCK    (1 * XTAL)
void set_systick_period(uint32_t ms)
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
	task_systemtick_handler();
}

void delay(int count)
{
	while(-- count > 0);
}

int task1Flag;
list_t list;
node_t node[8];

void task1_entry(void *param)
{
	set_systick_period(10);

	for (;;)
	{
		task1Flag = 0;
		task_delay(1);
		task1Flag = 1;
		task_delay(1);
	}
}

int task2Flag;
void task2_entry(void *param)
{
	for (;;)
	{
		task2Flag = 0;
		task_delay(1);
		task2Flag = 1;
		task_delay(1);
	}
}

task_tcb_s tcb_task1;
task_tcb_s tcb_task2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

task_tcb_s tcb_task_idle;
tTaskStack idleTaskEnv[1024];
void idle_task_entry(void *param)
{
	for (;;)
	{
	}
}

int main()
{
	task_schedule_init();
	
	task_delay_list_init();
	
	task_init(&tcb_task1, task1_entry, (void *)0x11111111, 0, &task1Env[1024]);
	task_init(&tcb_task2, task2_entry, (void *)0x22222222, 1, &task2Env[1024]);
	
	task_init(&tcb_task_idle, idle_task_entry, (void *)0, RZYOS_PRIO_COUNT - 1, &idleTaskEnv[1024]);
	idleTask = &tcb_task_idle;
	
	task_ready_table[0] = &tcb_task1;
	task_ready_table[1] = &tcb_task2;
	
	nextTask = task_highest_ready();
	
	task_run_first();
	
	return 0;
}
