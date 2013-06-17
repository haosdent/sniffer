#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "list.h"

struct record {
	uint32_t src_ip;
	uint16_t src_port;
	uint32_t dst_ip;
	uint16_t dst_port;
	uint32_t count;
	struct list_head node;
};

uint32_t convert_ip(unsigned char *start);

#endif
