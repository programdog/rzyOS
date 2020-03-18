#ifndef __RZYOS_FLAG_GROUP_H
#define __RZYOS_FLAG_GROUP_H

#include "rzyOS_event.h"
#include "rzyOS.h"

//事件标志组结构
typedef struct rzyOS_flag_group_s
{
	rzyOS_ecb_s rzyOS_ecb;
	//事件标志组标志
	uint32_t flag;
} rzyOS_flag_group_s;



//事件类型
/*========================================================================*/

//BIT8 : |__7__|__6__|__5__|__4__|---|__3__|__2__|any/all|set/clean|

//清除标志位
#define FLAGGROUP_CLEAR (0x0 << 0)
//设置标志位
#define FLAGGROUP_SET (0x1 << 0)
//任意的标志位
#define FLAGGROUP_ANY (0x0 << 1)
//所有的标志位
#define FLAGGROUP_ALL (0x1 << 1)

//事件类型,标志组合
//所有的标志位设置
#define FLAGGROUP_SET_ALL (FLAGGROUP_SET | FLAGGROUP_ALL)
//任意的标志位设置
#define FLAGGROUP_SET_ANY (FLAGGROUP_SET | FLAGGROUP_ANY)
//所有的标志位清除
#define FLAGGROUP_CLEAR_ALL (FLAGGROUP_CLEAR | FLAGGROUP_ALL)
//任意的标志位清除
#define FLAGGROUP_CLEAR_ANY (FLAGGROUP_CLEAR | FLAGGROUP_ANY)

//是否需要清零或置位
#define FLAGGROUP_CONSUME (1 << 7)

/*========================================================================*/


//时间组初始化函数
void rzyOS_flag_group_init(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t flags);

//事件组等待函数(阻塞)
uint32_t rzyOS_flag_group_wait(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t wait_type, uint32_t request_flag, uint32_t *result_flag, uint32_t wait_ticks);

//事件组等待函数(非阻塞)
uint32_t rzyOS_flag_group_no_wait(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t wait_type, uint32_t request_flag, uint32_t *result_flag);

//时间组通知函数
void rzyOS_flag_group_post(rzyOS_flag_group_s *rzyOS_flag_group, uint8_t is_set, uint32_t flag);

#endif
