#include "rzyOS.h"
#include "ARMCM3.h"

task_tcb_s *currentTask;
task_tcb_s *nextTask;
task_tcb_s *idleTask;

bitmap_s bitmap_taskprio;
list_t task_ready_table[RZYOS_PRIO_COUNT];

uint8_t schedLockCount;

list_t task_delay_list;


//从位图中找到就绪的最高优先级
//按照就绪的最高优先级在就绪任务列表中获取第一个任务节点
//从第一个任务节点得到对应的task的TCB
task_tcb_s *task_highest_ready(void)
{
	uint32_t highest_prio = bitmap_get_first_set(&bitmap_taskprio);
	node_t *node = list_first_node(&task_ready_table[highest_prio]);
	return node_parent(node, task_tcb_s, link_node);
}

//按照优先级,把就绪的任务的任务节点插入就绪任务list中
void task_insert_ready_list(task_tcb_s *task_tcb)
{
	//把任务节点添加到优先级队列中
	list_add_first(&task_ready_table[task_tcb -> prio], &(task_tcb -> link_node));
	bitmap_set(&bitmap_taskprio, task_tcb -> prio);
}

//当task使用延时函数,会调用此函数
//认为即将开启新的延时, 所以要在就绪list中删除当前任务的任务节点
void task_remove_ready_list(task_tcb_s *task_tcb)
{
	list_remove_pos_node(&task_ready_table[task_tcb -> prio], &(task_tcb -> link_node));
	
	if (0 == list_count(&task_ready_table[task_tcb -> prio]))
	{
		bitmap_clean(&bitmap_taskprio, task_tcb -> prio);
	}
}

//task调度, 切换到最高优先级中,延时用完的task
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
		task_switch(); //触发pendSV
	}
	
	task_exit_critical(status);
}

//初始化位图
//初始化任务就绪表list数组(数组按照优先级划分)
void task_schedule_init(void)
{
	int i;
	
	schedLockCount = 0;
	bitmap_init(&bitmap_taskprio);
	for (i = 0; i < RZYOS_PRIO_COUNT; i ++)
	{
		list_init(&task_ready_table[i]);
	}
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

//初始化延时列表list
void task_delay_list_init(void)
{
	list_init(&task_delay_list);
}

//当task使用延时函数,会调用此函数
//在当前任务的TCB中写入需要的延时
//在延时list中加入当前任务的延时节点
void delay_list_insert_time_node(task_tcb_s *task_tcb, uint32_t ticks)
{
	task_tcb -> delayTicks = ticks;
	list_add_first(&task_delay_list, &(task_tcb -> delay_node));
	task_tcb -> ready_status |= RZYOS_TASK_STATUS_DELAY;
}

//在延时队列中删除delay已经为0的延时节点
void delay_list_remove_time_node(task_tcb_s *task_tcb)
{
	list_remove_pos_node(&task_delay_list, &(task_tcb -> delay_node));
	task_tcb -> ready_status &= ~RZYOS_TASK_STATUS_DELAY;
}

//systick中断调用此函数
void task_systemtick_handler(void)
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
	
	//时间片轮转
	//因为调度的nextTask总是优先级列表的地第一个节点
	//所以基于时间片调度要对第一个节点操作,也就是currentTask
	currentTask -> slice --;
	//判断当前任务时间片是否用完
	if (0 == currentTask -> slice)
	{
		//如果用完, 且当前任务的同优先级任务list还有任务, 则切换轮转
		if (list_count(&task_ready_table[currentTask -> prio]) > 0)
		{
			//移除同优先级任务list中的任务节点, 并插入到最后一个节点, 完成基于同优先级的时间片调度
			remove_list_first(&task_ready_table[currentTask -> prio]);
			list_add_last(&task_ready_table[currentTask -> prio], &(currentTask -> link_node));
			
			//重置时间片
			currentTask -> slice = RZYOS_SLICE_MAX;
		}
	}
	
	task_exit_critical(status);
	
	task_schedule();
}

void delay(int count)
{
	while(-- count > 0);
}


task_tcb_s tcb_task_idle;
tTaskStack idleTaskEnv[RZYOS_IDLETASK_STACK_SIZE];

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
	
	rzyOS_app_init();
	
	task_init(&tcb_task_idle, idle_task_entry, (void *)0, RZYOS_PRIO_COUNT - 1, &idleTaskEnv[RZYOS_IDLETASK_STACK_SIZE]);
	idleTask = &tcb_task_idle;
	
	
	nextTask = task_highest_ready();
	
	task_run_first();
	
	return 0;
}
