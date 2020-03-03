#include "rzyOS_flag_group.h"

void rzyOS_flag_group_init(rzyOS_flag_group_s *rzyOS_flag_group, uint32_t flags)
{
	rzyOS_event_init(&(rzyOS_flag_group -> rzyOS_ecb), event_type_flag_group);
	rzyOS_flag_group -> flag = flags;
}
