#include "k_inc.h"
#include "k_mem.h"
#include "lpc1768_mem.h"
#include "printf.h"
#include "tester.h"
//#include "bit_array.h" uncomment this when u dont need tester.h
// bitArray array;
// U8 bitarray_RAM2_m[((2*(RAM2_SIZE)/32 -1)/8)];

// freeList_t list [11];

int main(){
	
    SystemInit();
    //*r2_start = 10;
    uart0_init();
    uart1_init();
    init_printf(NULL, putc);

	printf("this is a string\r\n");
	printf("%u \r\n", (RAM2_START));

	initializeBitArray(&array_RAM2, (freeList_t *)free_list_RAM2, bitarray_RAM2, RAM2_START, RAM2_END);
	// printBitArray(&array);
	printLinkedList(&array_RAM2);

	/*    U32 address = allocateNode(&array, (1 << 13) + 1);
    U32 address2 = allocateNode(&array, (1 << 12) + 1);
		
    printBitArray(&array);
		printLinkedList(&array);
	
		removeNodes(&array, address);

    printBitArray(&array);
		printLinkedList(&array);
		
		removeNodes(&array, address2);

    printBitArray(&array);
		printLinkedList(&array);
		*/

	U32 address = allocateNode(&array_RAM2, (1 << 4) + 1);
	U32 address2 = allocateNode(&array_RAM2, (1 << 4) + 1);
	 U32 address3 = allocateNode(&array_RAM2, (1 << 4) + 1);
	 U32 address4 = allocateNode(&array_RAM2, (1 << 4) + 1);
	 
	printBitArray(&array_RAM2);
	printLinkedList(&array_RAM2);
	
	removeNodes(&array_RAM2, address);
	removeNodes(&array_RAM2, address2);
	removeNodes(&array_RAM2, address3);
	removeNodes(&array_RAM2, address4);
	
	printBitArray(&array_RAM2);
	printLinkedList(&array_RAM2);
	
	
	
	//removeNodes(&array, address);
	//printBitArray(&array);
	//printLinkedList(&array);
	// removeNodes(&array, address4);
	// printLinkedList(&array);
	// removeNodes(&array, address3);
	// // printBitArray(&array);
	// printLinkedList(&array);


	return 0;
}
