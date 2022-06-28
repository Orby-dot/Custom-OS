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
    return 0;
}

int k_send_msg(task_t receiver_tid, const void *buf) {
#ifdef DEBUG_0
    printf("k_send_msg: receiver_tid = %d, buf=0x%x\r\n", receiver_tid, buf);
#endif /* DEBUG_0 */
	RTX_MSG_HDR * currentMsg = (RTX_MSG_HDR*)buf;
	
	//if msg can fit in mailbox
	if((g_tcbs[(U32)receiver_tid].mailbox.current_size + currentMsg->length) >=g_tcbs[(U32)receiver_tid].mailbox.max_size)
		{
			gp_current_task->msg = (void*)buf;
			gp_current_task->state = BLK_SEND;
			gp_current_task->destination = receiver_tid;
			addTCBtoSentBLK(readyQueuesArray, gp_current_task);
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
#ifdef DEBUG_0
    printf("k_send_msg_nb: receiver_tid = %d, buf=0x%x\r\n", receiver_tid, buf);
#endif /* DEBUG_0 */
	RTX_MSG_HDR * currentMsg = (RTX_MSG_HDR*)buf;
	
	if((g_tcbs[(U32)receiver_tid].mailbox.current_size + currentMsg->length) >=g_tcbs[(U32)receiver_tid].mailbox.max_size)
		{
			return ERROR;
			
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

int k_recv_msg(void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg: buf=0x%x, len=%d\r\n", buf, len);
#endif /* DEBUG_0 */
	void* tempMsg;
	if(gp_current_task->mailbox.current_size != 0)
	{
		tempMsg = getMessage(&gp_current_task->mailbox,len);
		if(tempMsg != NULL)
		{
			copyToBuf((U8*)buf,(U8*)tempMsg,len);
			k_mpool_dealloc(MPID_IRAM1,tempMsg);
			sendAll(gp_current_task->tid);
			addTCBtoFront(readyQueuesArray,gp_current_task->prio,gp_current_task);
			//call scheduler
			return k_tsk_run_new();
		}
		else{
			return ERROR;
		}
		
	}
	else{
		gp_current_task->state = BLK_RECV;
		addTCBtoRecvBLK(readyQueuesArray, gp_current_task);
		//call scheduler
	}
	
	
    return 0;
}

int k_recv_msg_nb(void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg_nb: buf=0x%x, len=%d\r\n", buf, len);
#endif /* DEBUG_0 */
	void* tempMsg;
	if(gp_current_task->mailbox.current_size != 0)
	{
		tempMsg = getMessage(&gp_current_task->mailbox,len);
		if(tempMsg != NULL)
		{
			copyToBuf((U8*)buf,(U8*)tempMsg,len);
			k_mpool_dealloc(MPID_IRAM1,tempMsg);
			sendAll(gp_current_task->tid);
			addTCBtoFront(readyQueuesArray,gp_current_task->prio,gp_current_task);
			//call scheduler
			return k_tsk_run_new();
		}
	}
	else{
		return -1;
	}
}

int k_mbx_ls(task_t *buf, size_t count) {
#ifdef DEBUG_0
    printf("k_mbx_ls: buf=0x%x, count=%u\r\n", buf, count);
#endif /* DEBUG_0 */
    return 0;
}

int k_mbx_get(task_t tid)
{
#ifdef DEBUG_0
    printf("k_mbx_get: tid=%u\r\n", tid);
#endif /* DEBUG_0 */
    return 0;
}
void sendAll()
{
	//loops thru the send queue and checks if we can send anything
	TCB * selectedTCB = canSendMsg(readyQueuesArray,gp_current_task->tid,gp_current_task->mailbox.current_size);
	
	while(selectedTCB != NULL)
	{
		addMessage(&gp_current_task->mailbox,selectedTCB->msg);
		selectedTCB->state = READY;
		//dealloc msg here if needed
		addTCBtoBack(readyQueuesArray,selectedTCB->prio,selectedTCB);
		
		selectedTCB = canSendMsg(readyQueuesArray,gp_current_task->tid,gp_current_task->mailbox.current_size);
	}
	
}
void copyToBuf(U8 * buf, U8 * input, U32 length)
{
	for(U32 i = 0; i < length; i++)
	{
		buf[i]= input[i];
	}
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

