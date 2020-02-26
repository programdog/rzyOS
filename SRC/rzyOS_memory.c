#include "rzyOS_memory.h"

//内存管理初始化函数
//parameter : 
//rzyOS_mem_block_s *rzyOS_mem_block ： 内存管理结构
//uint8_t *mem_start ： 内存块起始地址
//uint32_t bolck_size ： 子内存块容量大小
//uint32_t block_count ： 子内存块个数
void rzyOS_mem_block_init(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t *mem_start, uint32_t bolck_size, uint32_t block_count)
{
	//记录起始地址与结束地址
	uint8_t *mem_block_start = (uint8_t *)mem_start;
	uint8_t *mem_block_end = mem_block_start + bolck_size * block_count;

	//因为每个子内存块要在子内存块的初始位置建立链接节点， 所以子内存块的最小size必须大于node_t
	if (bolck_size < sizeof(node_t))
	{
		return ;
	}

	//事件结构初始化
	rzyOS_event_init(&(rzyOS_mem_block -> rzyOS_ecb), event_type_mem_block);

	//内存管理结构赋值
	rzyOS_mem_block -> mem_start = mem_block_start;
	rzyOS_mem_block -> bolck_size = bolck_size;
	rzyOS_mem_block -> max_count = block_count;

	//初始化内存管理结构链表
	list_init(&(rzyOS_mem_block -> block_list));

	//遍历初始化子内存块节点，并加入链表
	while (mem_block_start < mem_block_end)
	{
		//初始化子内存块节点
		node_init((node_t *)mem_block_start);
		//把子内存块加入到子内存块管理链表
		list_add_last(&(rzyOS_mem_block -> block_list), (node_t *)mem_block_start);
		//遍历地址向后偏移
		mem_block_start += bolck_size;
	}
}

//任务获取内存块函数(阻塞)
//parameter : 
//rzyOS_mem_block_s *rzyOS_mem_block : 内存管理结构
//uint8_t **mem : 获取到子内存块地址指针
//uint32_t wait_time : 超时等待
uint32_t rzyOS_mem_block_wait(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t **mem, uint32_t wait_time)
{
	uint32_t status = task_enter_critical();

	//统计是否有空闲的内存子块
	if (list_count(&(rzyOS_mem_block -> block_list)) > 0)
	{
		//如果有空闲子内存块, 则从内存块空闲链表中移除
		*mem = (uint8_t *)remove_list_first(&(rzyOS_mem_block -> block_list));
		task_exit_critical(status);

		return error_no_error;
	}
	else
	{
		//如果没有空闲子内存块, 则加入内存块等待队列
		rzyOS_event_wait(&(rzyOS_mem_block -> rzyOS_ecb), currentTask, (void *)0, event_type_mem_block, wait_time);
		task_exit_critical(status);

		//进行一次调度
		task_schedule();
		//获取子内存块地址指针
		*mem = currentTask -> event_msg;

		return currentTask -> wait_event_result;
	}
}

//任务获取内存块函数(非阻塞)
//parameter : 
//rzyOS_mem_block_s *rzyOS_mem_block : 内存管理结构
//uint8_t **mem : 获取到子内存块地址指针
uint32_t rzyOS_mem_block_no_wait(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t **mem)
{
	uint32_t status = task_enter_critical();

	//统计是否有空闲的内存子块
	if (list_count(&(rzyOS_mem_block -> block_list)) > 0)
	{
		*mem = (uint8_t *)remove_list_first(&(rzyOS_mem_block -> block_list));
		task_exit_critical(status);

		return error_no_error;
	}
	else
	{
		task_exit_critical(status);

		return error_resource_unvaliable;
	}
}

//子内存块释放函数
//parameter : 
//rzyOS_mem_block_s *rzyOS_mem_block : 内存管理结构
//uint8_t **mem : 释放的子内存块地址指针
void rzyOS_mem_block_post(rzyOS_mem_block_s *rzyOS_mem_block, uint8_t **mem)
{
	uint32_t status = task_enter_critical();

	//统计是否有任务等待内存子块
	if (rzyOS_event_wait_count(&(rzyOS_mem_block -> rzyOS_ecb)) > 0)
	{
		//如果有任务等待,则唤醒,并传递内存子块地址指针
		task_tcb_s *task_tcb = rzyOS_event_wakeup(&(rzyOS_mem_block -> rzyOS_ecb), (void *)mem, error_no_error);

		//判断被唤醒的任务优先级是否高于当前任务
		if (task_tcb -> prio < currentTask -> prio)
		{
			task_schedule();
		}
	}
	else
	{
		//如果没有任务等待,则加入内存块空闲链表中
		list_add_last(&(rzyOS_mem_block -> block_list), (node_t *)mem);
	}

	task_exit_critical(status);
}

//内存块销毁函数
uint32_t rzyOS_mem_block_destroy(rzyOS_mem_block_s *rzyOS_mem_block)
{
	uint32_t status = task_enter_critical();

	//删除等待空闲块的任务, 并在rzyOS_event_remove_all()把任务插入到就绪队列
	uint32_t count = rzyOS_event_remove_all(&(rzyOS_mem_block -> rzyOS_ecb), (void *)0, error_delete);

	task_exit_critical(status);

	//若获取移除的个数大于0, 则进行一次调度， 看看是否有更高优先级的任务处于刚才的等待状态
	if (count > 0)
	{
		task_schedule();
	}

	return count;
}

//内存块信息获取函数
void rzyOS_mem_block_get_info(rzyOS_mem_block_s *rzyOS_mem_block, rzyOS_mem_info_s *rzyOS_mem_info)
{
	uint32_t status = task_enter_critical();

	rzyOS_mem_info -> mem_count = list_count(&(rzyOS_mem_block -> block_list));
	rzyOS_mem_info -> max_count = rzyOS_mem_info -> max_count;
	rzyOS_mem_info -> block_size = rzyOS_mem_info -> block_size;
	rzyOS_mem_info -> task_count = rzyOS_event_wait_count(&(rzyOS_mem_block -> rzyOS_ecb));

	task_exit_critical(status);
}
