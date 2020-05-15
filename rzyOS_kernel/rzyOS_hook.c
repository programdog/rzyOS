#include "rzyOS_hook.h"

#if RZYOS_HOOK_ILDE == 1
void rzyOS_hook_idle(void)
{

}
#endif

#if RZYOS_HOOK_SYSTICK == 1
void rzyOS_hook_systick(void)
{

}
#endif

#if RZYOS_HOOK_SCHEDULE == 1
void rzyOS_hook_schedule(task_tcb_s *this_task, task_tcb_s *next_task)
{

}
#endif
