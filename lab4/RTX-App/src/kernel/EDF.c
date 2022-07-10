#include "EDF.h"
int pushToEDF(readyQueue_t * queue, TCB * tsk)
{
	TCB* currentTCB = queue->head;
	while(currentTCB != NULL)
	{
		//if found a tsk with a longer period
		if(isShorter(tsk->period,currentTCB->period))
		{
			//if that tsk is the head
			if(currentTCB == queue->head)
			{
				queue->head = tsk;
				tsk ->next = currentTCB;
				currentTCB->prev = tsk;
			}
			//if not
			else
			{
				TCB * temp = currentTCB->prev;
			
				temp->next = tsk;
				tsk->prev = temp;
			
				currentTCB->prev = tsk;
				tsk->next = currentTCB;
			}
			return 1;
		}
		//cycle thru LL
		currentTCB = currentTCB->next;
	}
	//if at end of LL and no tsk longer than tsk
	
	//if LL is empty
	if(queue->head == NULL)
	{
		queue->head = tsk;
		queue->tail = tsk;
		tsk->next = NULL;
		tsk->prev = NULL;
	}
	//if not empty and at tail 
	else{
		currentTCB = queue->tail;
		
		currentTCB->next = tsk;
		tsk->prev = currentTCB;
		
		tsk->next = NULL;
		queue->tail = tsk;
	}
	return 1;
}

TCB * popToEDF(readyQueue_t * queue)
{
	TCB* currentTCB = queue->head;
	if(currentTCB != NULL)
	{	
		queue->head = currentTCB->next;
		if(currentTCB->next != NULL) currentTCB->next->prev = NULL;
	}
	
	return currentTCB;
}
int isShorter(TIMEVAL current, TIMEVAL reference)
{
	if(current.sec < reference.sec)
	{
		return 1;
	}
	else if(current.sec > reference.sec)
	{
		return 0;
	}
	else{
		if(current.usec <= reference.usec)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	
	
}