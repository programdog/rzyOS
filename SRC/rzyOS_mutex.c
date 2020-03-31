#include "rzyOS_mutex.h"


//互斥锁初始化函数
void rzyOS_mutex_init(rzyOS_mutex_s *rzyOS_mutex)
{
	//互斥锁事件结构初始化
	rzyOS_event_init(&(rzyOS_mutex -> rzyOS_ecb), event_type_mutex);

	//互斥锁事件结构字段初始化
	rzyOS_mutex -> lock_count = 0;
	rzyOS_mutex -> owner = (task_tcb_s *)0;
	rzyOS_mutex -> owner_original_prio = RZYOS_PRIO_COUNT;
}
