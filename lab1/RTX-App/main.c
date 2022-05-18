#include "k_inc.h"
#include "k_mem.h"
#include "lpc1768_mem.h"
#include "printf.h"
#include "LPC17xx.h"
#include "uart_def.h"
#include "uart_polling.h"

int main(){
    SystemInit();
    int *r2_start = (int*) RAM2_START;
    *r2_start = 10;
    uart0_init();
    uart1_init();
    init_printf(NULL, putc);
    
    printf("this is a string\r\n");
    printf("%d", *r2_start);
    
    return 0;
}