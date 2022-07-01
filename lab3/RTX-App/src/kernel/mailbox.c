#include "mailbox.h"


void addMessage(mailbox_t *mailbox, void *message_pointer) {
	
	msg_node *message_node = (msg_node *)message_pointer;
	
	RTX_MSG_HDR* header = message_node->header;
	U8 length = header->length;
	
	if( (length) > (mailbox->max_size - mailbox->current_size)){ //length larger than available size in mailbox
		// NOT ENOUGH MEM;
	}
	
	char * endAddress = (char*) ((mailbox->ring_buffer ) + mailbox->max_size-1); // get the last available address in ring buffer before looping around
	
	if( ( mailbox->tail+ length ) > endAddress){
		//we need to wrap around the ring buffer
		
		U32 overflow = mailbox->tail+ length - endAddress;
		for(U32 i = 0;i< ( length - overflow ) ;i++){ // from the tail to the end
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
		mailbox->tail = mailbox->tail+length;
	}	
	mailbox->current_size+=length;
}

int getMessage(mailbox_t *mailbox, void* buf, U8 reqSize) {
	
	//need to add check for invalid edge case
	RTX_MSG_HDR *header = (RTX_MSG_HDR *)(mailbox->head);
	char *return_message = (void *)(mailbox->head);
	U8 length = header->length;
	
	if(length > (reqSize+6))
	{
		//SET ERRNO
		return RTX_ERR;
	}
		
	char * endAddress = (char*) ((mailbox->ring_buffer ) + mailbox->max_size-1);
	return_message = buf;

	char* headAddress = mailbox->head;
	
	if(( headAddress+ length ) > endAddress) {
		
		U32 overflow = headAddress + length - endAddress;

		for(U32 i = 0;i<(length - overflow);i++){
			return_message[i] = *(headAddress+i);
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
		mailbox->head = mailbox->head+length;
	}
	
	mailbox->current_size-=length;
	return RTX_OK;
}

//Using size as input for now, will need to change to max size later on
BOOL isMailboxFull(mailbox_t *mailbox, U32 size){
	return mailbox->current_size == mailbox->max_size;
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
