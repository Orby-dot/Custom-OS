#include "ready_queue.h"

void initializeArrayOfReadyQueues(readyQueue_t * readyQueuesArray) {
	for (U8 i = 0; i<4; i++){
		readyQueuesArray[i].head = NULL;
		readyQueuesArray[i].tail = NULL;
	}
}
	
void addTCBtoBack(readyQueue_t * readyQueuesArray, U32 priorityLevel, TCB tcb) {
	if(readyQueuesArray[priorityLevel].tail){
		readyQueuesArray[priorityLevel].tail->next = &tcb;
		tcb.prev = readyQueuesArray[priorityLevel].tail;
		tcb.next = NULL;
		readyQueuesArray[priorityLevel].tail = &tcb;
	} else { // if queue is empty
		tcb.prev = NULL;
		tcb.next = NULL;
		readyQueuesArray[priorityLevel].tail = &tcb;
		readyQueuesArray[priorityLevel].head = &tcb;
	}
}

void addTCBtoFront(readyQueue_t * readyQueuesArray, U32 priorityLevel, TCB tcb) {
	if(readyQueuesArray[priorityLevel].head){
		readyQueuesArray[priorityLevel].head->prev = &tcb;
		tcb.next = readyQueuesArray[priorityLevel].head;
		tcb.prev = NULL;
		readyQueuesArray[priorityLevel].head = &tcb;
	} else { // if queue is empty
		tcb.next = NULL;
		tcb.prev = NULL;
		readyQueuesArray[priorityLevel].tail = &tcb;
		readyQueuesArray[priorityLevel].head = &tcb;
	}
}


TCB *removeTCB(readyQueue_t * readyQueuesArray, U32 priorityLevel) {
	TCB *returnTCB = readyQueuesArray[priorityLevel].head;
	if (readyQueuesArray[priorityLevel].head == readyQueuesArray[priorityLevel].tail){
		readyQueuesArray[priorityLevel].head = NULL;
		readyQueuesArray[priorityLevel].tail = NULL;
	} else {
		readyQueuesArray[priorityLevel].head = returnTCB->next;
		readyQueuesArray[priorityLevel].head->prev = NULL;
		returnTCB->next = NULL;
	}
	return returnTCB;
}
