/**
 * @brief The Console Display Task Template File
 * @note  The file name and the function name can be changed
 * @see   k_tasks.h
 */

#include "rtx.h"

#include "k_msg.h"
#include "common.h"

// TODO: make it utility class function?
void printToUART(char *data, U32 data_len)
{
    for (int i = 0; i < data_len; i++)
    {
        char *to_send = k_mpool_alloc(MPID_IRAM2, 6 + 1);
        RTX_MSG_HDR *header_ts = (RTX_MSG_HDR *)to_send;
        char *data_ts = (char *)(to_send);
        data_ts += 6;

        header_ts->length = data_len + 6;
        header_ts->sender_tid = TID_KCD;
        header_ts->type = DISPLAY;

        data_ts = &(data[i]);

        k_send_msg(TID_CON, to_send);

        k_mpool_dealloc(MPID_IRAM2, to_send);
    }
}

void task_cdisp(void)
{
    k_mbx_create(CON_MBX_SIZE);

    U8 *msg_buf = k_mpool_alloc(MPID_IRAM2, KCD_CMD_BUF_SIZE); // is repeatedly overwritten

    LPC_UART_TypeDef *pUart;
    pUart = (LPC_UART_TypeDef *)LPC_UART0;

    while (1)
    {

        // CALLED when something to output (character)
        k_recv_msg(msg_buf, KCD_CMD_BUF_SIZE);
        RTX_MSG_HDR *header = (RTX_MSG_HDR *)msg_buf; 
        char *data = (char *)(msg_buf);
        data += 6;

        // sends messsage to UART0_IRQ_Handler to output
        if (header->type == DISPLAY)
        {

            printToUART(data, header->length - 6);
            // enable TX Interrupt
            pUart->IER |= IER_THRE;
        }
        else
        {
            // "Ignores the Rest"
        }
    }

    k_mpool_dealloc(MPID_IRAM2, msg_buf); // will probably never run
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
