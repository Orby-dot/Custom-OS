#include "ready_queue.h"

void initializeArrayOfReadyQueues(readyQueue_t * readyQueuesArray) {
	for (U8 i = 0; i<4; i++){
		readyQueuesArray[i].head = NULL;
		readyQueuesArray[i].tail = NULL;
	}
}
	
void addTCB(readyQueue_t * readyQueuesArray, U32 priorityLevel, TCB tcb) {
	if(readyQueuesArray[priorityLevel].tail){
		readyQueuesArray[priorityLevel].tail->next = &tcb;
		tcb.prev = readyQueuesArray[priorityLevel].tail;
		readyQueuesArray[priorityLevel].tail = &tcb;
	} else {
		readyQueuesArray[priorityLevel].tail = &tcb;
		readyQueuesArray[priorityLevel].head = &tcb;
	}
}

TCB removeTCB(readyQueue_t * readyQueuesArray, U32 priorityLevel) {
	TCB *returnTCB = readyQueuesArray[priorityLevel].head;
	if (readyQueuesArray[priorityLevel].head == readyQueuesArray[priorityLevel].tail){
		readyQueuesArray[priorityLevel].head = NULL;
		readyQueuesArray[priorityLevel].tail = NULL;
	} else {
		readyQueuesArray[priorityLevel].head = returnTCB->next;
		readyQueuesArray[priorityLevel].head->prev = NULL;
		returnTCB->next = NULL;
	}
	return *returnTCB;
}