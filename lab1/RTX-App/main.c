#include "k_inc.h"
#include "k_mem.h"
#include "lpc1768_mem.h"
#include "printf.h"
#include "tester.h"
//#include "bit_array.h" uncomment this when u dont need tester.h

// extern variables from k_mem.h
bitArray array_RAM1;
bitArray array_RAM2;
freeList_t free_list_RAM1 [8];
freeList_t free_list_RAM2 [11];

int main() {
    
    SystemInit();
    //*r2_start = 10;
    uart0_init();
    uart1_init();
    init_printf(NULL, putc);

    printf("this is a string\r\n");
    printf("%u \r\n", (RAM2_START));

    k_mpool_create(BUDDY, RAM2_START, RAM2_END);

    k_mpool_alloc(MPID_IRAM2, (1 << 2) - 1);

    printBitArray(&array_RAM2);
    printLinkedList(&array_RAM2);

    // Dealloc function mapping is correct, but dealloc is broken rn (infinite loop)
    // k_mpool_dealloc(MPID_IRAM2, (void *)(0x2007c000));
    // printBitArray(&array_RAM2);
    // printLinkedList(&array_RAM2);

    return 0;
}
