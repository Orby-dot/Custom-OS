#include "k_inc.h"
#include "k_mem.h"
#include "lpc1768_mem.h"
#include "printf.h"
#include "tester.h"
//#include "bit_array.h" uncomment this when u dont need tester.h
bitArray array;
freeList_t list [11];
int main(){
	
    SystemInit();
    //*r2_start = 10;
    uart0_init();
    uart1_init();
    init_printf(NULL, putc);
    
    printf("this is a string\r\n");
		printf("%u \r\n",(RAM2_START));
		
		initializeBitArray(&array,(freeList_t *)list, RAM2_START,RAM2_END);
		// printBitArray(&array);
		printLinkedList(&array);

    allocateNode(&array, (1 << 13) - 1);
		
    // printBitArray(&array);
		printLinkedList(&array);
    
    return 0;
}
