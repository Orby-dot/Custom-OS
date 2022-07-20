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

void timeFormat(char * display_string, U32 inputTime){
	int hours = (inputTime/3600)%24;
	int minutes = (inputTime - (inputTime/3600)*3600)/60;
	int seconds = (inputTime - (inputTime/3600)*3600-minutes*60);
	
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
}

U32 parseTime(char *data){
	U32 seconds = 0;
	seconds = (10*(data[0]-'0')+(data[1]-'0'))*3600; // hours to seconds;
	seconds+= (10*(data[3]-'0')+(data[4]-'0'))*60; ;
	seconds+= (10*(data[6]-'0')+(data[7]-'0'))*1; ;
	return seconds;
}

void printToConsole2(char *data, U32 data_len)
{
    char *to_send = mem_alloc(6 + data_len);
    RTX_MSG_HDR *header_ts = (RTX_MSG_HDR *)to_send;
    char *data_ts = (char *)(to_send);
    data_ts += 6;

    header_ts->length = data_len + 6;
    header_ts->sender_tid = TID_KCD;
    header_ts->type = DISPLAY;

    for (int i = 0; i < data_len; i++)
    {
        *(data_ts + i) = *(data + i);
    }
    //printf("KCD SENDING TO CONSOLE LENGTH %d********* \n\r", header_ts->length);
    send_msg(TID_CON, to_send);

    mem_dealloc(to_send);
}

// Purpose: unprivileged soft real-time task that displays a digital clock on the RTX console terminal
// HH:MM:SS format
void task_wall_clock(void)
{
	// create mailbox
	// mbx_create(KCD_MBX_SIZE); // required? not sure

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
	char* display = mem_alloc(9);
	display[8] = '\0';
	while(1){
		// wait for timer1 interrupt handler and go in when called
		// recv_msg_nb(msg_buf, KCD_CMD_BUF_SIZE);
		
		if(recv_msg_nb(msg_buf, KCD_CMD_BUF_SIZE) == RTX_OK){
			RTX_MSG_HDR *header = (RTX_MSG_HDR *)msg_buf; 
			char *data = (char *)(msg_buf);
			data += 7;
			
			if(header->length>7){ // arguments have been sent back
				get_tick(time1, 1);
				
				if(*data == 'R'){ // reset the wall counter
					offset = -(time1->tc);
					timeFormat(display, 0);
				}
				else if(*data == 'T'){ // remove the wall clock display
					FLAG_RemoveWallClock = !FLAG_RemoveWallClock;
					timeFormat(display, time1->tc+offset);
				}
				else if(*data == 'S'){ // set the wall clock display time
					data+=2;
					offset = parseTime(data) - time1->tc;
					display = data;
				}
			}
		}
		else{
			// no message received so simply increment update wall clock
			get_tick(time1,1);
			timeFormat(display, time1->tc+offset);
		}

		display[8] = '\0';
		// read from timer1
		// needs to be replaced with comms to uart but that's not priority at the moment, need to first get the task working periodically
		if(!FLAG_RemoveWallClock){
			// insert code to print to uart and remove printf
			printf("\r\n%s\r\n", display);
			char *disp = mem_alloc(23);
			sprintf(disp, "\033[s\033[H\033[73G%s\033[u", display);
			// sprintf(disp, "\r%s\r", display);
			printToConsole2(disp, 23);
			mem_dealloc(disp);
		}
		
		rt_tsk_susp();
	}
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
