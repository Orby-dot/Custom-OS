#include "k_inc.h"
#include "common.h"

#ifndef READY_QUEUES

#define READY_QUEUES

typedef struct readyQueue {
	TCB *head;
	TCB *tail;
} readyQueue_t;

void initializeArrayOfReadyQueues(readyQueue_t * readyQueuesArray);
void addTCBtoBack(readyQueue_t * readyQueuesArray, U8 priorityLevel, TCB *tcb);
void addTCBtoFront(readyQueue_t * readyQueuesArray, U8 priorityLevel, TCB *tcb);
TCB *removeTCB(readyQueue_t * readyQueuesArray, U8 priorityLevel);
void removeSpecificTCB(readyQueue_t * readyQueuesArray, U8 priorityLevel, task_t tid);

void addTCBtoRecvBLK(readyQueue_t * readyQueuesArray, TCB *tcb);
void addTCBtoSentBLK(readyQueue_t * readyQueuesArray, TCB *tcb);

TCB *canSendMsg(readyQueue_t * readyQueuesArray, U8 target,U32 size);

#endif
