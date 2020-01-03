#include "rzyOS_event.h"
#include "rzyOS.h"

//事件初始化
//parameter
//rzyOS_ecb_s *ecb ： 待初始化的任务控制快
//rzyOS_event_type_e type ： 初始化的事件类型
void rzyOS_event_init(rzyOS_ecb_s *ecb, rzyOS_event_type_e type)
{
	//事件类型
	ecb -> type = type;
	//初始化事件等待列表
	list_init(&(ecb -> wait_list));
}

//事件等待函数（作为基础组件， 不做调度处理， 在信号量等待函数中再做调度）
//把任务插入到事件的等待队列
//parameter
//rzyOS_ecb_s *rzyOS_ecb ： 期望要加入的事件控制块
//task_tcb_s *task_tcb ： 要插入的任务
//void *msg ： 消息地址
//uint32_t state ： 任务状态
//uint32_t timeout ： 超时等待
void rzyOS_event_wait(rzyOS_ecb_s *rzyOS_ecb, task_tcb_s *task_tcb, void *msg, uint32_t state, uint32_t timeout)
{
	uint32_t status = task_enter_critical();
	
	task_tcb -> task_status |= state;
	//设置任务等待的事件控制块
	task_tcb -> wait_event = rzyOS_ecb;
	task_tcb -> event_msg = msg;
	
	//复位事件等待结果
	task_tcb -> wait_event_result = error_no_error;
	
	//从就绪队列中移出
	task_remove_ready_list(task_tcb);
	//插入事件控制块的等待队列的尾部, 因为先获得事件的任务先执行, 所以插入到尾部
	list_add_last(&(rzyOS_ecb -> wait_list), &(task_tcb -> link_node));
	
	//如果有超时等待， 则该任务即在时间等待队列也在延时队列
	//若超时等待生效， 则在延时处理机制中把任务从事件等待队列移除
	if (timeout)
	{
		//如果有等待超时, 则加入到延时等待队列
		delay_list_insert_time_node(task_tcb, timeout);
	}
	
	task_exit_critical(status);
}

//唤醒通知相关事件控制块
//唤醒相关事件列表的第一个任务
task_tcb_s *rzyOS_event_wakeup(rzyOS_ecb_s *rzyOS_ecb, void *msg, uint32_t result)
{
	node_t *node;
	task_tcb_s *task;

	uint32_t status = task_enter_critical();
	
	//把事件等待队列的第一个任务从列表删除， 并返回事件等待队列节点指针
	node = remove_list_first(&(rzyOS_ecb -> wait_list));
	
	if (node != (node_t *)0)
	{
		//设置相应的TCB值
		task = (task_tcb_s *)node_parent(node, task_tcb_s, link_node);
		task -> wait_event = (rzyOS_ecb_s *)0;
		task -> event_msg = msg;
		task -> wait_event_result = result;
		task -> task_status &= ~RZYOS_TASK_WAIT_MASK;
		
		//若设置了超时等待
		if (task -> delayTicks != 0)
		{
			//则强制从延时队列中唤醒, 以便事件到来, 及时相应
			rzyOS_task_delay_list_remove(task);
		}
		
		//把任务插入到就绪队列, 等待运行
		task_insert_ready_list(task);
	}
	
	task_exit_critical(status);
	
	return task;
}

//把指定的任务从事件控制块强制移出
void rzyOS_event_remove(task_tcb_s *task_tcb, void *msg, uint32_t result)
{
	uint32_t status = task_enter_critical();

	//在事件等待列表中移除指定的任务
	list_remove_pos_node(&(task_tcb -> wait_event -> wait_list), &(task_tcb -> link_node));
	//设置成0 , 不再等待事件
	task_tcb -> wait_event = (rzyOS_ecb_s *)0;
	task_tcb -> event_msg = msg;
	task_tcb -> wait_event_result = result;
	task_tcb -> task_status &= ~RZYOS_TASK_WAIT_MASK;

	task_exit_critical(status);

	//此函数将在systemtick中调用, 将在systick中断中唤醒, 所以不做任务强制唤醒处理
}

//移出事件队列中所有的正在等待事件的任务
//返回移除的个数
uint32_t rzyOS_event_remove_all(rzyOS_ecb_s *rzyOS_ecb, void *msg, uint32_t result)
{
	uint32_t element_count = 0;
	task_tcb_s *task;
	node_t *node;

	uint32_t status = task_enter_critical();

	//统计事件中等待队列中的任务数
	element_count = list_count(&(rzyOS_ecb -> wait_list));

	while ((node = remove_list_first(&(rzyOS_ecb -> wait_list))) != (node_t *)0)
	{
		task = (task_tcb_s *)node_parent(node, task_tcb_s, link_node);
		//设置成0 , 不再等待事件
		task -> wait_event = (rzyOS_ecb_s *)0;
		task -> event_msg = msg;
		task -> wait_event_result = result;
		task -> task_status &= ~RZYOS_TASK_WAIT_MASK;

		if (task -> delayTicks != 0)
		{
			//如果有延时, 则强制从延时队列中唤醒, 以便事件到来, 及时相应
			rzyOS_task_delay_list_remove(task);
		}

		task_insert_ready_list(task);
	}

	task_exit_critical(status);

	//返回移除的个数
	return element_count;
}

//统计事件等待队列中任务的数量
uint32_t rzyOS_event_wait_count(rzyOS_ecb_s *rzyOS_ecb)
{
	uint32_t count = 0;
	
	uint32_t status = task_enter_critical();
	
	//统计事件等待队列中任务个数
	count = list_count(&(rzyOS_ecb -> wait_list));
	
	task_exit_critical(status);
	
	return count;
}
