#ifndef RZYOS_MBOX_H
#define RZYOS_MBOX_H

#include "rzyOS.h"

#define rzyOS_mbox_send_normal 0x00
//插入到消息缓冲区前面
#define rzyOS_mbox_send_front 0x01

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

//消息等待函数（阻塞模式）
uint32_t rzyOS_mbox_wait(rzyOS_mbox_s *rzyOS_mbox, void **msg, uint32_t wait_tick);

//消息等待函数（非阻塞模式）
uint32_t rzyOS_mbox_nowait(rzyOS_mbox_s *rzyOS_mbox, void **msg);

//post一个消息
uint32_t rzyOS_mbox_post(rzyOS_mbox_s *rzyOS_mbox, void *msg, uint32_t notify_option);

//邮箱的清空
void rzyOS_mbox_flush(rzyOS_mbox_s *rzyOS_mbox);

//邮箱的删除
uint32_t rzyOS_mbox_destory(rzyOS_mbox_s *rzyOS_mbox)


#endif
