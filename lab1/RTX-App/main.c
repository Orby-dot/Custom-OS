#include "k_inc.h"
#include "k_mem.h"
#include "lpc1768_mem.h"
#include "printf.h"
#include "tester.h"
//#include "bit_array.h" uncomment this when u dont need tester.h

int main(){
	
    SystemInit();
		
    bitArray* array = (bitArray*) RAM2_START;
    //*r2_start = 10;
    uart0_init();
    uart1_init();
    init_printf(NULL, putc);
    
    printf("this is a string\r\n");
		printf("%u \r\n",(RAM2_START));
		
		initializeBitArray(array, 0,1023);
		printBitArray(array);
		
    
    return 0;
}
