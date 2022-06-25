#include "common.h"
#include "message_node.h"

#ifndef _MAILBOX
#define _MAILBOX

typedef struct mailbox {
	U8 id;
	struct message_node *head;
	struct message_node *tail;
	U32 size;
} mailbox_t;

void addMessage(mailbox_t *mailbox, message_node *message);

int removeMessage(mailbox_t *mailbox, message_node *message);

void initializeMailbox(mailbox_t *mailbox, U8 id);

void deallocateMailbox(mailbox_t *mailbox);

#endif