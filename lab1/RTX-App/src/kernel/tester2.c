#include "ready_queue.h"

#define QUEUE_LENGTH 4
void printReadyQueue(readyQueue_t * queue)
{
	for(int i = 0 ; i < QUEUE_LENGTH;i++)
	{
		printLinkList(queue[i]);
		
	}
}
void printLinkList(readyQueue list)
{
	TCB * current = list.head;
	
	while (current != NULL)
	{
		printf("ID: %u, ",current->tid);
		current = current->next
		
	}
	printf("\r\n");
	
}