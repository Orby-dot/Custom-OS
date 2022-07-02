/**
 * @brief The Console Display Task Template File 
 * @note  The file name and the function name can be changed
 * @see   k_tasks.h
 */

#include "rtx.h"

#include "k_msg.h"
#include "common.h"
#include "message_node.h"

// TODO: make it utility class function?
void printToUART(char *data, U32 data_len) {
    for (int i = 0; i < data_len; i++) {
        msg_node *to_send = k_mpool_alloc(MPID_IRAM2, 6 + 1); // 1 for character to echo
        to_send->header->length = 1 + 6;
        to_send->header->sender_tid = TID_CON;
        to_send->header->type = DISPLAY;

        to_send->data = &(data[i]);

        k_send_msg(TID_UART, to_send);

        k_mpool_dealloc(MPID_IRAM2, to_send);
    }
}

void task_cdisp(void)
{
    k_mbx_create(CON_MBX_SIZE);

    U8 *msg_buf = k_mpool_alloc(MPID_IRAM2, KCD_CMD_BUF_SIZE); // is repeatedly overwritten
    msg_node *tmp_msg;

    
    while(1) {

        // CALLED when something to output (character)
        k_recv_msg(msg_buf, KCD_CMD_BUF_SIZE);
        tmp_msg = (msg_node *)msg_buf;

        // sends messsage to UART0_IRQ_Handler to output
        if (tmp_msg->header->type == DISPLAY) {

            printToUART(tmp_msg->data, tmp_msg->header->length - 6 );
            // enable TX Interrupt
            pUart->IER |= IER_THRE;
        } else {
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

