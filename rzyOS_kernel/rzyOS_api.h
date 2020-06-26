#ifndef RZYOS_API_H
#define RZYOS_API_H

#include "rzyOSconfig.h"

#include "rzyOS_schedule.h"

#include "rzyOS_eventgroup.h"
#include "rzyOS_hook.h"
#include "rzyOS_mbox.h"

//默认使用mm1
#if RZYOS_ENABLE_MEMORY == 1

	#if RZYOS_MM1_USE == 1
		#include "rzyOS_mm1.h"
	#endif

	#if RZYOS_MM2_USE == 1
		#include "rzyOS_mm2.h"
	#endif

#endif

#include "rzyOS_semaphore.h"
#include "rzyOS_mutex.h"
#include "rzyOS_wqueue.h"


#endif

