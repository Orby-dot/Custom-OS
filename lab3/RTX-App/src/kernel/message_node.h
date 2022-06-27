#include "common.h"

#ifndef _MESSAGE_NODE
#define _MESSAGE_NODE

/** 
* taken from lab3 manual - changed name due to definition error
*/
typedef __packed struct msg_header {
	U32 length;
	task_t sender_tid;
	U8 type;
} MSG_HEADER;

/*Message Types*/
#define DEFAULT 0
#define KCD_REG 1
#define KCD_CMD 2
#define DISPLAY 3
#define KEY_IN 4

typedef __packed struct message_node {
	struct msg_header *header;
	char*data;
} msg_node;

#endif