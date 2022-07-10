#include "k_inc.h"
#include "k_mem.h"
#include "lpc1768_mem.h"
#include "printf.h"
#include "tester.h"
//#include "bit_array.h" uncomment this when u dont need tester.h

int main(){
    
    SystemInit();
    //*r2_start = 10;
    uart0_init();
    uart1_init();
    init_printf(NULL, putc);
	
    printf("RAM Starts: %x %x \r\n", RAM1_START, RAM2_START);
	
		k_mpool_create(BUDDY, RAM1_START, RAM1_END);
    k_mpool_create(BUDDY, RAM2_START, RAM2_END);
	
    void * address = k_mpool_alloc(MPID_IRAM2, 0x8000);
	    void * address2 = k_mpool_alloc(MPID_IRAM2, 0x8000);

    printf("Returned %x \r\n", address);
	    printf("Returned %x \r\n", address2);

    
    return 0;
}
