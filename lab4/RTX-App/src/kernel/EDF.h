#include "ready_queue.h"

int pushToEDF(readyQueue_t * queue, TCB * tsk);
TCB* popToEDF(readyQueue_t * queue);
int isShorter(TIMEVAL current, TIMEVAL reference);