#ifndef __OSCONFIG_H
#define __OSCONFIG_H

//rzyOS 目前支持的任务数
#define RZYOS_PRIO_COUNT 32

//rzyOS 同优先级task时间片 
#define RZYOS_SLICE_MAX 10 // 10 * 10ms

//rzyOS idle task 堆栈大小
#define RZYOS_IDLETASK_STACK_SIZE 1024
//rzyOS idle task 优先级
#define RZYOS_IDLETASK_PRIO (RZYOS_PRIO_COUNT - 1)

//work queue task 堆栈大小
#define RZYOS_WQUEUE_STACK_SIZE 1024
//work queue task 优先级
#define RZYOS_WQUEUE_PRIO 1


#endif
