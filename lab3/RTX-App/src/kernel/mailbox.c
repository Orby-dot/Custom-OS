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
}

void removeMessage(mailbox_t *mailbox) {
	if(mailbox->tail->prev == NULL) {
		mailbox->tail = NULL;
	}
	if(mailbox->head){
		message_node *nextMessageNode = mailbox->head->next;
		nextMessageNode->prev = NULL;
		mailbox->head->next = NULL;
		mailbox->head = nextMessageNode;
		mailbox->size -= message_node->message->length;
	}
}
