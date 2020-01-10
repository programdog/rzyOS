#ifndef RZYOS_MBOX_H
#define RZYOS_MBOX_H

#include "rzyOS.h"

typedef struct rzyOS_mbox_s
{
	//事件控制块
	rzyOS_ecb_s rzyOS_ecb;
	//邮箱计数
	uint32_t count;
	//读的位置
	uint32_t read;
	//写的位置
	uint32_t write;
	//最大空间
	uint32_t max_count;
	//buffer地址
	void **msg_buffer;
} rzyOS_mbox_s;

//邮箱初始化
void rzyOS_mbox_init(rzyOS_mbox_s *rzyOS_mbox, void **msg_buffer, uint32_t max_count);

#endif
