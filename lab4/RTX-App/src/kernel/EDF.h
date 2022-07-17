#include "ready_queue.h"

int pushToEDF(readyQueue_t * queue, TCB * tsk);
TCB* popFromEDF(readyQueue_t * readyQueuesArray);
int isShorter(TIMEVAL current, TIMEVAL reference);
