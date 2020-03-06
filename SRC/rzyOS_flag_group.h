#ifndef __RZYOS_FLAG_GROUP_H
#define __RZYOS_FLAG_GROUP_H

#include "rzyOS_event.h"

typedef struct rzyOS_flag_group_s
{
	rzyOS_ecb_s rzyOS_ecb;
	uint32_t flag;
} rzyOS_flag_group_s;

//标志位形式
//清除标志位
#define FLAGGROUP_CLEAR (0x0 << 0)
//设置标志位
#define FLAGGROUP_SET (0x1 << 0)
//任意的标志位
#define FLAGGROUP_ANY (0x0 << 1)
//所有的标志位
#define FLAGGROUP_ALL (0x1 << 1)

//标志组合
//设置所有的标志位
#define FLAGGROUP_SET_ALL (FLAGGROUP_SET | FLAGGROUP_ALL)
//设置任意的标志位
#define FLAGGROUP_SET_ANY (FLAGGROUP_SET | FLAGGROUP_ANY)
//清除所有的标志位
#define FLAGGROUP_CLEAR_ALL (FLAGGROUP_CLEAR | FLAGGROUP_ALL)
//清除任意的标志位
#define FLAGGROUP_CLEAR_ANY (FLAGGROUP_CLEAR | FLAGGROUP_ANY)

//是否需要清零或置位
#define FLAGGROUP_CONSUME (1 << 7)


//时间组初始化函数
void rzyOS_flag_group_init(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t flags);

void rzyOS_flag_group_wait(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t wait_type, uint32_t request_flag, uint32_t *result_flag, uint32_t wait_ticks);

#endif
