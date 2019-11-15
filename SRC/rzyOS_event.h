#ifndef RZYOS_EVENT_H
#define RZYOS_EVENT_H

#include "bitMapLib.h"

typedef enum rzyOS_event_type
{
	event_type_unknow,
} rzyOS_event_type;

typedef struct rzyOS_ecb
{
	rzyOS_event_type type;
	list_t wait_list;
} rzyOS_ecb;

void rzyOS_event_init(rzyOS_ecb *ecb, rzyOS_event_type type);

#endif
