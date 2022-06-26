#include "mailbox.h"

void addMessage(mailbox_t *mailbox, void *message_pointer) {
	
	struct message_node *message_node = (struct message_node *)message_pointer;
	if(mailbox->tail == NULL) {
		mailbox->head = message;
		mailbox->tail = message;
	}else {
		mailbox->tail->next = message;
		message->prev = mailbox->tail;
		mailbox->tail = message;
	}
	mailbox->size += message_node->message->length;
	mailbox->tail->next = mailbox->head;
	mailbox->head->prev = mailbox->tail;
}

void getMessage(mailbox_t *mailbox) {
	message_node *returnNode = mailbox->head;
	if(mailbox->tail && mailbox->tail->prev == NULL) {
		mailbox->tail = NULL;
	}
	if(mailbox->head){
		message_node *nextMessageNode = mailbox->head->next;
		nextMessageNode->prev = NULL;
		mailbox->head->next = NULL;
		mailbox->head->prev = NULL;
		mailbox->size -= mailbox->head->message->length;
		mailbox->head = nextMessageNode;
	}
	return returnNode;
}

//Using size as input for now, will need to change to max size later on
int isMailboxFull(mailbox_t *mailbox, U32 size){
	return mailbox->size <= size ? 1 : 0;
}

void initializeMailbox(mailbox_t *mailbox, U8 id, U32 size) {
	mailbox->id = id;	
	mailbox->size = size;
	mailbox->ring_buffer = k_mpool_alloc(MPID_IRAM2, size);
	if(mailbox->ring_buffer == NULL){
		//SET AN ERROR
	}
}

//Keep removing messages until mailbox empty
void deallocateMailbox(mailbox_t *mailbox){
	while (removeMessage(mailbox) == 1);
}