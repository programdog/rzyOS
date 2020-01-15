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

//消息释放函数
uint32_t rzyOS_mbox_post(rzyOS_mbox_s *rzyOS_mbox, void *msg, uint32_t notify_option)
{
	uint32_t status = task_enter_critical();
	
	//统计是否有等待的任务， 如果有任务等待
	if (rzyOS_event_wait_count(&(rzyOS_mbox -> rzyOS_ecb)) > 0)
	{
		task_tcb_s *task_tcb = rzyOS_event_wakeup(&(rzyOS_mbox -> rzyOS_ecb), (void *)msg, error_no_error);
		if (task_tcb -> prio < currentTask -> prio)
		{
			task_schedule();
		}
	}
	else
	{
		if (rzyOS_mbox -> count >= rzyOS_mbox -> max_count)
		{
			task_exit_critical(status);
			return error_resource_full;
		}
		else
		{
			if (notify_option & rzyOS_mbox_send_front)
			{
				if (rzyOS_mbox -> read <= 0)
				{
					rzyOS_mbox -> read = rzyOS_mbox -> max_count -1;
				}
				else
				{
					rzyOS_mbox -> read --;
				}
				
				rzyOS_mbox -> msg_buffer[rzyOS_mbox -> read] = msg;
			}
			else
			{
				rzyOS_mbox -> msg_buffer[rzyOS_mbox -> write] = msg;
				rzyOS_mbox -> write ++;
				if (rzyOS_mbox -> write >= rzyOS_mbox -> max_count)
				{
					rzyOS_mbox -> write = 0;
				}
			}
		}
		
		rzyOS_mbox -> count ++;
	}
	
	task_exit_critical(status);
	
	return error_no_error;
}
