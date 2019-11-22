#ifndef RZYOS_EVENT_H
#define RZYOS_EVENT_H

#include "bitMapLib.h"


typedef enum rzyOS_event_type_e
{
	event_type_unknow,
} rzyOS_event_type_e;


typedef struct rzyOS_ecb_s
{
	rzyOS_event_type_e type;
	list_t wait_list;
} rzyOS_ecb_s;


//任务希望请求的事件标志组时间类型
#define FLAGGROUP_CLEAR (0x0 << 0)
#define FLAGGROUP_SET (0x1 << 0)
#define FLAGGROUP_ANY (0x0 << 1)
#define FLAGGROUP_ALL (0x1 << 1)

#define FLAGGROUP_SET_ALL (FLAGGROUP_SET | FLAGGROUP_ALL)
#define FLAGGROUP_SET_ANY (FLAGGROUP_SET | FLAGGROUP_ANY)
#define FLAGGROUP_CLEAR_ALL (FLAGGROUP_CLEAR | FLAGGROUP_ALL)
#define FLAGGROUP_CLEAR_ANY (FLAGGROUP_CLEAR | FLAGGROUP_ANY)

#define FLAGGROUP_CONSUME (1 << 7)


void rzyOS_event_init(rzyOS_ecb_s *ecb, rzyOS_event_type_e type);

#endif
