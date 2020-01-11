#ifndef RZYOS_TASK_H
#define RZYOS_TASK_H

#include "rzyOS_event.h"

//(任务状态)任务就绪
#define RZYOS_TASK_STATUS_READY 0
//(任务状态)任务等待
#define RZYOS_TASK_STATUS_DELAY (1 << 1)
//(任务状态)任务挂起
#define RZYOS_TASK_STATUS_SUSPEND (1 << 2)
//(任务状态)任务删除
#define RZYOS_TASK_STATUS_DELETE (1 << 3)


//(任务状态)高16位作为事件等待标志
#define RZYOS_TASK_WAIT_MASK (0xff << 16)


//定义任务堆栈地址为32位类型
typedef uint32_t tTaskStack;


//错误类型
typedef enum rzyOS_error_e
{
	//无错误
	error_no_error = 0,
	//超时等待
	error_timeout = 1,
	//事件(信号量&邮箱)无资源
	error_resource_unvaliable = 2,
	//信号量已经被删除
	error_sem_delete = 3,
	//资源已满
	error_resource_full = 4,
} rzyOS_error_e;


//task.h与event.h结构体互相编译, 则做一下 原型宣告 声明
typedef struct rzyOS_ecb_s rzyOS_ecb_s;


//task TCB
typedef struct task_tcb_s
{
	//任务堆栈指针
	tTaskStack *stack;

	//任务节点
	node_t link_node;

	//任务注册延时周期
	uint32_t delayTicks;

	//任务延时节点
	node_t delay_node;

	//任务优先级
	uint32_t prio;

	//任务状态
	uint32_t task_status;

	//时间片
	uint32_t slice;
	
	//任务挂起计数器
	uint32_t suspend_count;
	
	//任务删除
	//任务清除callback函数
	void (*clean)(void *param);
	//任务清除callback函数参数
	void *clean_param;
	//任务请求删除状态标记
	uint8_t request_delete_flag;
	
	//事件控制块
	rzyOS_ecb_s *wait_event;
	//等待事件的消息数据
	void *event_msg;
	//等待事件的结果
	uint32_t wait_event_result;
	//请求的事件类型
	uint32_t wait_flag_type;
	//请求的事件标志
	uint32_t event_flag;
} task_tcb_s;


//task info struct
typedef struct rzyOS_task_info_s
{
	//任务注册延时周期
	uint32_t delay_ticks;
	//任务优先级
	uint32_t prio;
	//任务状态
	uint32_t task_status;
	//时间片(针对同优先级任务)
	uint32_t slice;
	//任务挂起计数器
	uint32_t suspend_count;
} rzyOS_task_info_s;


//task初始化
void task_init(task_tcb_s *task, void (*entry)(void *), void *param, uint32_t prio, tTaskStack *stack);

//任务的挂起
void rzyOS_task_suspend(task_tcb_s *task);

//任务的唤醒
void rzyOS_task_wakeup(task_tcb_s *task);

//任务清除回调函数
void rzyOS_task_clean_callback(task_tcb_s *task, void (*clean)(void *param), void *param);

//任务强制删除函数
void rzyOS_task_force_delete(task_tcb_s *task);

//任务请求删除函数
void rzyOS_task_request_delete(task_tcb_s *task);

//对于任务请求删除的检测函数
uint8_t rzyOS_task_request_delete_check(void);

//任务自我删除函数
void rzyOS_task_delete_self(void);

//获取任务信息
void rzyOS_task_get_info(task_tcb_s *task, rzyOS_task_info_s *info);

#endif
