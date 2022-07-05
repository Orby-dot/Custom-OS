/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTX LAB  
 *
 *                     Copyright 2020-2022 Yiqing Huang
 *                          All rights reserved.
 *---------------------------------------------------------------------------
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice and the following disclaimer.
 *
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
 

/**************************************************************************//**
 * @file        k_msg.c
 * @brief       kernel message passing routines          
 * @version     V1.2021.06
 * @authors     Yiqing Huang
 * @date        2021 JUN
 *****************************************************************************/

#include "k_inc.h"
#include "k_rtx.h"
#include "k_msg.h"


int k_mbx_create(size_t size) {
#ifdef DEBUG_0
    printf("k_mbx_create: size = %u\r\n", size);
#endif /* DEBUG_0 */
	//i set the head of the mailbox to null when we first create the task
	if(gp_current_task->mailbox.head != NULL)
	{
		return -1;
	}
	initializeMailbox(&gp_current_task->mailbox, gp_current_task->tid,size);
	return 0;
}

int k_send_msg(task_t receiver_tid, const void *buf) {
    // printf("k_send_msg: receiver_tid = %d, buf=0x%x\r\n", receiver_tid, buf);
	RTX_MSG_HDR * currentMsg = (RTX_MSG_HDR*)buf;
	//if msg can fit in mailbox

	if((g_tcbs[(U32)receiver_tid].mailbox.current_size + currentMsg->length) >g_tcbs[(U32)receiver_tid].mailbox.max_size)
	{
		gp_current_task->msg = (void*)buf;
		gp_current_task->state = BLK_SEND;
		gp_current_task->destination = receiver_tid;
		addTCBtoBack(sendQueuesArray, gp_current_task->prio,gp_current_task);
		//call scheduler
		return k_tsk_run_new();
		
	}
	else{
		TCB * selectedTCB = &g_tcbs[(U32) receiver_tid];
		addMessage(&selectedTCB->mailbox,(void*)buf);
		if(selectedTCB->state == BLK_RECV)
		{
			selectedTCB->state = READY;
			
			removeSpecificTCB(readyQueuesArray,RECV_PRIO,receiver_tid);
			addTCBtoBack(readyQueuesArray,selectedTCB->prio,selectedTCB);
			
			addTCBtoFront(readyQueuesArray,gp_current_task->prio,gp_current_task);
			
			return k_tsk_run_new();
		}
	}
	return 0;
}

int k_send_msg_nb(task_t receiver_tid, const void *buf) {
    // printf("SSSS k_send_msg_nb: receiver_tid = %d, buf=0x%x\r\n", receiver_tid, buf);
	RTX_MSG_HDR * currentMsg = (RTX_MSG_HDR*)buf;
	
	if(receiver_tid != TID_UART && (g_tcbs[(U32)receiver_tid].mailbox.current_size + currentMsg->length) >=g_tcbs[(U32)receiver_tid].mailbox.max_size){
		return RTX_ERR;		
	}
	else if(receiver_tid == TID_UART && (uart_mailbox->current_size + currentMsg->length) >= uart_mailbox->max_size){
		return RTX_ERR;
	}
	else if(receiver_tid == TID_UART){
		addMessage(uart_mailbox,(void*)buf);
	}
	else{
		TCB * selectedTCB = &g_tcbs[(U32) receiver_tid];
		addMessage(&(selectedTCB->mailbox),(void*)buf);
		// printf("======== %x %x \r\n", selectedTCB->state, BLK_RECV);
		if((selectedTCB->state) == BLK_RECV)
		{
			selectedTCB->state = READY;
			removeSpecificTCB(readyQueuesArray,RECV_PRIO,receiver_tid);
			addTCBtoBack(readyQueuesArray,selectedTCB->prio,selectedTCB);
			addTCBtoFront(readyQueuesArray,gp_current_task->prio,gp_current_task);
			return k_tsk_run_new();
		}
	}
		
    return 0;
}

int k_recv_msg(void *buf, size_t len) {
	// printf("k_recv_msg: buf=0x%x, len=%d\r\n", buf, len);
	sendAll();
	if(gp_current_task->mailbox.current_size != 0)
	{
		if(getMessage(&gp_current_task->mailbox,buf,len) == RTX_OK)
		{
			sendAll();
			addTCBtoFront(readyQueuesArray,gp_current_task->prio,gp_current_task);
			//call scheduler
			return k_tsk_run_new();
		}
		else{
			return RTX_ERR;
		}
		
	}
	else{
		gp_current_task->state = BLK_RECV;
		addTCBtoRecvBLK(readyQueuesArray, gp_current_task);
		//call scheduler
		k_tsk_run_new();
		getMessage(&gp_current_task->mailbox,buf,len);
		sendAll();
		addTCBtoFront(readyQueuesArray,gp_current_task->prio,gp_current_task);
		return k_tsk_run_new();;
	}
}

int k_recv_msg_nb_uart(void *buf, size_t len) {
	// printf("k_recv_msg_nb: buf=0x%x, len=%d\r\n", buf, len);
	if (buf == NULL ){ // buf is null
		errno = EFAULT;
		return RTX_ERR;
	}
	else if(uart_mailbox->ring_buffer == NULL){ // no mailbox
		errno = ENOENT;		
		return RTX_ERR;
	}
	else if(uart_mailbox->current_size == 0){ // mailbox has no message
		errno = ENOMSG;
		return RTX_ERR;
	}

	if(getMessage(uart_mailbox,buf,len) == RTX_OK)
	{
		// do nothing
		return RTX_OK;
	}
	else{
		errno = ENOSPC;
		return RTX_ERR;
	}

}

int k_recv_msg_nb(void *buf, size_t len) {
	// printf("k_recv_msg_nb: buf=0x%x, len=%d\r\n", buf, len);
	if (buf == NULL ){ // buf is null
		errno = EFAULT;
		return RTX_ERR;
	}
	else if(gp_current_task->mailbox.ring_buffer == NULL){ // no mailbox
		errno = ENOENT;		
		return RTX_ERR;
	}
	else if(gp_current_task->mailbox.current_size == 0){ // mailbox has no message
		errno = ENOMSG;
		return RTX_ERR;
	}

	if(getMessage(&gp_current_task->mailbox,buf,len) == RTX_OK)
	{
		sendAll();
		addTCBtoFront(readyQueuesArray,gp_current_task->prio,gp_current_task);
		//call scheduler
		return k_tsk_run_new();
	}
	else{
		errno = ENOSPC;
		return RTX_ERR;
	}

}

int k_mbx_ls(task_t *buf, size_t count) {
	U8 currentNumTCB =0;
	for(U8 i =0; i < MAX_TASKS; i++)
	{
		if(g_tcbs[i].state != DORMANT)
		{
			if(g_tcbs[i].mailbox.head != NULL)
			{
				buf[currentNumTCB] = g_tcbs[i].tid;
				currentNumTCB ++;
				if(currentNumTCB == count)
				{
					return currentNumTCB;
				}
			}
		}
	}
   
	return currentNumTCB;
}

int k_mbx_get(task_t tid)
{
#ifdef DEBUG_0
    printf("k_mbx_get: tid=%u\r\n", tid);
#endif /* DEBUG_0 */
		return g_tcbs[(U32)tid].mailbox.max_size - g_tcbs[(U32)tid].mailbox.current_size;
}
void sendAll(void)
{
	//loops thru the send queue and checks if we can send anything
	TCB * selectedTCB = sendQScheduler();
	
	while(selectedTCB != NULL)
	{
		addMessage(&gp_current_task->mailbox,selectedTCB->msg);
		selectedTCB->state = READY;
		//dealloc msg here if needed
		addTCBtoBack(readyQueuesArray,selectedTCB->prio,selectedTCB);
		
		selectedTCB = sendQScheduler();
	}
	
}

TCB *sendQScheduler(void)
{
    //task_t tid = gp_current_task->tid;
    //return &g_tcbs[(++tid)%g_num_active_tasks];
	task_t target = gp_current_task->tid;
	U32 availableSize = gp_current_task->mailbox.max_size-gp_current_task->mailbox.current_size;
	for (U8 i = 0; i < 5 ;i++) {
		if (sendQueuesArray[i].head){
			
			TCB * selectedTCB = canSendMsg(sendQueuesArray,target,availableSize,i );
			if(selectedTCB != NULL)
			{
				return selectedTCB;
			}
				
		}
	}
	return NULL;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

