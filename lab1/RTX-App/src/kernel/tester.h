/*
Purpose: Create print functions for bit array and 
	linked lists. Also have a function that will print
	out all the allocated memory onto UART 2
	
TO-DO: Need to make a doc for this!
*/

#include "common.h"
#include "printf.h"
#include "LPC17xx.h"
#include "uart_def.h"
#include "uart_polling.h"
#include "bit_array.h"

#define MIN_SIZE 32


void printBitArray(bitArray * array);
void printBitLevel(bitArray * array,int level, int numOfNodes, U8 index);
void printLinkedList(bitArray * array);
void printListLevel(freeList_t  list);
void printListLevelReverse(freeList_t list);



