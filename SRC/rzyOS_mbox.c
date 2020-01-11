#include "rzyOS_mbox.h"

//邮箱初始化
//parameter
//rzyOS_mbox_s *rzyOS_mbox : 要使用的邮箱结构
//void **msg_buffer ：邮箱buffer地址
//uint32_t max_count ：邮箱最大空间
void rzyOS_mbox_init(rzyOS_mbox_s *rzyOS_mbox, void **msg_buffer, uint32_t max_count)
{
	rzyOS_event_init(&(rzyOS_mbox -> rzyOS_ecb), event_type_mbox);
	
	rzyOS_mbox -> msg_buffer = msg_buffer;
	rzyOS_mbox -> max_count = max_count;
	rzyOS_mbox -> count = 0;
	rzyOS_mbox -> read = 0;
	rzyOS_mbox -> write = 0;
}

uint32_t rzyOS_mbox_wait(rzyOS_mbox_s *rzyOS_mbox, void **msg, uint32_t wait_tick)
{
	uint32_t status = task_enter_critical();
	
	if (rzyOS_mbox -> count > 0)
	{
		rzyOS_mbox -> count --;
		*msg = rzyOS_mbox -> msg_buffer[rzyOS_mbox -> read];
		rzyOS_mbox -> read ++;
		if (rzyOS_mbox -> read >= rzyOS_mbox -> max_count)
		{
			rzyOS_mbox -> read = 0;
		}
		task_exit_critical(status);
		return error_no_error;
	}
	else
	{
		rzyOS_event_wait(&(rzyOS_mbox -> rzyOS_ecb), currentTask, (void *)0, event_type_mbox, wait_tick);
		task_exit_critical(status);
		
		task_schedule();
		
		*msg = currentTask -> event_msg;
		
		return currentTask -> wait_event_result;
	}
}

uint32_t rzyOS_mbox_nowait(rzyOS_mbox_s *rzyOS_mbox, void **msg)
{
	uint32_t status = task_enter_critical();
	
	if (rzyOS_mbox -> count > 0)
	{
		rzyOS_mbox -> count --;
		*msg = rzyOS_mbox -> msg_buffer[rzyOS_mbox -> read];
		rzyOS_mbox -> read ++;
		if (rzyOS_mbox -> read >= rzyOS_mbox -> max_count)
		{
			rzyOS_mbox -> read = 0;
		}
		task_exit_critical(status);
		return error_no_error;
	}
	else
	{
		task_exit_critical(status);
		
		return error_resource_unvaliable;
	}
}

uint32_t rzyOS_mbox_post(rzyOS_mbox_s *rzyOS_mbox, void *msg, uint32_t notify_option)
{
	uint32_t status = task_enter_critical();
	
	if (rzyOS_mbox -> count > 0)
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
