/*
Purpose: Create print functions for bit array and 
	linked lists. Also have a function that will print
	out all the allocated memory onto UART 2
	
TO-DO: Need to make a doc for this!
*/
#include "tester.h"


void printBitArray(bitArray * array)
{
	int numOfLevels = (log_2(array->size) - log_2(MIN_SIZE)+1);
	
	printf("Start: %x \r\n",(array)->startAddress);
	printf("End: %x  \r\n",(array)->endAddress);
	printf("Size: %u \r\n",(array)->size);
	printf("Num of levels: %d\r\n",numOfLevels);
	
	for(int i = 0 ; i < numOfLevels;i++){
		printBitLevel(array,i,(1<<i),((1<<i) - 1)%8);	
	}
}

void printBitLevel(bitArray * array,int level, int numOfNodes, U8 index)
{
	int baseIndex = (1<<level)-1;
	U8 compare =0;
	for(int i = index ; i < numOfNodes + index;i++)
	{
		compare = 1 <<(7 - i%8);
		if ((array->bitStatus[baseIndex/8] & compare) > 0)
		{
			printf("1 ");
		}
		else{
			printf("0 ");
		}
		baseIndex++;
		
	}
	printf("\r\n");
	
}


void printLinkedList(bitArray * array)
{
	int numOfLevels = (log_2(array->size) - log_2(MIN_SIZE) +1);

	
	for(int i = 0 ; i < numOfLevels;i++){
		printListLevel(&array->freeList[i]);
		
	}
}

void printListLevel(freeList_t * list)
{
	node_t * current = list->head;
	
	while (current != NULL){
		printf ("%u <-> ",current->tail);
	}
	
	
	printf(" NULL \r\n");
}
