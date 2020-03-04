#ifndef __RZYOS_FLAG_GROUP_H
#define __RZYOS_FLAG_GROUP_H

#include "rzyOS_event.h"

typedef struct rzyOS_flag_group_s
{
	rzyOS_ecb_s rzyOS_ecb;
	uint32_t flag;
} rzyOS_flag_group_s;

//任务希望请求的事件标志组时间类型
//清除
#define FLAGGROUP_CLEAR (0x0 << 0)
//设置
#define FLAGGROUP_SET (0x1 << 0)
//等待所有的标志位设置后的清零
#define FLAGGROUP_ANY (0x0 << 1)
//所有的标志位设置后的清零
#define FLAGGROUP_ALL (0x1 << 1)

//设置相关
//所有的标志位都设置
#define FLAGGROUP_SET_ALL (FLAGGROUP_SET | FLAGGROUP_ALL)
//任意的标志位都置1
#define FLAGGROUP_SET_ANY (FLAGGROUP_SET | FLAGGROUP_ANY)
//清除相关
#define FLAGGROUP_CLEAR_ALL (FLAGGROUP_CLEAR | FLAGGROUP_ALL)
#define FLAGGROUP_CLEAR_ANY (FLAGGROUP_CLEAR | FLAGGROUP_ANY)

//是否需要清零或置1
#define FLAGGROUP_CONSUME (1 << 7)

void rzyOS_flag_group_init(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t flags);

#endif
