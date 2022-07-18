#include "mailbox.h"


void addMessage(mailbox_t *mailbox, void *message_pointer) {
	
	RTX_MSG_HDR* header = message_pointer;
	U8 length = header->length;
	// U32 available_space = 0; // unused variable
	
	if( (length) > (mailbox->max_size - mailbox->current_size)){ //length larger than available size in mailbox
		// NOT ENOUGH MEM;
		return;
	}
	
	char * endAddress = (char*) ((mailbox->ring_buffer ) + mailbox->max_size-1); // get the last available address in ring buffer before looping around

	if( ( mailbox->tail+ length-1 ) > endAddress){
		//we need to wrap around the ring buffer
		
		U32 overflow = mailbox->tail+ length-1 - endAddress;
		for(U32 i = 0;i< ( length - overflow ) ;i++){ // from the tail to the end
			*(mailbox->tail+i) = ((char*) message_pointer)[i];
		}
		for(U32 i = 0;i<(overflow);i++){ //from the beginning and cover the remaining overflowed length
			*(mailbox->ring_buffer+i) = ((char*) message_pointer)[i + (length - overflow) ];
		}
		mailbox->tail = mailbox->ring_buffer+(overflow);
	}
	else{
		// we can allocate a contiguous chunk of memory
		for(U32 i = 0;i<length;i++){ // from the tail to the end
			*(mailbox->tail+i) = ((char*) message_pointer)[i];
		}
		mailbox->tail = mailbox->tail+length;
	}	
	mailbox->current_size+=length;
}

int getMessage(mailbox_t *mailbox, void* buf, U8 reqSize) {
	
	char * endAddress = (char*) ((mailbox->ring_buffer ) + mailbox->max_size-1);
	
	char tmp_header[6];
	char * tmp_ptr = mailbox->head;
	U32 i = 0;
	while(i<6){
		if( (tmp_ptr+i)>endAddress ){
			tmp_ptr = mailbox->ring_buffer-i;
		}
		tmp_header[i] = *(tmp_ptr+i);
		i++;
	}
	
	RTX_MSG_HDR *header = (RTX_MSG_HDR *)tmp_header;
	
	U8 length = header->length;//
	
	// This is the only error we need to read the header
	if(length > (reqSize)) {
		errno = ENOSPC;
		return RTX_ERR;//
	}
	
	char *return_message = buf;
	char* headAddress = mailbox->head;
	//printf("Head: %x\r\n",headAddress);
	
	if( ( headAddress+ length -1 ) > endAddress ) {
		
		U32 overflow = headAddress + length -1 - endAddress;
		for(U32 i = 0;i<(length - overflow);i++){
			return_message[i] = *(headAddress+i);
		}
		for(U32 i = 0;i<(overflow);i++){
			return_message[i+(length - overflow)] = *(mailbox->ring_buffer+i);
		}
	
		mailbox->head = mailbox->ring_buffer+(overflow);
	} 
	else {
		for( i = 0 ; i < length; i++)
		{
			return_message[i] = *(mailbox->head+i);
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
	mailbox->head = mailbox->ring_buffer;
	mailbox->tail = mailbox->ring_buffer; 
	if(mailbox->ring_buffer == NULL){
		errno = ENOMEM;
		// return RTX_ERR happens in k_mbx_create
	}
}

//Keep removing messages until mailbox empty
void deallocateMailbox(mailbox_t *mailbox){
	k_mpool_dealloc(MPID_IRAM1, mailbox->ring_buffer);
	mailbox->head = NULL;
	mailbox->tail = NULL;
	mailbox->ring_buffer=NULL;
	mailbox->max_size = 0;
	mailbox->current_size = 0;
}
