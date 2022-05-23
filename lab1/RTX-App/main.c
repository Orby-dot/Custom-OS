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
    printf("RAM Start: %x \r\n", (RAM2_START));

    initializeBitArray(&array_RAM1, (freeList_t *)free_list_RAM1, bitarray_RAM1, RAM1_START, RAM1_END);
		initializeBitArray(&array_RAM2, (freeList_t *)free_list_RAM2, bitarray_RAM2, RAM2_START, RAM2_END);

	
    printBitArray(&array_RAM1);
    printLinkedList(&array_RAM1);

    U32 address = allocateNode(&array_RAM1, (1 << 4) + 1);
 
    printBitArray(&array_RAM1);
    printLinkedList(&array_RAM1);
    
    removeNodes(&array_RAM1, address);
    
    printBitArray(&array_RAM1);
    printLinkedList(&array_RAM1);

    // printf("-----------------------------------\r\n");

    //     printLinkedList(&array_RAM2);

    // U32 address2 = allocateNode(&array_RAM2, (1 << 4) + 1);
 
    // printBitArray(&array_RAM2);
    // printLinkedList(&array_RAM2);
    
    // removeNodes(&array_RAM2, address);
    
    // printBitArray(&array_RAM2);
    // printLinkedList(&array_RAM2);

    return 0;
}
