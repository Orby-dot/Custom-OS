#include "common.h"

#ifndef _MESSAGE_NODE
#define _MESSAGE_NODE

typedef __packed struct rtx_msg_buf {
	U32 length;
	task_t sender_tid;
	U8 type;
} RTX_MSG_BUF;

/*Message Types*/
#define DEFAULT 0
#define KCD_REG 1
#define KCD_CMD 2
#define DISPLAY 3
#define KEY_IN 4

typedef struct message_node {
	struct message_node *prev;
	struct message_node *next;
	struct rtx_msg_buf *message;
} message_node;

#endif