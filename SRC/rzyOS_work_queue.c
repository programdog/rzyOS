#include "rzyOS_work_queue.h"

static list_t rzyOS_high_wqueue_list;
static list_t rzyOS_low_wqueue_list;

static rzyOS_sem_s rzyOS_wqueue_protect_sem;
static rzyOS_sem_s rzyOS_wqueue_tick_sem;

//工作队列初始化函数
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
void rzyOS_queue_init(rzyOS_wqueue_s *rzyOS_wqueue, uint32_t start_delay_tick, uint32_t period_tick, worker_t worker, void *arg, uint32_t wqueue_config)
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

static task_tcb_s rzyOS_wqueue_task_tcb;
static tTaskStack rzyOS_wqueue_task_stack[RZYOS_WQUEUE_STACK_SIZE];

static void rzyOS_wqueue_task(void *param)
{
	while(1)
	{
		rzyOS_sem_wait(&rzyOS_wqueue_tick_sem, 0);

		rzyOS_sem_wait(&rzyOS_wqueue_protect_sem, 0);


		rzyOS_sem_post(&rzyOS_wqueue_protect_sem);
	}
}

void rzyOS_wqueue_module_init(void)
{
	list_init(&rzyOS_high_wqueue_list);
	list_init(&rzyOS_low_wqueue_list);

	//初始值为0, 最大值1
	rzyOS_sem_init(&rzyOS_wqueue_protect_sem, 1, 1);
	//初始值为0, 最大值无限制
	rzyOS_sem_init(&rzyOS_wqueue_tick_sem, 0, 0);

#if (RZYOS_WQUEUE_PRIO) >= (RZYOS_IDLETASK_PRIO)
	#error "work queue task priority must be higher then idle task"
#endif
	task_init(&rzyOS_wqueue_task_tcb, rzyOS_wqueue_task, (void *)0, RZYOS_WQUEUE_PRIO, &rzyOS_wqueue_task_stack[RZYOS_WQUEUE_STACK_SIZE]);
}

