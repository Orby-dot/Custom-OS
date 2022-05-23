#include "k_inc.h"
#include "k_mem.h"
#include "lpc1768_mem.h"
#include "printf.h"
#include "tester.h"
//#include "bit_array.h" uncomment this when u dont need tester.h
int main()
{
	SystemInit();
	uart0_init();
	uart1_init();
	init_printf(NULL, putc);

	printf("this is a string\r\n");
	printf("%u \r\n", (RAM2_START));
	
	k_mpool_create(BUDDY, RAM1_START, RAM1_END);	
	k_mpool_create(BUDDY, RAM2_START, RAM2_END);

	void * test = k_mpool_alloc(MPID_IRAM2, (1 << 2) - 1);

	printBitArray(&array_RAM2);
	printLinkedList(&array_RAM2);

	k_mpool_dealloc(MPID_IRAM2, test);

	printBitArray(&array_RAM2);
	printLinkedList(&array_RAM2);

	return 0;
}