#include "k_inc.h"
#include "common.h"

#ifndef READY_QUEUES

#define READY_QUEUES

typedef struct readyQueue {
	TCB *head;
	TCB *tail;
} readyQueue_t;

void initializeArrayOfReadyQueues(readyQueue_t * readyQueuesArray);
void addTCB(readyQueue_t * readyQueuesArray,U32 priorityLevel, TCB tcb);
TCB removeTCB(readyQueue_t * readyQueuesArray, U32 priorityLevel);

#endif