#ifndef __OSCONFIG_H
#define __OSCONFIG_H

//rzyOS 目前支持的任务数
#define RZYOS_PRIO_COUNT 32

//rzyOS 同优先级task时间片 
#define RZYOS_SLICE_MAX 10 // 10 * 1ms

//rzyOS idle task 堆栈大小
#define RZYOS_IDLETASK_STACK_SIZE 1024
//rzyOS idle task 优先级
#define RZYOS_IDLETASK_PRIO (RZYOS_PRIO_COUNT - 1)

//work queue task 堆栈大小
#define RZYOS_WQUEUE_STACK_SIZE 1024
//work queue task 优先级
#define RZYOS_WQUEUE_PRIO 1

//一个系统节拍的毫秒数
#define RZYOS_TICK_MS 1  //1ms

//一秒的系统节拍数
#define ONE_SECOND (1000 / RZYOS_TICK_MS)



/***********内核编译选项***********/

#define RZYOS_ENABLE_SEMAPHORE 1
#define RZYOS_ENABLE_MUTEX 1
#define RZYOS_ENABLE_EVENTGROUP 1
#define RZYOS_ENABLE_MBOX 1
#define RZYOS_ENABLE_MEMORY 1
#define RZYOS_ENABLE_WQUEUE 1
#define RZYOS_ENABLE_CPU_DETECT 1

/********************************/



/***********内核编译选项***********/

#define RZYOS_HOOK_ILDE  0
#define RZYOS_HOOK_SYSTICK  0
#define RZYOS_HOOK_SCHEDULE  0

/********************************/


#endif
