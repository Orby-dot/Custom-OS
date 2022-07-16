/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2022 Yiqing Huang
 *                          All rights reserved.
 *
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
 ****************************************************************************
 */

/**************************************************************************//**
 * @file        k_task.c
 * @brief       task management C file
 * @version     V1.2021.05
 * @authors     Yiqing Huang
 * @date        2021 MAY
 *
 * @attention   assumes NO HARDWARE INTERRUPTS
 * @details     The starter code shows one way of implementing context switching.
 *              The code only has minimal sanity check.
 *              There is no stack overflow check.
 *              The implementation assumes only three simple tasks and
 *              NO HARDWARE INTERRUPTS.
 *              The purpose is to show how context switch could be done
 *              under stated assumptions.
 *              These assumptions are not true in the required RTX Project!!!
 *              Understand the assumptions and the limitations of the code before
 *              using the code piece in your own project!!!
 *
 *****************************************************************************/


#include "k_inc.h"
#include "k_task.h"
#include "k_rtx.h"
#include "ready_queue.h"
#include "k_mem.h"
#include "EDF.h"
// #include "tester2.h"
/*
 *==========================================================================
 *                            GLOBAL VARIABLES
 *==========================================================================
 */

TCB             *gp_current_task = NULL;    // the current RUNNING task
TCB             g_tcbs[MAX_TASKS];          // an array of TCBs
//TASK_INIT       g_null_task_info;           // The null task info
U32             g_num_active_tasks = 0;     // number of non-dormant tasks

/*---------------------------------------------------------------------------
The memory map of the OS image may look like the following:
                   RAM1_END-->+---------------------------+ High Address
                              |                           |
                              |                           |
                              |       MPID_IRAM1          |
                              |   (for user space heap  ) |
                              |                           |
                 RAM1_START-->|---------------------------|
                              |                           |
                              |  unmanaged free space     |
                              |                           |
&Image$$RW_IRAM1$$ZI$$Limit-->|---------------------------|-----+-----
                              |         ......            |     ^
                              |---------------------------|     |
                              |      PROC_STACK_SIZE      |  OS Image
              g_p_stacks[2]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[1]-->|---------------------------|     |
                              |      PROC_STACK_SIZE      |     |
              g_p_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |                           |  OS Image
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |     |                
             g_k_stacks[15]-->|---------------------------|     |
                              |                           |     |
                              |     other kernel stacks   |     |                              
                              |---------------------------|     |
                              |      KERN_STACK_SIZE      |  OS Image
              g_k_stacks[2]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |                      
              g_k_stacks[1]-->|---------------------------|     |
                              |      KERN_STACK_SIZE      |     |
              g_k_stacks[0]-->|---------------------------|     |
                              |   other  global vars      |     |
                              |---------------------------|     |
                              |        TCBs               |  OS Image
                      g_tcbs->|---------------------------|     |
                              |        global vars        |     |
                              |---------------------------|     |
                              |                           |     |          
                              |                           |     |
                              |        Code + RO          |     |
                              |                           |     V
                 IRAM1_BASE-->+---------------------------+ Low Address
    
---------------------------------------------------------------------------*/ 

BOOL debugEntryPrint = FALSE;



/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */


/**************************************************************************//**
 * @brief   scheduler, pick the TCB of the next to run task
 *
 * @return  TCB pointer of the next to run task
 * @post    gp_curret_task is updated
 * @note    you need to change this one to be a priority scheduler
 *
 *****************************************************************************/

TCB *scheduler(void)
{
    /* *****MODIFY THIS FUNCTION ***** */
    //task_t tid = gp_current_task->tid;
    //return &g_tcbs[(++tid)%g_num_active_tasks];
	TCB * selectedTCB = NULL;
	
	if(readyQueuesArray[0].head){
		selectedTCB = popFromEDF(readyQueuesArray);
	}
	
	for (U8 i = 1; i < 5 ;i++) {
		if (readyQueuesArray[i].head){
			selectedTCB = removeTCB(readyQueuesArray, i);
		}
	}
	
	if(selectedTCB){
		selectedTCB->state = RUNNING;
		return selectedTCB;
	}
	
	return &g_tcbs[0]; // null task
}

/**
 * @brief initialize the first task in the system
 */
void k_tsk_init_first(TASK_INIT *p_task)
{
    p_task->prio         = PRIO_NULL;
    p_task->priv         = 0;
    p_task->tid          = TID_NULL;
    p_task->ptask        = &task_null;
    p_task->u_stack_size = PROC_STACK_SIZE;
}

/**************************************************************************//**
 * @brief       initialize all boot-time tasks in the system,
 *
 *
 * @return      RTX_OK on success; RTX_ERR on failure
 * @param       task_info   boot-time task information structure pointer
 * @param       num_tasks   boot-time number of tasks
 * @pre         memory has been properly initialized
 * @post        none
 * @see         k_tsk_create_first
 * @see         k_tsk_create_new
 *****************************************************************************/

int k_tsk_init(TASK_INIT *task, int num_tasks)
{
    if (num_tasks > MAX_TASKS - 1) {
        return RTX_ERR;
    }
    
    TASK_INIT taskinfo;

		for(int i=0;i<MAX_TASKS;i++){
			//g_tcbs[i].msp = (U32*)&(g_k_stacks[i]);
			g_tcbs[i].tid = i;
			g_tcbs[i].initialized = 0;
		}
    
    k_tsk_init_first(&taskinfo);
    if ( k_tsk_create_new(&taskinfo, &g_tcbs[TID_NULL], TID_NULL) == RTX_OK ) {
        g_num_active_tasks = 1;
        gp_current_task = &g_tcbs[TID_NULL];
    } else {
        g_num_active_tasks = 0;
        return RTX_ERR;
    }
    
    // create the rest of the tasks
    for ( int i = 0; i < num_tasks; i++ ) {
        TCB *p_tcb = &g_tcbs[i+1];
        if (k_tsk_create_new(&task[i], p_tcb, i+1) == RTX_OK) {
            g_num_active_tasks++;
        }
    }
    
    return RTX_OK;
}
/**************************************************************************//**
 * @brief       initialize a new task in the system,
 *              one dummy kernel stack frame, one dummy user stack frame
 *
 * @return      RTX_OK on success; RTX_ERR on failure
 * @param       p_taskinfo  task initialization structure pointer
 * @param       p_tcb       the tcb the task is assigned to
 * @param       tid         the tid the task is assigned to
 *
 * @details     From bottom of the stack,
 *              we have user initial context (xPSR, PC, SP_USR, uR0-uR3)
 *              then we stack up the kernel initial context (kLR, kR4-kR12, PSP, CONTROL)
 *              The PC is the entry point of the user task
 *              The kLR is set to SVC_RESTORE
 *              20 registers in total
 * @note        YOU NEED TO MODIFY THIS FILE!!!
 *****************************************************************************/
int k_tsk_create_new(TASK_INIT *p_taskinfo, TCB *p_tcb, task_t tid)
{
    extern U32 SVC_RTE;

    U32 *usp;
    U32 *ksp;

    if (p_taskinfo == NULL || p_tcb == NULL)
    {
				errno = EINVAL;
        return RTX_ERR;
    }

    p_tcb->tid   = tid;
    p_tcb->state = READY;
    p_tcb->prio  = p_taskinfo->prio;
    p_tcb->priv  = p_taskinfo->priv;
		p_tcb->mailbox.head = NULL;
    
    /*---------------------------------------------------------------
     *  Step1: allocate user stack for the task
     *         stacks grows down, stack base is at the high address
     * ATTENTION: you need to modify the following three lines of code
     *            so that you use your own dynamic memory allocator
     *            to allocate variable size user stack.
     * -------------------------------------------------------------*/
		
		U32 sizeToAllocate = p_taskinfo->u_stack_size>PROC_STACK_SIZE ? p_taskinfo->u_stack_size : PROC_STACK_SIZE;
    usp = (U32*)(k_mpool_alloc(MPID_IRAM2, sizeToAllocate));
    if (usp == NULL) {
				errno = ENOMEM;
        return RTX_ERR;
    }
		p_tcb->psp_base = usp;
		p_tcb->psp_stack_size = sizeToAllocate;
		p_tcb->entry_point = p_taskinfo->ptask;

    /*-------------------------------------------------------------------
     *  Step2: create task's thread mode initial context on the user stack.
     *         fabricate the stack so that the stack looks like that
     *         task executed and entered kernel from the SVC handler
     *         hence had the exception stack frame saved on the user stack.
     *         This fabrication allows the task to return
     *         to SVC_Handler before its execution.
     *
     *         8 registers listed in push order
     *         <xPSR, PC, uLR, uR12, uR3, uR2, uR1, uR0>
     * -------------------------------------------------------------*/

    // if kernel task runs under SVC mode, then no need to create user context stack frame for SVC handler entering
    // since we never enter from SVC handler in this case
    
    *(--usp) = INITIAL_xPSR;             // xPSR: Initial Processor State
    *(--usp) = (U32) (p_taskinfo->ptask);// PC: task entry point
        
    // uR14(LR), uR12, uR3, uR3, uR1, uR0, 6 registers
    for ( int j = 0; j < 6; j++ ) {
        
#ifdef DEBUG_0
        *(--usp) = 0xDEADAAA0 + j;
#else
        *(--usp) = 0x0;
#endif
    }
    
    // allocate kernel stack for the task
    ksp = (U32*)(k_mpool_alloc(MPID_IRAM2, KERN_STACK_SIZE));
    if ( ksp == NULL ) {
				errno = ENOMEM;
        return RTX_ERR;
    }
		p_tcb->msp_base = ksp;
		p_tcb->msp_stack_size = KERN_STACK_SIZE;

    /*---------------------------------------------------------------
     *  Step3: create task kernel initial context on kernel stack
     *
     *         12 registers listed in push order
     *         <kLR, kR4-kR12, PSP, CONTROL>
     * -------------------------------------------------------------*/
    // a task never run before directly exit
    *(--ksp) = (U32) (&SVC_RTE);
    // kernel stack R4 - R12, 9 registers
#define NUM_REGS 9    // number of registers to push
      for ( int j = 0; j < NUM_REGS; j++) {        
#ifdef DEBUG_0
        *(--ksp) = 0xDEADCCC0 + j;
#else
        *(--ksp) = 0x0;
#endif
    }
        
    // put user sp on to the kernel stack
    *(--ksp) = (U32) usp;
    
    // save control register so that we return with correct access level
    if (p_taskinfo->priv == 1) {  // privileged 
        *(--ksp) = __get_CONTROL() & ~BIT(0); 
    } else {                      // unprivileged
        *(--ksp) = __get_CONTROL() | BIT(0);
    }

    p_tcb->msp = ksp;
		p_tcb->psp = usp;
		if(tid != TID_NULL) addTCBtoBack(readyQueuesArray,p_tcb->prio,p_tcb);
		p_tcb->initialized = 1;

    return RTX_OK;
}

/**************************************************************************//**
 * @brief       switching kernel stacks of two TCBs
 * @param       p_tcb_old, the old tcb that was in RUNNING
 * @return      RTX_OK upon success
 *              RTX_ERR upon failure
 * @pre         gp_current_task is pointing to a valid TCB
 *              gp_current_task->state = RUNNING
 *              gp_crrent_task != p_tcb_old
 *              p_tcb_old == NULL or p_tcb_old->state updated
 * @note        caller must ensure the pre-conditions are met before calling.
 *              the function does not check the pre-condition!
 * @note        The control register setting will be done by the caller
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * @attention   CRITICAL SECTION
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *****************************************************************************/
__asm void k_tsk_switch(TCB *p_tcb_old)
{
        PRESERVE8
        EXPORT  K_RESTORE
        
        PUSH    {R4-R12, LR}                // save general pupose registers and return address
        MRS     R4, CONTROL                 
        MRS     R5, PSP
        PUSH    {R4-R5}                     // save CONTROL, PSP
        STR     SP, [R0, #TCB_MSP_OFFSET]   // save SP to p_old_tcb->msp
K_RESTORE
        LDR     R1, =__cpp(&gp_current_task)
        LDR     R2, [R1]
        LDR     SP, [R2, #TCB_MSP_OFFSET]   // restore msp of the gp_current_task
        POP     {R4-R5}
        MSR     PSP, R5                     // restore PSP
        MSR     CONTROL, R4                 // restore CONTROL
        ISB                                 // flush pipeline, not needed for CM3 (architectural recommendation)
        POP     {R4-R12, PC}                // restore general purpose registers and return address
}


__asm void k_tsk_start(void)
{
        PRESERVE8
        B K_RESTORE
}

/**************************************************************************//**
 * @brief       run a new thread. The caller becomes READY and
 *              the scheduler picks the next ready to run task.
 * @return      RTX_ERR on error and zero on success
 * @pre         gp_current_task != NULL && gp_current_task == RUNNING
 * @post        gp_current_task gets updated to next to run task
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * @attention   CRITICAL SECTION
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *****************************************************************************/
int k_tsk_run_new(void)
{
    TCB *p_tcb_old = NULL;
    
    if (gp_current_task == NULL) {
        return RTX_ERR;
    }

    p_tcb_old = gp_current_task;
    gp_current_task = scheduler();
    // printf(" ########## NOW RUNNING %u FROM %u\r\n", gp_current_task->tid, p_tcb_old->tid);
    if ( gp_current_task == NULL  ) {
        gp_current_task = p_tcb_old;        // revert back to the old task
        return RTX_ERR;
    }

    // at this point, gp_current_task != NULL and p_tcb_old != NULL
    if (gp_current_task != p_tcb_old) {
        gp_current_task->state = RUNNING;   // change state of the to-be-switched-in  tcb
        if (p_tcb_old->state != DORMANT && p_tcb_old->state != BLK_RECV && p_tcb_old->state != BLK_SEND) p_tcb_old->state = READY;           // change state of the to-be-switched-out tcb
				k_tsk_switch(p_tcb_old);            // switch kernel stacks       
    }

    return RTX_OK;
}

 
/**************************************************************************//**
 * @brief       yield the cpu
 * @return:     RTX_OK upon success
 *              RTX_ERR upon failure
 * @pre:        gp_current_task != NULL &&
 *              gp_current_task->state = RUNNING
 * @post        gp_current_task gets updated to next to run task
 * @note:       caller must ensure the pre-conditions before calling.
 *****************************************************************************/
int k_tsk_yield(void)
{
    if (debugEntryPrint) printf("===== k_tsk_yield is called =====\r\n");
	addTCBtoBack(readyQueuesArray,gp_current_task->prio,gp_current_task); // TODO: should we be passing pointer instead of value of gp_current_task?
	gp_current_task->state = READY;
	return k_tsk_run_new();
}

void k_tsk_run_new_put_current_task_to_rq(void) 
{
#ifdef DEBUG_0
    printf("k_tsk_run_new_put_current_task_to_rq: entering...\n\r");
#endif /* DEBUG_0 */
	addTCBtoBack(readyQueuesArray,gp_current_task->prio,gp_current_task);
	k_tsk_run_new();
    return;
}

/**
 * @brief   get task identification
 * @return  the task ID (TID) of the calling task
 */
task_t k_tsk_gettid(void)
{
    return gp_current_task->tid;
}

/*
 *===========================================================================
 *                             TO BE IMPLEMETED IN LAB2
 *===========================================================================
 */


/**
* 
*/
int k_tsk_create(task_t *task, void (*task_entry)(void), U8 prio, U32 stack_size)
{
	if (prio > LOWEST || prio < HIGH)
	{
		errno = EINVAL;
		return RTX_ERR;
	}
#ifdef DEBUG_0
    printf("k_tsk_create: entering...\n\r");
    printf("task = 0x%x, task_entry = 0x%x, prio=%d, stack_size = %d\n\r", task, task_entry, prio, stack_size);
#endif /* DEBUG_0 */
	
    extern U32 SVC_RTE;

    U32 *usp;
    U32 *ksp;
	
		//get empty TCB
	
		TCB *freeTCB;
		freeTCB->priv = 2; // privilege can only be 0 or 1
		for(int i=0;i<MAX_TASKS;i++){
			if(g_tcbs[i].state == DORMANT){
				freeTCB = &g_tcbs[i];
				break;
			}
		}
		if (freeTCB->priv == 2){
			errno = EAGAIN;
			return RTX_ERR; // look into correct return
		}
		
		*task=freeTCB->tid;
		freeTCB->state = READY;
		freeTCB->prio = prio;
		freeTCB->priv = 0;
		freeTCB->mailbox.head = NULL;

    /*---------------------------------------------------------------
     *  Step1: allocate user stack for the task
     *         stacks grows down, stack base is at the high address
     * ATTENTION: you need to modify the following three lines of code
     *            so that you use your own dynamic memory allocator
     *            to allocate variable size user stack.
     * -------------------------------------------------------------*/
		U32 sizeToAllocate = stack_size>PROC_STACK_SIZE ? stack_size : PROC_STACK_SIZE;
		usp = (U32*)(k_mpool_alloc(MPID_IRAM2, sizeToAllocate));
    if (usp == NULL) {
			errno = ENOMEM;
        return RTX_ERR;
    }
		freeTCB->psp_base = usp;
		freeTCB->psp_stack_size = sizeToAllocate;

    /*-------------------------------------------------------------------
     *  Step2: create task's thread mode initial context on the user stack.
     *         fabricate the stack so that the stack looks like that
     *         task executed and entered kernel from the SVC handler
     *         hence had the exception stack frame saved on the user stack.
     *         This fabrication allows the task to return
     *         to SVC_Handler before its execution.
     *
     *         8 registers listed in push order
     *         <xPSR, PC, uLR, uR12, uR3, uR2, uR1, uR0>
     * -------------------------------------------------------------*/

    // if kernel task runs under SVC mode, then no need to create user context stack frame for SVC handler entering
    // since we never enter from SVC handler in this case
    
    *(--usp) = INITIAL_xPSR;             // xPSR: Initial Processor State
    *(--usp) = (U32) (task_entry);// PC: task entry point
        
    // uR14(LR), uR12, uR3, uR3, uR1, uR0, 6 registers
    for ( int j = 0; j < 6; j++ ) {
        
#ifdef DEBUG_0
        *(--usp) = 0xDEADAAA0 + j;
#else
        *(--usp) = 0x0;
#endif
    }
		
		freeTCB->psp = usp; //probable bug
    
    // get kernel sp which should be initialized in freeTCB
		if(freeTCB->initialized){
			ksp = freeTCB->msp_base;		
		}
		else{
			ksp = (U32*)(k_mpool_alloc(MPID_IRAM2, KERN_STACK_SIZE));
			freeTCB->msp_base = ksp;
		}
		if ( ksp == NULL ) {
				errno = ENOMEM;
				return RTX_ERR;
		}		
		
		freeTCB->entry_point = task_entry;

    /*---------------------------------------------------------------
     *  Step3: create task kernel initial context on kernel stack
     *
     *         12 registers listed in push order
     *         <kLR, kR4-kR12, PSP, CONTROL>
     * -------------------------------------------------------------*/
    // a task never run before directly exit
    *(--ksp) = (U32) (&SVC_RTE);
    // kernel stack R4 - R12, 9 registers
#define NUM_REGS 9    // number of registers to push
      for ( int j = 0; j < NUM_REGS; j++) {        
#ifdef DEBUG_0
        *(--ksp) = 0xDEADCCC0 + j;
#else
        *(--ksp) = 0x0;
#endif
    }
        
    // put user sp on to the kernel stack
    *(--ksp) = (U32) usp;
    
    // save control register so that we return with correct access level
		*(--ksp) = __get_CONTROL() | BIT(0);
		
		addTCBtoBack(readyQueuesArray,freeTCB->prio,freeTCB); // TODO: should we be passing pointer instead of value of freeTCB?
				    
		freeTCB->msp = ksp;

		freeTCB->initialized = 1;
		addTCBtoFront(readyQueuesArray,gp_current_task->prio,gp_current_task);
		gp_current_task->state = READY;
    return k_tsk_run_new();

}

void k_tsk_exit(void) 
{
#ifdef DEBUG_0
    printf("k_tsk_exit: entering...\n\r");
#endif /* DEBUG_0 */
    if (debugEntryPrint) printf("===== task exit =====\r\n");
	k_mpool_dealloc(MPID_IRAM2,gp_current_task->psp);
	
	gp_current_task->state = DORMANT;
	
	gp_current_task->mailbox.max_size=0xffff;
	gp_current_task->mailbox.current_size=0;
	TCB* selectedTCB =sendQScheduler();
	
	while(selectedTCB !=NULL){
		selectedTCB->state = READY;
		selectedTCB ->msg = NULL;
		selectedTCB ->destination = NULL;
		
		addTCBtoBack(readyQueuesArray,selectedTCB->prio,selectedTCB);
		selectedTCB =sendQScheduler();
	}
	
	deallocateMailbox(&gp_current_task->mailbox);
	k_tsk_run_new();
    return;
}

int k_tsk_set_prio(task_t task_id, U8 prio) 
{
#ifdef DEBUG_0
    printf("k_tsk_set_prio: entering...\n\r");
    printf("task_id = %d, prio = %d.\n\r", task_id, prio);
#endif /* DEBUG_0 */

	if((prio > LOWEST || prio < HIGH) && prio!=PRIO_RT)
	{
		errno = EINVAL;
		return RTX_ERR;
	}
	
	if (task_id == gp_current_task->tid)
	{
		if(gp_current_task->prio < prio)
		{
			gp_current_task->prio = prio;
			
			addTCBtoBack(readyQueuesArray,prio,gp_current_task);
			gp_current_task->state = READY;
			return k_tsk_run_new();
			
		}
		else // this should include the case where we increase a task's prio to PRIO_RT
		{
			gp_current_task->prio = prio;
			return RTX_OK;
		}
		
	}
	else 
	{
		TCB * selectedTCB = &g_tcbs[(U32) task_id];
		if((selectedTCB->priv == 1 && gp_current_task->priv == 0))
		{
			errno = EPERM;
			return RTX_ERR;
		}
		else if(selectedTCB->prio == PRIO_NULL){
			errno = EINVAL;
			return RTX_ERR;
		}
		else if(selectedTCB->prio == prio)
		{
			return RTX_OK;
		}
		else
		{
			if(selectedTCB->state != BLK_RECV && selectedTCB->state != BLK_SEND)
			{
				removeSpecificTCB(readyQueuesArray,selectedTCB->prio,task_id);
				
				addTCBtoFront(readyQueuesArray,gp_current_task->prio,gp_current_task);
				addTCBtoBack(readyQueuesArray,prio,selectedTCB);
				
				
				gp_current_task->state = READY;
			}
			else if(selectedTCB->state != BLK_SEND)
			{
				removeSpecificTCB(sendQueuesArray,selectedTCB->prio,task_id);
				
				addTCBtoBack(sendQueuesArray,prio,selectedTCB);
			}
			selectedTCB->prio = prio;

			
			return k_tsk_run_new();
		}	
	}
}

/**
 * @brief   Retrieve task internal information 
 * @note    this is a dummy implementation, you need to change the code
 */
int k_tsk_get(task_t tid, RTX_TASK_INFO *buffer)
{
#ifdef DEBUG_0
    printf("k_tsk_get: entering...\n\r");
    printf("tid = %d, buffer = 0x%x.\n\r", tid, buffer);
#endif /* DEBUG_0 */   
    if (tid >= MAX_TASKS) {
        errno = EINVAL;
        return RTX_ERR;
    }
    if (buffer == NULL) {
        errno = EFAULT;
        return RTX_ERR;
    }

    // Check if task_id is empty in g_tcbs - unknown if it should be null or tid null
    if (g_tcbs[tid].initialized == FALSE) {
            errno = EINVAL;
            return RTX_ERR;
    }
    
    buffer->tid           = g_tcbs[tid].tid;
    buffer->prio          = g_tcbs[tid].prio;
    buffer->priv          = g_tcbs[tid].priv;
    buffer->state         = g_tcbs[tid].state;
    buffer->ptask         = g_tcbs[tid].entry_point;

    buffer->k_sp          = (U32)g_tcbs[tid].msp;
    buffer->k_sp_base     = (U32)g_tcbs[tid].msp_base;
    buffer->k_stack_size  = g_tcbs[tid].msp_stack_size;

    buffer->u_sp          = (U32)g_tcbs[tid].psp;
    buffer->u_sp_base     = (U32)g_tcbs[tid].psp_base;
    buffer->u_stack_size  = g_tcbs[tid].psp_stack_size;

    return RTX_OK;     
}

int k_tsk_ls(task_t *buf, size_t count){
#ifdef DEBUG_0
    printf("k_tsk_ls: buf=0x%x, count=%u\r\n", buf, count);
#endif /* DEBUG_0 */

    if (buf == NULL || count == 0) {
        errno = EFAULT;
        return RTX_ERR;
    }

    int buf_i = 0; 
    for (int i = 0; i < MAX_TASKS; i++) { // NOT skipping null task
        if (g_tcbs[i].state != DORMANT) {
            buf[buf_i] = g_tcbs[i].tid;
            buf_i++;

            if (buf_i == count) {
                return count;
            }
        }
    }

    return buf_i;
}

U32 getTotalTimeMicroSeconds(TIMEVAL * tv){
	return tv->sec*1000000+tv->usec;
}

//Purpose: elevate a non-RT task to RT
int k_rt_tsk_set(TIMEVAL *p_tv)
{
#ifdef DEBUG_0
    printf("k_rt_tsk_set: p_tv = 0x%x\r\n", p_tv);
#endif /* DEBUG_0 */
	
		//check if the current task is already a RT task
		if(gp_current_task->state == PRIO_RT) {
			errno = EPERM;
			return RTX_ERR;
		}
		
		//verify that p_tv is a multiple of 2500 microseconds
		U32 microSecondTime = getTotalTimeMicroSeconds(p_tv);
		if(microSecondTime%2500!=0 || microSecondTime==0){
			errno = EINVAL;
			return RTX_ERR;
		}
		
		// TODO: error for ENOMEM - don't see where this could occur - may fail at k_tsk_run_new() but not due to mem
		// Kind of explains it for the rt_set_function: https://piazza.com/class/l2ahaqd6n9c6nk?cid=457
		// Adding the line below for now but I don't think it's necessary for the current state of the rt_task_info struct
		gp_current_task->rt_info = k_mpool_alloc(MPID_IRAM2, sizeof(rt_task_info));
		if(gp_current_task->rt_info == NULL){
			errno = ENOMEM;
			return RTX_ERR;
		}
		
		//Set to RT and release immediately
		k_tsk_set_prio(gp_current_task->tid, PRIO_RT); // TODO: this won't work yet
		pushToEDF(&readyQueuesArray[0], gp_current_task); // TODO: should we be adding to the front by force here - "release immediately"?
		return k_tsk_run_new();
}

// Purpose: suspend a real-time task until its next period starts
// This function should not be called unless the finished job has met the task's deadline 
int k_rt_tsk_susp(void)
{
#ifdef DEBUG_0
    printf("k_rt_tsk_susp: entering\r\n");
#endif /* DEBUG_0 */

		//if not called by an RT task, return -1
		if(gp_current_task->prio != PRIO_RT) {
			errno = EPERM;
			return RTX_ERR;
		}
		// TODO: error for ENOMEM - don't see where this could occur - may fail at k_tsk_run_new() but not due to mem
		// Still confused: https://piazza.com/class/l2ahaqd6n9c6nk?cid=457
		gp_current_task->state = SUSPENDED;
		addTCBtoBack(readyQueuesArray, SUSP_PRIO, gp_current_task);
		// I am assuming at this point the task is no longer in the EDF, so all we have to do is set it's state

		// if the current task is suspended, we should schedule another RT task or some non-RT task
		return k_tsk_run_new();
}

// Purpose: obtain real-time task period information from the kernel
int k_rt_tsk_get(task_t tid, TIMEVAL *buffer)
{
#ifdef DEBUG_0
    printf("k_rt_tsk_get: entering...\n\r");
    printf("tid = %d, buffer = 0x%x.\n\r", tid, buffer);
#endif /* DEBUG_0 */    
    if (buffer == NULL) {
        return RTX_ERR;
    }
		
    /* The code fills the buffer with some fake rt task information. 
       You should fill the buffer with correct information    */
		// if the data is ABCDEEFF, something error has occured
    buffer->sec  = 0xABCD;
    buffer->usec = 0xEEFF;
    
		//calling task not RT
		if(gp_current_task->prio!=PRIO_RT){
			errno = EPERM;
			return RTX_ERR;
		}
		
		//task_id does not point to an RT task
		if(g_tcbs[tid].prio != PRIO_RT) {
			errno = EINVAL;
			return RTX_ERR;
		}
		
    buffer->sec  = g_tcbs[tid].rt_info->period.sec;
    buffer->usec = g_tcbs[tid].rt_info->period.usec; 
		
    return RTX_OK;
}
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

