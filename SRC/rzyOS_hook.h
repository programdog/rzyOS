#ifndef RZYOS_HOOK_H
#define RZYOS_HOOK_H

#include "rzyOS.h"

void rzyOS_hook_idle(void);

void rzyOS_hook_systick(void);

void rzyOS_hook_schedule(task_tcb_s *this_task, task_tcb_s *next_task);


#endif
