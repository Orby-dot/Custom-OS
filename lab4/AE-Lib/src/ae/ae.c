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
 * @file        ae.c
 * @brief       Automated Evaluation (AE) Framework Source File
 *
 * @version     V1.2022.05
 * @authors     Yiqing Huang
 * @date        2022 MAY
 *****************************************************************************/

#include "ae.h"

/**************************************************************************//**
 * @brief   	ae_init
 * @return		RTX_OK on success and RTX_ERR on failure
 * @param[out]  sys_info system initialization struct AE writes to
 * @param[out]	task_info boot-time tasks struct array AE writes to
 *
 *****************************************************************************/

int	ae_init(RTX_SYS_INFO *sys_info, 
                TASK_INIT    **tasks, 
                int          *num_tasks, 
                int          (*cb_func) (void *(arg)), 
                void         *arg)
{
	if ( ae_set_sys_info(sys_info) != RTX_OK ) {
		return RTX_ERR;
	}
    cb_func(arg);
	ae_set_init_tasks_info(tasks, num_tasks);
	return RTX_OK;
}


/**************************************************************************//**
 * @brief   	 debugger .ini can reference this one to exit 
 *****************************************************************************/

void ae_exit(void)
{
    while(1){
			tsk_yield();
		}
}

/**************************************************************************//**
 * @brief       fill the sys_info struct with system configuration info.
 * @return		RTX_OK on success and RTX_ERR on failure
 * @param[out]  sys_info system initialization struct AE writes to
 *
 *****************************************************************************/
int ae_set_sys_info(RTX_SYS_INFO *sys_info)
{
    if (sys_info == NULL) {
        return RTX_ERR;
    }

    sys_info->mem_algo      = BUDDY;
#ifndef ECE350_P4
    sys_info->sched         = DEFAULT;
#else    
    sys_info->sched         = EDF;
#endif    
    return RTX_OK;
}

void ae_set_init_tasks_info (TASK_INIT **pp_tasks, int *p_num_tasks)
{
    if (pp_tasks == NULL) {
		return;
	}
    set_ae_init_tasks(pp_tasks, p_num_tasks);
    
    return;
}



/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

