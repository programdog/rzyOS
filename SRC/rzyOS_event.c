#include "rzyOS_event.h"
#include "rzyOS.h"

void rzyOS_event_init(rzyOS_ecb_s *ecb, rzyOS_event_type_e type)
{
	ecb -> type = event_type_unknow;
	list_init(&(ecb -> wait_list));
}

//事件等待函数
//把任务插入到事件的等待队列
void rzyOS_event_wait(rzyOS_ecb_s *rzyOS_ecb, task_tcb_s *task_tcb, void *msg, uint32_t state, uint32_t timeout)
{
	uint32_t status = task_enter_critical();
	
	task_tcb -> task_status |= state;
	task_tcb -> wait_event = rzyOS_ecb;
	task_tcb -> event_msg = msg;
	
	task_tcb -> wait_event_result = event_type_unknow;
	
	//从就绪队列中移出
	task_remove_ready_list(task_tcb);
	//插入事件控制块的等待队列的尾部,因为先获得事件的任务先执行,所以插入到尾部
	list_add_last(&(rzyOS_ecb -> wait_list), &(task_tcb -> link_node));
	
	if (timeout)
	{
		//如果有等待超时, 则加入到延时等待队列
		delay_list_insert_time_node(task_tcb, timeout);
	}
	
	task_exit_critical(status);
}

//通知相关事件控制块
//唤醒相关事件列表的第一个任务
task_tcb_s *rzyOS_event_wakeup(rzyOS_ecb_s *rzyOS_ecb, void *msg, uint32_t result)
{
	node_t *node;
	task_tcb_s *task;
	uint32_t status = task_enter_critical();
	
	//把事件等待队列的第一个任务从列表删除
	node = remove_list_first(&(rzyOS_ecb -> wait_list));
	
	if (node != (node_t *)0)
	{
		task = (task_tcb_s *)node_parent(node, task_tcb_s, link_node);
		task -> wait_event = (rzyOS_ecb_s *)0;
		task -> event_msg = msg;
		task -> wait_event_result = result;
		task -> task_status &= ~RZYOS_TASK_WAIT_MASK;
		
		if (task -> delayTicks != 0)
		{
			//如果有延时, 则强制从延时队列中唤醒, 以便事件到来, 及时相应
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

	//该事件中等待队列中的任务数
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

//事件等待队列中,任务的数量
uint32_t rzyOS_event_wait_count(rzyOS_ecb_s *rzyOS_ecb)
{
	uint32_t count = 0;
	
	uint32_t status = task_enter_critical();
	
	count = list_count(&(rzyOS_ecb -> wait_list));
	
	task_exit_critical(status);
	
	return count;
}
