#include "rzyOS_schedule.h"
#include "rzyOSarch.h"
#include "rzyOS_wqueue.h"



//当前任务指针
task_tcb_s *currentTask;
//下一个准备执行的任务指针
task_tcb_s *nextTask;
//空闲任务指针
task_tcb_s *idleTask;

//定义一个以优先级划分的位图结构
bitmap_s bitmap_taskprio;

//定义 RZYOS_PRIO_COUNT（32）个双向链表管理结构， 用于同优先级的就绪任务管理
list_t task_ready_table[RZYOS_PRIO_COUNT];

//计数调度锁
uint8_t schedLockCount;

//定义延时链表
list_t task_delay_list;


//系统节拍计数
uint32_t tick_count;

#if RZYOS_ENABLE_CPU_DETECT == 1
//空闲任务运行节拍统计
uint32_t idle_count;
//空闲任务满负荷运行节拍统计
uint32_t idle_max_count;
//cpu使用率检测函数
static void check_cpu_usage_detect(void);
#endif


//从位图中找到就绪的最高优先级
//按照就绪的最高优先级在就绪任务列表中获取第一个任务节点
//从第一个任务节点得到对应的task的TCB
task_tcb_s *task_highest_ready(void)
{
	//先找到位图中标记的最高优先级组
	uint32_t highest_prio = bitmap_get_first_set(&bitmap_taskprio);
	//找到该优先级组中第一个任务就绪节点
	node_t *node = list_first_node(&task_ready_table[highest_prio]);
	//返回任务指针
	return node_parent(node, task_tcb_s, link_node);
}

//按照优先级,把就绪的任务的任务节点插入就绪任务list中
void task_insert_ready_list(task_tcb_s *task_tcb)
{
	//把任务节点按照顺序添加到优先级队列中
	list_add_last(&task_ready_table[task_tcb -> prio], &(task_tcb -> link_node));
	bitmap_set(&bitmap_taskprio, task_tcb -> prio);
}

//把任务从就绪队列中删移出(内部使用)
void task_remove_ready_list(task_tcb_s *task_tcb)
{
	//当task使用延时函数,会调用此函数

	//认为即将开启新的延时, 所以要在就绪list中删除当前任务的任务节点
	list_remove_pos_node(&task_ready_table[task_tcb -> prio], &(task_tcb -> link_node));
	
	if (0 == list_count(&task_ready_table[task_tcb -> prio]))
	{
		bitmap_clean(&bitmap_taskprio, task_tcb -> prio);
	}
}

//把任务从就绪队列中移出
void rzyOS_task_ready_list_remove(task_tcb_s *task_tcb)
{
	list_remove_pos_node(&task_ready_table[task_tcb -> prio], &(task_tcb -> link_node));
	
	if (0 == list_count(&task_ready_table[task_tcb -> prio]))
	{
		bitmap_clean(&bitmap_taskprio, task_tcb -> prio);
	}
}

//task调度, 切换到最高优先级中,延时用完的task
//任务处于运行状态时依然在就绪列表中
void task_schedule(void)
{
	task_tcb_s *tempTask;
	uint32_t status = task_enter_critical();
	
	//计数调度锁 > 0 ； 禁止调度
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

//调度失能 , 计数调度锁 + 1
void task_schedule_disable(void)
{
	uint32_t status = task_enter_critical();
	
	if (schedLockCount < 0xff)
	{
		schedLockCount ++;
	}
	
	task_exit_critical(status);
}

//调度使能 , 计数调度锁 - 1
void task_schedule_enable(void)
{
	uint32_t status = task_enter_critical();
	
	if (schedLockCount > 0)
	{
		schedLockCount --;

		//当正好减到0 , 则切入调度
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



//在延时list中加入当前任务的延时节点
void delay_list_insert_time_node(task_tcb_s *task_tcb, uint32_t ticks)
{
	//当task使用延时函数,会调用此函数

	//在当前任务的TCB中写入需要的延时
	task_tcb -> delayTicks = ticks;
	list_add_first(&task_delay_list, &(task_tcb -> delay_node));
	//标注任务状态为延时等待
	task_tcb -> task_status |= RZYOS_TASK_STATUS_DELAY;
}

//把任务在延时队列中删除(内部调用)
void delay_list_remove_time_node(task_tcb_s *task_tcb)
{
	list_remove_pos_node(&task_delay_list, &(task_tcb -> delay_node));
	//清除任务延时等待状态
	task_tcb -> task_status &= ~RZYOS_TASK_STATUS_DELAY;
}

//把任务在延时队列中删除(外部调用)
void rzyOS_task_delay_list_remove(task_tcb_s *task_tcb)
{
	delay_list_remove_time_node(task_tcb);
}

//系统节拍初始化
void rzyOS_tick_count_init(void)
{
	tick_count = 0;
}

//systick中断调用此函数
void task_systemtick_handler(void)
{
	node_t *node;
	
	uint32_t status = task_enter_critical();
	
	//遍历延时等待队列
	for (node = task_delay_list.head_node.next_node; node != &(task_delay_list.head_node); node = node -> next_node)
	{
		//根据延时节点，获取任务控制块
		task_tcb_s *task_tcb =  (task_tcb_s *)node_parent(node, task_tcb_s, delay_node);
		//任务延时递减
		task_tcb -> delayTicks --;

		//延时已为0
		if (0 == task_tcb -> delayTicks)
		{
			//若任务仍处于事件等待状态
			if (task_tcb -> wait_event)
			{
				//如果事件还未到来,但延时到达, 则从事件等待列表删除
				rzyOS_event_remove(task_tcb, (void *)0, error_timeout);
			}

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

	//系统节拍累加
	tick_count ++;

#if RZYOS_ENABLE_CPU_DETECT == 1

	//cpu使用率检测
	check_cpu_usage_detect();
#endif
	
	task_exit_critical(status);

#if RZYOS_ENABLE_WQUEUE == 1
	//系统tick， 周期性调用工作队列处理函数
	rzyOS_wqueue_tick_handle();
#endif
	
	task_schedule();
}

//不会引起任务调度的延时
void delay(int count)
{
	while(-- count > 0);
}


#if RZYOS_ENABLE_CPU_DETECT == 1

//cpu使用率
static float cpu_usage;
//cpu同步等待标志
static uint32_t cpu_sync_flag;

//cpu使用率检测模块变量初始化
static void cpu_usage_state_init(void)
{
	idle_count = 0;
	idle_max_count = 0;
	cpu_usage = 0.0f;
	cpu_sync_flag = 0;
}

//cpu使用率检测函数
static void check_cpu_usage_detect(void)
{
	//进入此函数则代表cpu同步
	//若为cpu未进入同步状态，则置位cpu_sync_flag
	//只置位cpu_sync_flag一次
	if (0 == cpu_sync_flag)
	{
		//置位cpu_sync_flag
		cpu_sync_flag = 1;
		return ;
	}

	//系统节拍计数第1秒
	if (ONE_SECOND == tick_count)
	{
		//把第一秒的空闲任务满负荷节拍计数赋值
		idle_max_count = idle_count;
		idle_count = 0;

		//开启调度
		task_schedule_enable();
	}
	//每当tick_count计数1秒
	else if (0 == tick_count % ONE_SECOND)
	{
		//计算cpu使用率
		cpu_usage = 100 - (float)100.0 * (float)idle_count / (float)idle_max_count;
		idle_count = 0;
	}
}

static void cpu_tick_sync(void)
{
	//cpu未同步，则while(1)等待
	while (0 == cpu_sync_flag)
	{
		;;;;;
	}
}

//获取cpu使用率，并返回
float rzyOS_get_cpu_usage(void)
{
	float usage = 0.0f;

	uint32_t status = task_enter_critical();
	usage = cpu_usage;
	task_exit_critical(status);

	return usage;
}

#endif

static task_tcb_s tcb_task_idle;
static tTaskStack idleTaskEnv[RZYOS_IDLETASK_STACK_SIZE];

void idle_task_entry(void *param)
{
	//关闭任务调度
	task_schedule_disable();

//	//app任务初始化
//	rzyOS_app_init();

#if RZYOS_ENABLE_WQUEUE == 1
	//工作队列任务初始化
	rzyOS_wqueue_task_init();
#endif

//	//设定systick中断时间周期
//	 set_systick_period(RZYOS_TICK_MS);

#if RZYOS_ENABLE_CPU_DETECT == 1
	//cpu同步等待
	cpu_tick_sync();
#endif 

	for (;;)
	{
#if RZYOS_ENABLE_CPU_DETECT == 1
		uint32_t status = task_enter_critical();
		//空闲任务运行节拍计数
		idle_count ++;
		task_exit_critical(status);
#endif 
	}
}

void rzyOS_kernel_init(void)
{
	task_schedule_init();
	
	task_delay_list_init();

#if RZYOS_ENABLE_WQUEUE == 1
	rzyOS_wqueue_module_init();
#endif

	//系统节拍变量初始化
	rzyOS_tick_count_init();

#if RZYOS_ENABLE_CPU_DETECT == 1
	//cpu 状态检测模块变量初始化
	cpu_usage_state_init();
#endif

#if (RZYOS_ENABLE_MEMORY == 1) && (RZYOS_MM1_USE == 1)
	//初始化memory方式1内存池
	rzyOS_memory_mm1_init();
#endif

	task_init(&tcb_task_idle, idle_task_entry, (void *)0, RZYOS_IDLETASK_PRIO, idleTaskEnv, RZYOS_IDLETASK_STACK_SIZE);
	idleTask = &tcb_task_idle;
}
