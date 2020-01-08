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
