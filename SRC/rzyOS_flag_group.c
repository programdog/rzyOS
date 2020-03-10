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
//uint32_t type : 等待的事件类型(标志组合)
//uint32_t flag : 任务期望等待的事件类型标志
static uint32_t rzyOS_flag_group_check(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t type, uint32_t *flag)
{
	//任务希望等待的标志
	uint32_t src_flag = *flag;
	//判断是设置还是清零
	uint32_t is_set = type & FLAGGROUP_SET;
	//判断是不是等待所有的标志
	uint32_t is_all = type & FLAGGROUP_ALL;
	//判断满足条件后是否需要把标志消耗掉
	uint32_t is_consume = type & FLAGGROUP_CONSUME;

	//对当前发生的标志位(设置或清零)进行计算
	//is_set ? : 对置位进行统计
	//(rzyOS_flag_group -> flag & src_flag) : 当前事件组与任务期望等待的标志类型所匹配的置位的位
	//(~rzyOS_flag_group -> flag & src_flag) : 当前事件组与任务期望等待的标志类型所匹配的为零的位
	uint32_t cal_flag = is_set ? (rzyOS_flag_group -> flag & src_flag) : (~rzyOS_flag_group -> flag & src_flag);

	//判断相应的标志是否已经满足
	//所有标志 && 计算的与请求的一样 || 任意标志 && 计算标志有统计到的
	if (((is_all != 0) && (cal_flag == src_flag)) || ((is_all == 0) && (cal_flag != 0)))
	{
		//是否需要消耗掉标志
		if (is_consume)
		{
			//如果是置位 ,则清零
			if (is_set)
			{
				rzyOS_flag_group -> flag &= ~src_flag;
			}
			//如果是清零, 则置位
			else
			{
				rzyOS_flag_group -> flag |= src_flag;
			}
		}

		//传递计算的标志值
		*flag = cal_flag;

		return error_no_error
	}

	//不满足
	//无资源
	*flag = cal_flag;
	return error_resource_unvaliable;
}

void rzyOS_flag_group_wait(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t wait_type, uint32_t request_flag, uint32_t *result_flag, uint32_t wait_ticks)
{

}