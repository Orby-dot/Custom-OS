/**
 * @brief The Console Display Task Template File
 * @note  The file name and the function name can be changed
 * @see   k_tasks.h
 */

#include "rtx.h"
#include "k_msg.h"
#include "common.h"
#include "uart_irq.h"
#include "uart_polling.h"

// uint8_t g_send_char;
extern uint8_t g_tx_irq;


// TODO: make it utility class function?
void printToUART(char *data, U32 data_len)
{
		char *to_send = mem_alloc(6 + data_len-1);
		RTX_MSG_HDR *header_ts = (RTX_MSG_HDR *)to_send;
		char *data_ts = (char *)(to_send);

		header_ts->length = data_len-1 + 6;
		header_ts->sender_tid = TID_CON;
		header_ts->type = DISPLAY;

		if(data_len>1) {
			data_ts += 6;
			for(int i=1;i<data_len;i++){ // skipping 1st letter
				*(data_ts+i-1) = data[i];
			}
		}

    
    LPC_UART_TypeDef *pUart;
    pUart = (LPC_UART_TypeDef *)LPC_UART0;

    // printf("IER B: %d\n\r", pUart->IER);
    while ((pUart->IER & (1 << 1))); // wait until IER_THRE is cleared
    // printf("IER A: %d\n\r", pUart->IER);
    
		send_msg_nb(TID_UART, to_send);

    pUart->THR = *data;
    g_tx_irq = 1;
    g_send_char = 0;
                // enable TX Interrupt
    pUart->IER |= IER_THRE;
		mem_dealloc(to_send);
}

void task_cdisp(void)
{
    mbx_create(CON_MBX_SIZE);

    U8 *msg_buf = mem_alloc(KCD_CMD_BUF_SIZE); // is repeatedly overwritten



    while (1)
    {

        // CALLED when something to output (character)
        recv_msg(msg_buf, KCD_CMD_BUF_SIZE);
				//printf("CONSOLE DISPLAY HAS GOT A MESSAGE!\r\n");
        RTX_MSG_HDR *header = (RTX_MSG_HDR *)msg_buf; 
        char *data = (char *)(msg_buf);
        data += 6;
			
				//printf("CDISP DATA WE GOT: %x \r\n", *data);


        // sends messsage to UART0_IRQ_Handler to output
        if (header->type == DISPLAY)
        {

            printToUART(data, header->length - 6);

        }
        else
        {
            // "Ignores the Rest"
        }
    }

   //  mem_dealloc(msg_buf); // will probably never run
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
