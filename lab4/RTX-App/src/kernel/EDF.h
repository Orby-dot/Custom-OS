#include "ready_queue.h"

#define PERIOD 0
#define REMAINTIME 1 
int pushToEDF(readyQueue_t * queue, TCB * tsk, U8 mode);
TCB* popFromEDF(readyQueue_t * readyQueuesArray);
int isShorter(TIMEVAL current, TIMEVAL reference);
