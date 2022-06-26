#include "common.h"
#include "message_node.h"
#include "ready_queue.h"

#ifndef _MAILBOX
#define _MAILBOX

typedef struct mailbox {
	U8 id;
	U32 size;
	char* ring_buffer;
} mailbox_t;

void addMessage(mailbox_t *mailbox, void *message_pointer);

void getMessage(mailbox_t *mailbox);

void initializeMailbox(mailbox_t *mailbox, U8 id, U32 size);

void deallocateMailbox(mailbox_t *mailbox);

#endif