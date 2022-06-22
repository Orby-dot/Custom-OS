#include "ready_queue.h"

#define MAX_U8 255

// Converts common.h-style priority level to index from 0 to 4
U8 priorityLevelToIndex(U8 priorityLevel) {
	if (priorityLevel >= 0x80 && priorityLevel <= 0x83) {
		return (priorityLevel - 0x80);
	} else {
		return 4; // for the null task
	}
}

// readyQueuesArray: Array of Ready Queues
void initializeArrayOfReadyQueues(readyQueue_t * readyQueuesArray) {
	for (U8 i = 0; i<5; i++){
		readyQueuesArray[i].head = NULL;
		readyQueuesArray[i].tail = NULL;
	}
}
	
// readyQueuesArray: Array of Ready Queues, priorityLevel: 0 to 3, tcb: to add
void addTCBtoBack(readyQueue_t * readyQueuesArray, U8 priorityLevel, TCB *tcb) {
	U8 arrayIndex = priorityLevelToIndex(priorityLevel);
	if(readyQueuesArray[arrayIndex].tail){
		readyQueuesArray[arrayIndex].tail->next = tcb;
		tcb->prev = readyQueuesArray[arrayIndex].tail;
		tcb->next = NULL;
		readyQueuesArray[arrayIndex].tail = tcb;
	} else { // if queue is empty
		tcb->prev = NULL;
		tcb->next = NULL;
		readyQueuesArray[arrayIndex].tail = tcb;
		readyQueuesArray[arrayIndex].head = tcb;
	}
}

// readyQueuesArray: Array of Ready Queues, priorityLevel: 0 to 3, tcb: to add
void addTCBtoFront(readyQueue_t * readyQueuesArray, U8 priorityLevel, TCB *tcb) {
	U8 arrayIndex = priorityLevelToIndex(priorityLevel);
	if(readyQueuesArray[arrayIndex].head){
		readyQueuesArray[arrayIndex].head->prev = tcb;
		tcb->next = readyQueuesArray[arrayIndex].head;
		tcb->prev = NULL;
		readyQueuesArray[arrayIndex].head = tcb;
	} else { // if queue is empty
		tcb->next = NULL;
		tcb->prev = NULL;
		readyQueuesArray[arrayIndex].tail = tcb;
		readyQueuesArray[arrayIndex].head = tcb;
	}
}

/**
 * readyQueuesArray: Array of Ready Queues, priorityLevel: 0 to 3
 * returns pointer to TCB that was removed from the HEAD of the queue
 * this function does NOT check if the queue is empty
 */
TCB *removeTCB(readyQueue_t * readyQueuesArray, U8 arrayIndex) {
	TCB *returnTCB = readyQueuesArray[arrayIndex].head;
	if (readyQueuesArray[arrayIndex].head == readyQueuesArray[arrayIndex].tail){
		readyQueuesArray[arrayIndex].head = NULL;
		readyQueuesArray[arrayIndex].tail = NULL;
	} else {
		readyQueuesArray[arrayIndex].head = returnTCB->next;
		readyQueuesArray[arrayIndex].head->prev = NULL;
		returnTCB->next = NULL;
	}
	return returnTCB;
}

void removeSpecificTCB(readyQueue_t * readyQueuesArray, U8 priorityLevel, task_t tid) {
	U8 arrayIndex = priorityLevelToIndex(priorityLevel);
	TCB *taskToRemove = readyQueuesArray[arrayIndex].head;
	
	while(taskToRemove && taskToRemove->tid!=tid){
		taskToRemove = taskToRemove->next;
	}
	
	if(taskToRemove){
		
		if(taskToRemove->prev){
			taskToRemove->prev->next = taskToRemove->next;
		}
		
		if (taskToRemove->next){
			taskToRemove->next->prev = taskToRemove->prev;
		}
		
		if(taskToRemove == readyQueuesArray[priorityLevel].head){
			readyQueuesArray[arrayIndex].head = taskToRemove->next;
		}
		
		if(taskToRemove == readyQueuesArray[priorityLevel].tail){
			readyQueuesArray[arrayIndex].tail = taskToRemove->prev;
		}
		
		taskToRemove->next = NULL;
		taskToRemove->prev = NULL;
	}
}
