#include "common.h"
#include "message_node.h"
#include "k_mem.h"

#ifndef _MAILBOX
#define _MAILBOX

typedef struct mailbox {
	U8 id;
	U32 max_size; // total bytes available for the ring_buffer - defined in initialize function
	U32 current_size; //number of bytes occupied
	char* ring_buffer;	
	char* head;
	char* tail;
} mailbox_t;

void addMessage(mailbox_t *mailbox, void *message_pointer);
int getMessage(mailbox_t *mailbox,void* buf, U8 reqSize);
void initializeMailbox(mailbox_t *mailbox, U8 id, U32 size);
void deallocateMailbox(mailbox_t *mailbox);
BOOL isMailboxFull(mailbox_t *mailbox, U32 size);

#endif
