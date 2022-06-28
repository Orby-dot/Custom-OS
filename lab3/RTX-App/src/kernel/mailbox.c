#include "mailbox.h"

void addMessage(mailbox_t *mailbox, void *message_pointer) {
	
	msg_node *message_node = (msg_node *)message_pointer;
	
	MSG_HEADER* header = message_node->header;
	U8 length = header->length;
	
	if( (length+6) > (mailbox->max_size - mailbox->current_size)){ //length larger than available size in mailbox
		// NOT ENOUGH MEM;
	}
	
	char * endAddress = (char*) ((mailbox->ring_buffer ) + mailbox->max_size-1); // get the last available address in ring buffer before looping around
	
	if( ( (mailbox->tail)+ (length+6) ) > endAddress){
		//we need to wrap around the ring buffer
		
		U32 overflow = (mailbox->tail)+ (length+6) - endAddress;
		for(U32 i = 0;i<(length - overflow);i++){ // from the tail to the end
			*(mailbox->tail+i) = ((char*) message_node)[i];
		}
		for(U32 i = 0;i<(overflow);i++){ //from the beginning and cover the remaining overflowed length
			*(mailbox->ring_buffer+i) = ((char*) message_node)[i];
		}
		mailbox->tail = mailbox->ring_buffer+overflow;
	}
	else{
		// we can allocate a contiguous chunk of memory
		for(U32 i = 0;i<length;i++){ // from the tail to the end
			*(mailbox->tail+i) = ((char*) message_node)[i];
		}
		mailbox->tail = mailbox->tail+(length+6);
	}	
	mailbox->current_size+=(length+6);
}

void *getMessage(mailbox_t *mailbox) {
	
	//need to add check for invalid edge case
	MSG_HEADER *header = (MSG_HEADER *)(mailbox->head);
	void *return_message = (void *)(mailbox->head);
	U8 length = header->length;
	
	char * endAddress = (char*) ((mailbox->ring_buffer ) + mailbox->max_size-1);
	char *return_message = (char*)k_mpool_alloc(MPID_IRAM1, length);
	
	if(((mailbox->tail)+ (length+6)) > endAddress) {
		U32 overflow = (mailbox->head)+ (length+6) - endAddress;

		for(U32 i = 0;i<(length - overflow);i++){
			return_message[i] = *(mailbox->head+i);
		}
		for(U32 i = 0;i<(overflow);i++){
			return_message[i] = *(mailbox->ring_buffer+i);
		}
	
		mailbox->head = mailbox->ring_buffer+overflow;
	} 
	else {
		U32 i;
		while(i<length){
			return_message[i] = *(mailbox->head+i);
			i+=1;
		}
		mailbox->head = mailbox->head+(length+6);
	}
	
	mailbox->current_size-=(length+6);
	return ((void *)return_message;
}

//Using size as input for now, will need to change to max size later on
int isMailboxFull(mailbox_t *mailbox, U32 size){
	return mailbox->size <= size ? 1 : 0;
}

void initializeMailbox(mailbox_t *mailbox, U8 id, U32 size) {
	mailbox->id = id;	
	mailbox->max_size = size;
	mailbox->current_size = 0;
	mailbox->ring_buffer = k_mpool_alloc(MPID_IRAM1, size);
	if(mailbox->ring_buffer == NULL){
		//SET AN ERROR
	}
}

//Keep removing messages until mailbox empty
void deallocateMailbox(mailbox_t *mailbox){
	k_mpool_dealloc(MPID_IRAM1, mailbox->ring_buffer);
}