/**
 * @brief The Wall Clock Display Task Template File 
 * @note  The file name and the function name can be changed
 * @see   k_tasks.h
 */

#include "rtx.h"
#include "k_msg.h"
#include "uart_irq.h"
#include "uart_polling.h"
#include "timer.h"

// Purpose: unprivileged soft real-time task that displays a digital clock on the RTX console terminal
// HH:MM:SS format
void task_wall_clock(void)
{
	// create mailbox
	mbx_create(KCD_MBX_SIZE); // required? not sure

	// register commands with KCD task by sending to mailbox
	// WR - wall clock reset
	// WS - wall clock set (pass in HH:MM:SS as arg)
	// WT - wall clock remove from console

	char* buf = mem_alloc(8);
	RTX_MSG_HDR *header = (void*) buf;
	header->length = 6+1;
	header->type = KCD_REG;
	header->sender_tid = TID_WCLCK;
	buf+=6;
	*buf = 'W';
	send_msg_nb(TID_KCD, (void*)header);
	
	char* msg_buf = mem_alloc(KCD_CMD_BUF_SIZE);
	while(1){
		// wait for timer1 interrupt handler and go in when called
		recv_msg_nb(msg_buf, KCD_CMD_BUF_SIZE);
		
		// read from timer1
		TM_TICK*tc;
		get_tick(tc, 1);
		printf("%d\n", tc->tc);
		tsk_yield();
	}
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

