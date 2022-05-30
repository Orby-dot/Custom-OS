#include "ready_queue.h"

#define MAX_U8 255

// Converts common.h-style priority level to index from 0 to 4
U8 priorityLevelToIndex(U8 priorityLevel) {
	if (priorityLevel >= 0x80 && priorityLevel <= 0x83) {
		return (priorityLevel - 0x80);
	} else {
		return MAX_U8;
	}
}

// readyQueuesArray: Array of Ready Queues
void initializeArrayOfReadyQueues(readyQueue_t * readyQueuesArray) {
	for (U8 i = 0; i<4; i++){
		readyQueuesArray[i].head = NULL;
		readyQueuesArray[i].tail = NULL;
	}
}
	
// readyQueuesArray: Array of Ready Queues, priorityLevel: 0 to 3, tcb: to add
void addTCBtoBack(readyQueue_t * readyQueuesArray, U8 priorityLevel, TCB tcb) {
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

// readyQueuesArray: Array of Ready Queues, priorityLevel: 0 to 3, tcb: to add
void addTCBtoFront(readyQueue_t * readyQueuesArray, U8 priorityLevel, TCB tcb) {
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

/**
 * readyQueuesArray: Array of Ready Queues, priorityLevel: 0 to 3
 * returns pointer to TCB that was removed from the HEAD of the queue
 * this function does NOT check if the queue is empty
 */
TCB *removeTCB(readyQueue_t * readyQueuesArray, U8 priorityLevel) {
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
