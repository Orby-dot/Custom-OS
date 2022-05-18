/*
Purpose: Create print functions for bit array and 
	linked lists. Also have a function that will print
	out all the allocated memory onto UART 2
	
TO-DO: Need to make a doc for this!
*/
#include "tester.h"


void printBitArray(bitArray * array)
{
	int numOfLevels = (log_2(array->size) - log_2(MIN_SIZE));
	int numOfNodes = 1;
	
	for(int i = 0 ; i < numOfLevels;i++){
		printBitLevel(array,i,(2^i));
		
	}
}

void printBitLevel(bitArray * array,int level, int numOfNodes)
{
	int baseIndex = (1<<level)-1;
	U8 compare =0;
	for(int i = 0 ; i < numOfNodes;i++)
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