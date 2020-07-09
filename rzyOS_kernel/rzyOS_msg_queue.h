#ifndef RZYOS_MSG_QUEUE_H
#define RZYOS_MSG_QUEUE_H

#include "rzyOS_schedule.h"

//正常模式， 插入到缓冲区的后边
#define rzyOS_msg_queue_send_normal 0x00
//插入到消息缓冲区前面
#define rzyOS_msg_queue_send_front 0x01

//消息队列信息结构
typedef struct rzyOS_msg_queue_info_s
{
	//当前消息数量
	uint32_t count;
	//最大支持的消息数量
	uint32_t max_count;
	//等待的任务数量
	uint32_t task_count;
} rzyOS_msg_queue_info_s;

//消息队列结构
typedef struct rzyOS_msg_queue_s
{
	//事件控制块
	rzyOS_ecb_s rzyOS_ecb;
	//消息队列计数
	uint32_t count;
	//读的位置
	uint32_t read;
	//写的位置
	uint32_t write;
	//最大空间
	uint32_t max_count;
	//buffer地址
	void **msg_buffer;
} rzyOS_msg_queue_s;

//消息队列初始化
void rzyOS_msg_queue_init(rzyOS_msg_queue_s *rzyOS_msg_queue, void **msg_buffer, uint32_t max_count);

//消息等待函数（阻塞模式）
uint32_t rzyOS_msg_queue_wait(rzyOS_msg_queue_s *rzyOS_msg_queue, void **msg, uint32_t wait_tick);

//消息等待函数（非阻塞模式）
uint32_t rzyOS_msg_queue_nowait(rzyOS_msg_queue_s *rzyOS_msg_queue, void **msg);

//post一个消息
uint32_t rzyOS_msg_queue_post(rzyOS_msg_queue_s *rzyOS_msg_queue, void *msg, uint32_t notify_option);

//消息队列的清空
void rzyOS_msg_queue_flush(rzyOS_msg_queue_s *rzyOS_msg_queue);

//消息队列的删除
uint32_t rzyOS_msg_queue_destory(rzyOS_msg_queue_s *rzyOS_msg_queue);

//消息队列状态查询
void rzyOS_msg_queue_get_info(rzyOS_msg_queue_s *rzyOS_msg_queue, rzyOS_msg_queue_info_s *rzyOS_msg_queue_info);


#endif
