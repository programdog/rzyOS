#include "rzyOS_mbox.h"

//邮箱初始化
//parameter
//rzyOS_mbox_s *rzyOS_mbox : 要使用的邮箱结构
//void **msg_buffer ：邮箱buffer地址
//uint32_t max_count ：邮箱最大空间
void rzyOS_mbox_init(rzyOS_mbox_s *rzyOS_mbox, void **msg_buffer, uint32_t max_count)
{
	//事件结构初始化
	rzyOS_event_init(&(rzyOS_mbox -> rzyOS_ecb), event_type_mbox);
	
	rzyOS_mbox -> msg_buffer = msg_buffer;
	rzyOS_mbox -> max_count = max_count;
	rzyOS_mbox -> count = 0;
	rzyOS_mbox -> read = 0;
	rzyOS_mbox -> write = 0;
}

//消息等待函数（阻塞模式）
//parameter
//rzyOS_mbox_s *rzyOS_mbox : 邮箱结构
//void **msg ： 消息指针
//uint32_t wait_tick ： 超时等待
uint32_t rzyOS_mbox_wait(rzyOS_mbox_s *rzyOS_mbox, void **msg, uint32_t wait_tick)
{
	uint32_t status = task_enter_critical();
	
	//缓冲区中是否有消息
	if (rzyOS_mbox -> count > 0)
	{
		//如果 > 0 ， 取出消息
		rzyOS_mbox -> count --;
		//取出消息指针
		*msg = rzyOS_mbox -> msg_buffer[rzyOS_mbox -> read];
		//读索引向后移动
		rzyOS_mbox -> read ++;

		//读索引已经到末尾了， 需要回头部
		if (rzyOS_mbox -> read >= rzyOS_mbox -> max_count)
		{
			rzyOS_mbox -> read = 0;
		}
		task_exit_critical(status);
		return error_no_error;
	}
	//缓冲区中无消息
	else
	{
		//把任务插入到任务等待队列
		rzyOS_event_wait(&(rzyOS_mbox -> rzyOS_ecb), currentTask, (void *)0, event_type_mbox, wait_tick);
		task_exit_critical(status);
		
		//调度切换
		task_schedule();
		
		//被切换回来后，从event_msg中获取消息
		*msg = currentTask -> event_msg;
		
		return currentTask -> wait_event_result;
	}
}

//消息等待函数（非阻塞模式）
uint32_t rzyOS_mbox_nowait(rzyOS_mbox_s *rzyOS_mbox, void **msg)
{
	uint32_t status = task_enter_critical();
	
	//缓冲区中是否有消息
	if (rzyOS_mbox -> count > 0)
	{
		//如果 > 0 ， 取出消息
		rzyOS_mbox -> count --;
		//取出消息指针
		*msg = rzyOS_mbox -> msg_buffer[rzyOS_mbox -> read];
		//读索引向后移动
		rzyOS_mbox -> read ++;

		//读索引已经到末尾了， 需要回头部
		if (rzyOS_mbox -> read >= rzyOS_mbox -> max_count)
		{
			rzyOS_mbox -> read = 0;
		}

		task_exit_critical(status);

		return error_no_error;
	}
	//缓冲区中无消息， 不进行阻塞等待，继续运行
	else
	{
		task_exit_critical(status);
		
		//返回无资源
		return error_resource_unvaliable;
	}
}

//post一个消息
uint32_t rzyOS_mbox_post(rzyOS_mbox_s *rzyOS_mbox, void *msg, uint32_t notify_option)
{
	uint32_t status = task_enter_critical();
	
	//统计是否有等待的任务， 如果有任务等待
	if (rzyOS_event_wait_count(&(rzyOS_mbox -> rzyOS_ecb)) > 0)
	{
		//唤醒事件控制块中第一个task，并传递消息指针
		task_tcb_s *task_tcb = rzyOS_event_wakeup(&(rzyOS_mbox -> rzyOS_ecb), (void *)msg, error_no_error);
		//判断优先级，是否需要调度
		if (task_tcb -> prio < currentTask -> prio)
		{
			task_schedule();
		}
	}
	//没有等待任务，进行消息插入
	else
	{
		//消息已经超出了最大消息计数值
		if (rzyOS_mbox -> count >= rzyOS_mbox -> max_count)
		{
			task_exit_critical(status);

			return error_resource_full;
		}
		//消息未超出最大消息计数值
		else
		{
			//消息插入模式为向前插入，并不做数据覆盖，而是向前写入
			if (notify_option & rzyOS_mbox_send_front)
			{
				//如果读索引在头部
				if (rzyOS_mbox -> read <= 0)
				{
					//让读索引指向尾部
					rzyOS_mbox -> read = rzyOS_mbox -> max_count -1;
				}
				//如果读索引不在头部
				else
				{
					//读索引向前移动
					rzyOS_mbox -> read --;
				}
				
				//插入消息
				rzyOS_mbox -> msg_buffer[rzyOS_mbox -> read] = msg;
			}
			//消息向后插入
			else
			{
				//插入消息
				rzyOS_mbox -> msg_buffer[rzyOS_mbox -> write] = msg;
				rzyOS_mbox -> write ++;

				//写索引已经到了最大值，则写索引回到头部
				if (rzyOS_mbox -> write >= rzyOS_mbox -> max_count)
				{
					rzyOS_mbox -> write = 0;
				}
			}
		}
		
		//消息计数+1
		rzyOS_mbox -> count ++;
	}
	
	task_exit_critical(status);
	
	return error_no_error;
}

//邮箱的清空
void rzyOS_mbox_flush(rzyOS_mbox_s *rzyOS_mbox)
{
	uint32_t status = task_enter_critical();

	//若等待事件的任务为0, 则说明处于有消息且无任务等待的状态， 需要清理
	if (rzyOS_event_wait_count(&(rzyOS_mbox -> rzyOS_ecb)) == 0)
	{
		rzyOS_mbox = count;
		rzyOS_mbox = read;
		rzyOS_mbox = write;
	}

	task_exit_critical(status);
}

//邮箱的删除
//在rzyOS_event_remove_all()函数中会把任务插入到就绪队列
uint32_t rzyOS_mbox_destory(rzyOS_mbox_s *rzyOS_mbox)
{
	uint32_t status = task_enter_critical();

	//移除等待列表中的任务， 并返回个数
	uint32_t count = rzyOS_event_remove_all(&(rzyOS_mbox -> rzyOS_ecb), (void *)0, error_delete);

	task_exit_critical(status);

	//若有任务在等待，则调用一次切换
	if (count > 0)
	{
		task_schedule();
	}

	return count;
}

//邮箱状态查询
void rzyOS_mbox_get_info(rzyOS_mbox_s *rzyOS_mbox, rzyOS_mbox_info_s *rzyOS_mbox_info)
{
	uint32_t status = task_enter_critical();

	rzyOS_mbox_info -> count = rzyOS_mbox -> count;
	rzyOS_mbox_info -> max_count = rzyOS_mbox -> max_count;
	rzyOS_mbox_info -> task_count = rzyOS_mbox -> rzyOS_event_wait_count(&(rzyOS_mbox -> rzyOS_ecb));

	task_exit_critical(status);
}
