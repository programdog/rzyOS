#include "rzyOS_flag_group.h"

//时间组初始化函数
//parameter : 
//rzyOS_flag_group_s *rzyOS_flag_group : 事件标志组
//uint32_t flags : 标志组类型
void rzyOS_flag_group_init(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t flags)
{
	rzyOS_event_init(&(rzyOS_flag_group -> rzyOS_ecb), event_type_flag_group);
	rzyOS_flag_group -> flag = flags;
}

//事件检查函数
//parameter : 
//uint32_t type : 等待的事件类型
//uint32_t flag : 任务期望等待的事件类型标志
static uint32_t rzyOS_flag_group_check(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t type, uint32_t *flag)
{
	uint32_t src_flag = *flag;
	//判断是设置还是清零
	uint32_t is_set = type & FLAGGROUP_SET;
	uint32_t is_all = type & FLAGGROUP_ALL;
	uint32_t is_consume = type & FLAGGROUP_CONSUME;

	uint32_t cal_flag = is_set ? (rzyOS_flag_group -> flag & src_flag) : (~rzyOS_flag_group -> flag & src_flag);

	if (((is_all != 0) && (cal_flag == src_flag)) || ((is_all == 0) && (cal_flag != 0)))
	{
		if (is_consume)
		{
			if (is_set)
			{
				rzyOS_flag_group -> flag &= ~src_flag;
			}
			else
			{
				rzyOS_flag_group -> flag |= src_flag;
			}
		}

		*flag = cal_flag;

		return error_no_error
	}

	*flag = cal_flag;
	return error_resource_unvaliable;
}

void rzyOS_flag_group_wait(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t wait_type, uint32_t request_flag, uint32_t *result_flag, uint32_t wait_ticks)
{

}