#include "tester2.h"
#define QUEUE_LENGTH 4
void printReadyQueue(readyQueue_t * queue)
{
	for(int i = 0 ; i < QUEUE_LENGTH;i++)
	{
		printLinkList(queue[i]);
		
	}
}
void printLinkList(readyQueue_t list)
{
	TCB * current = list.head;
	
	while (current != NULL)
	{
		printf("ID: %u, ",current->tid);
		current = current->next;
		
	}
	printf("\r\n");
	
}

void printTCB()
{
	for(int i= 0 ; i < MAX_TASKS;i++)
	{
		printf("TCB #%u \r\n msp: %u \r\n tid:%u \r\n psp:%u \r\n state:%u \r\n",
		i,g_tcbs[i].msp,g_tcbs[i].tid,g_tcbs[i].psp,g_tcbs[i].state);
		
	}
}

void printKernelStack()
{
	for(int i= 0 ; i < MAX_TASKS;i++)
	{
		printf("%u \r\n",g_k_stacks[i]);
		
	}
	
	
}