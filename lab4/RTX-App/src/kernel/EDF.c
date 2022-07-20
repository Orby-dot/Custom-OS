#include "EDF.h"

int pushToEDF(readyQueue_t * queue, TCB * tsk, U8 mode)
{
	TCB* currentTCB = queue->head;
	while(currentTCB != NULL)
	{
		if(mode)
		{
			//if found a tsk with a longer period
			if(isShorter(tsk->rt_info->remainingTime,currentTCB->rt_info->remainingTime))
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
		}
		else
		{
			if(isShorter(tsk->rt_info->period,currentTCB->rt_info->period))
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

TCB * popFromEDF(readyQueue_t * readyQueuesArray)
{
	TCB* currentTCB = readyQueuesArray->head;
	if(currentTCB != NULL)
	{	
		readyQueuesArray->head = currentTCB->next;
		if(currentTCB->next != NULL)
		{
			currentTCB->next->prev = NULL;
		}
		else{
			readyQueuesArray->tail = NULL;
		}
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
		return current.usec < reference.usec;
	}
	
	
}
