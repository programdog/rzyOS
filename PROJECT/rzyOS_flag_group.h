#ifndef __RZYOS_FLAG_GROUP_H
#define __RZYOS_FLAG_GROUP_H

#include "rzyOS_event.h"

typedef struct rzyOS_flag_group_s
{
	rzyOS_ecb_s rzyOS_ecb;
	uint32_t flag;
} rzyOS_flag_group_s;

#endif
