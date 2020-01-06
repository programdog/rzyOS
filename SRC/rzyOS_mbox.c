#include "rzyOS_mbox.h"

void rzyOS_mbox_init(rzyOS_mbox_s *rzyOS_mbox, void **msg_buffer, uint32_t max_count)
{
	rzyOS_event_init(&(rzyOS_mbox -> rzyOS_ecb), event_type_mbox);
	
	rzyOS_mbox -> msg_buffer = msg_buffer;
	rzyOS_mbox -> max_count = max_count;
	rzyOS_mbox -> count = 0;
	rzyOS_mbox -> read = 0;
	rzyOS_mbox -> write = 0;
}
