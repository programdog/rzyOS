#ifndef RZYOS_MBOX_H
#define RZYOS_MBOX_H

#include "rzyOS_event.h"

typedef struct rzyOS_mbox_s
{
	//ÊÂ¼þ¿ØÖÆ¿é
	rzyOS_ecb_s rzyOS_ecb;
	uint32_t count;
	uint32_t read;
	uint32_t write;
	uint32_t max_count;
	void **msg_buffer;
} rzyOS_mbox_s;

void rzyOS_mbox_init(rzyOS_mbox_s *rzyOS_mbox, void **msg_buffer, uint32_t max_count);

#endif
