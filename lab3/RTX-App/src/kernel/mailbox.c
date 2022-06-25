#include "mailbox.h"

void addMessage(mailbox_t *mailbox, message_node *message_node) {
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

int removeMessage(mailbox_t *mailbox) {
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
	return mailbox->head == NULL ? 0 : 1;
}

//Using size as input for now, will need to change to max size later on
int isMailboxFull(mailbox_t *mailbox, U32 size){
	return mailbox->size <= size ? 1 : 0;
}

void initializeMailbox(mailbox_t *mailbox, U8 id) {
	mailbox->id = id;
	mailbox->size = 0;
}

//Keep removing messages until mailbox empty
void deallocateMailbox(mailbox_t *mailbox){
	while (removeMessage(mailbox) == 1);
}