#include "rzyOS_wqueue.h"

#if RZYOS_ENABLE_WQUEUE == 1

//高速工作队列链表
static list_t rzyOS_high_wqueue_list;
//低速工作队列链表
static list_t rzyOS_low_wqueue_list;

//工作队列链表保护操作信号量(因为其他任务(调用rzyOS_wqueue_start)和工作队列任务都会对链表做操作)
static rzyOS_sem_s rzyOS_wqueue_protect_sem;
//系统节拍tick信号量
static rzyOS_sem_s rzyOS_wqueue_tick_sem;

//工作队列节点初始化函数
/**
 * @Author    remzhongyu
 * @DateTime  2020-04-11
 * @copyright [open  source]
 * @version   [V1.0]
 * @param rzyOS_wqueue_s *rzyOS_wqueue : 工作队列结构
 * @param uint32_t start_delay_tick ： 延时多久后启动
 * @param uint32_t period_tick ： 工作队列循环周期
 * @param worker_t worker ： 回调函数
 * @param void *arg ： 回调函数传入参数
 * @param uint32_t wqueue_config ： 工作队列模式
 */
void rzyOS_wqueue_init(rzyOS_wqueue_s *rzyOS_wqueue, uint32_t start_delay_tick, uint32_t period_tick, worker_t worker, void *arg, uint32_t wqueue_config)
{
	//队列节初始化
	node_init(&(rzyOS_wqueue -> node));
	//起始延时节拍
	rzyOS_wqueue -> start_delay_tick = start_delay_tick;
	//执行周期节拍
	rzyOS_wqueue -> period_tick = period_tick;

	//跟据起始延时节拍赋值当前计数节拍
	if (0 == start_delay_tick)
	{
		//为0时则一个周期后开始执行
		rzyOS_wqueue -> count_tick = period_tick;
	}
	else
	{
		rzyOS_wqueue -> count_tick = start_delay_tick;
	}

	//回调函数
	rzyOS_wqueue -> worker = worker;
	//回调函数参数
	rzyOS_wqueue -> arg = arg;
	//工作队列模式
	rzyOS_wqueue -> wqueue_config = wqueue_config;
	//当前状态
	rzyOS_wqueue -> rzyOS_wqueue_status = wqueue_create;
}

//启动指定工作队列节点(工作队列组件的外部任务调用)
void rzyOS_wqueue_start(rzyOS_wqueue_s *rzyOS_wqueue)
{
	//判断当前状态
	switch (rzyOS_wqueue -> rzyOS_wqueue_status)
	{
		//只有已经初始化和停止的才能启动
		case wqueue_create:
		case wqueue_stop:
		{
			//对计数tick赋值
			rzyOS_wqueue -> count_tick = rzyOS_wqueue -> start_delay_tick ? rzyOS_wqueue -> start_delay_tick : rzyOS_wqueue -> period_tick;

			rzyOS_wqueue -> rzyOS_wqueue_status = wqueue_start;

			//判断是否是高速工作队列
			if (HIGH_WORK_QUEUE == rzyOS_wqueue -> wqueue_config)
			{
				uint32_t status = task_enter_critical();

				//在高速工作队列链表中插入节点
				list_add_first(&rzyOS_high_wqueue_list, &(rzyOS_wqueue -> node));

				task_exit_critical(status);
			}
			//低速工作队列
			else
			{
				rzyOS_sem_wait(&rzyOS_wqueue_protect_sem, 0);

				//在低速工作队列链表中插入节点
				list_add_first(&rzyOS_low_wqueue_list, &(rzyOS_wqueue -> node));

				rzyOS_sem_post(&rzyOS_wqueue_protect_sem);
			}

			break ;
		}

		default :
		{
			break ;
		}
	}
}

//停止指定工作队列节点(工作队列组件的外部任务调用)
void rzyOS_wqueue_stop(rzyOS_wqueue_s *rzyOS_wqueue)
{
	//判断当前状态
	switch (rzyOS_wqueue -> rzyOS_wqueue_status)
	{
		//只有已经启动和运行的工作队列才能被停止
		case wqueue_start:
		case wqueue_running:
		{
			//判断是否是高速工作队列
			if (HIGH_WORK_QUEUE == rzyOS_wqueue -> wqueue_config)
			{
				uint32_t status = task_enter_critical();

				//在高速工作队列链表中移除节点
				list_remove_pos_node(&rzyOS_high_wqueue_list, &(rzyOS_wqueue -> node));

				task_exit_critical(status);
			}
			//低速工作队列
			else
			{
				rzyOS_sem_wait(&rzyOS_wqueue_protect_sem, 0);

				//在低速工作队列链表中移除节点
				list_remove_pos_node(&rzyOS_low_wqueue_list, &(rzyOS_wqueue -> node));

				rzyOS_sem_post(&rzyOS_wqueue_protect_sem);
			}

			break ;
		}

		default :
		{
			break ;
		}
	}
}

//销毁工作队列节点
void rzyOS_wqueue_destroy(rzyOS_wqueue_s *rzyOS_wqueue)
{
	rzyOS_wqueue_stop(rzyOS_wqueue);
	rzyOS_wqueue -> rzyOS_wqueue_status = wqueue_destroy;
}

//工作队列节点信息获取函数
void rzyOS_wqueue_get_info(rzyOS_wqueue_s *rzyOS_wqueue, rzyOS_wqueue_info_s *rzyOS_wqueue_info)
{
	uint32_t status = task_enter_critical();

	rzyOS_wqueue_info -> start_delay_tick = rzyOS_wqueue -> start_delay_tick;
	rzyOS_wqueue_info -> period_tick = rzyOS_wqueue -> period_tick;
	rzyOS_wqueue_info -> worker = rzyOS_wqueue -> worker;
	rzyOS_wqueue_info -> arg = rzyOS_wqueue -> arg;
	rzyOS_wqueue_info -> wqueue_config = rzyOS_wqueue -> wqueue_config;
	rzyOS_wqueue_info -> rzyOS_wqueue_status = rzyOS_wqueue -> rzyOS_wqueue_status;

	task_exit_critical(status);
}

//工作队列处理函数
static void rzyOS_wqueue_call(list_t *list)
{
	node_t *node;

	//便利链表中的节点
	for (node = list -> head_node.next_node; node != &(list -> head_node); node = node -> next_node)
	{
		//获取连接节点所处于的工作队列结构
		rzyOS_wqueue_s *rzyOS_wqueue = node_parent(node, rzyOS_wqueue_s, node);

		//tick递减
		rzyOS_wqueue -> count_tick --;

		//当tick为0
		if (0 == rzyOS_wqueue -> count_tick)
		{
			rzyOS_wqueue -> rzyOS_wqueue_status = wqueue_running;
			//运行回调函数
			rzyOS_wqueue -> worker(rzyOS_wqueue -> arg);
			rzyOS_wqueue -> rzyOS_wqueue_status = wqueue_start;

			//如果period_tick大于0
			//则对计数count_tick重新赋值
			if (rzyOS_wqueue -> period_tick > 0)
			{
				rzyOS_wqueue -> count_tick = rzyOS_wqueue -> period_tick;
			}
			//如果period_tick等于0
			//则认为只执行一次，
			else
			{
				//把工作队列节点在链表中删除
				list_remove_pos_node(list, node);

				rzyOS_wqueue -> rzyOS_wqueue_status = wqueue_stop;
			}
		}
	}
}

//工作队列任务控制块
static task_tcb_s rzyOS_wqueue_task_tcb;
//工作队列任务堆栈
static tTaskStack rzyOS_wqueue_task_stack[RZYOS_WQUEUE_STACK_SIZE];

//工作队列任务
static void rzyOS_wqueue_task(void *param)
{
	while(1)
	{
		//等待系统tick节拍信号量
		rzyOS_sem_wait(&rzyOS_wqueue_tick_sem, 0);

		//等待链表保护操作信号量
		rzyOS_sem_wait(&rzyOS_wqueue_protect_sem, 0);

		//处理低速工作队列
		rzyOS_wqueue_call(&rzyOS_low_wqueue_list);

		//释放链表保护操作信号量
		rzyOS_sem_post(&rzyOS_wqueue_protect_sem);
	}
}

//系统节拍， tick周期性工作队列处理函数
void rzyOS_wqueue_tick_handle(void)
{
	uint32_t status = task_enter_critical();

	//处理高速工作队列
	rzyOS_wqueue_call(&rzyOS_high_wqueue_list);

	task_exit_critical(status);

	//post系统节拍信号量(处理低速工作队列)
	rzyOS_sem_post(&rzyOS_wqueue_tick_sem);
}

//工作队列模块组件初始化
void rzyOS_wqueue_module_init(void)
{
	//工作队列管理链表初始化
	list_init(&rzyOS_high_wqueue_list);
	list_init(&rzyOS_low_wqueue_list);

	//初始值为0, 最大值1
	rzyOS_sem_init(&rzyOS_wqueue_protect_sem, 1, 1);
	//初始值为0, 最大值无限制
	rzyOS_sem_init(&rzyOS_wqueue_tick_sem, 0, 0);
}

//工作队列任务初始化
void rzyOS_wqueue_task_init(void)
{
//预编译判断
//工作队列优先级必须高于空闲任务
//我在内核中设定工作队列优先级为最高
#if (RZYOS_WQUEUE_PRIO) >= (RZYOS_IDLETASK_PRIO)
	#error "work queue task priority must be higher then idle task"
#endif

	//初始化工作队列任务
	task_init(&rzyOS_wqueue_task_tcb, rzyOS_wqueue_task, (void *)0, RZYOS_WQUEUE_PRIO, rzyOS_wqueue_task_stack, RZYOS_WQUEUE_STACK_SIZE);
}

#endif
