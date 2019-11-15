#include "rzyOS_event.h"

void rzyOS_event_init(rzyOS_ecb *ecb, rzyOS_event_type type)
{
	ecb -> type = event_type_unknow;
	list_init(&(ecb -> wait_list));
}
