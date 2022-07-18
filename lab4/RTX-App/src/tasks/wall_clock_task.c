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


char* timeFormat(char * display_string, U32 inputTime){
	int hours = inputTime/3600;
	int minutes = inputTime/60;
	int seconds = inputTime%60;
	
	display_string[2] = ':';
	display_string[5] = ':';
	
	// display hours, minutes, and seconds but with two digits each
	// hours
	if(hours<10){
		display_string[0] = '0';
		display_string[1] = hours+'0';
	}
	else{
		display_string[0] = (hours/10)+'0';
		display_string[1] = (hours%10)+'0';
	}

	// minutes
	if(minutes<10){
		display_string[3] = '0';
		display_string[4] = minutes+'0';
	}
	else{
		display_string[3] = (minutes/10)+'0';
		display_string[4] = (minutes%10)+'0';
	}

	// seconds
	if(seconds<10){
		display_string[6] = '0';
		display_string[7] = seconds+'0';
	}
	else{
		display_string[6] = (seconds/10)+'0';
		display_string[7] = (seconds%10)+'0';
	}
	
	return display_string;	
}

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
	
	int FLAG_RemoveWallClock = 0;
	int offset = 0; // used to reset the counter since I cannot directly change the timer (I don't think I can)
	
	char* msg_buf = mem_alloc(KCD_CMD_BUF_SIZE);
	TM_TICK*time1 = mem_alloc(sizeof(TM_TICK));
	U32 seconds = 0;
	char* display = mem_alloc(8);
	while(1){
		// wait for timer1 interrupt handler and go in when called
		// recv_msg_nb(msg_buf, KCD_CMD_BUF_SIZE);
		
		if(recv_msg_nb(msg_buf, KCD_CMD_BUF_SIZE) == RTX_OK){
			RTX_MSG_HDR *header = (RTX_MSG_HDR *)msg_buf; 
			char *data = (char *)(msg_buf);
			data += 6;
			
			if(header->length>6){ // arguments have been sent back
				
				if(*data == 'R'){ // reset the wall counter
					get_tick(time1, 1);
					offset = -(time1->tc);
					timeFormat(display, 0);
				}
				else if(*data == 'T'){ // remove the wall clock display
					FLAG_RemoveWallClock = 1;
				}
				else if(*data == 'S' && header->length == 15){ // set the wall clock display time
					data+=2;
					display = data;
				}
			}
		}
		else{
			// no message received so simply increment update wall clock
			get_tick(time1,1);
			display = timeFormat(display, time1->tc+offset);
		}

		// read from timer1
		// needs to be replaced with comms to uart but that's not priority at the moment, need to first get the task working periodically
		if(!FLAG_RemoveWallClock){
			// insert code to print to uart and remove printf
			printf("\r\n%s\r\n", display);
		}
		
		rt_tsk_susp();
	}
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

