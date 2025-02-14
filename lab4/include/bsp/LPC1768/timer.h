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
 * @file        timer.h
 * @brief       timer interrupt header file 
 *              
 * @version     V1.2021.07
 * @authors     Yiqing Huang
 * @date        2021 JUL
 *****************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <LPC17xx.h>
#include "../../../RTX-App/src/kernel/EDF.h"

/*
 *===========================================================================
 *                            MACROS
 *===========================================================================
 */

#define TIMER0  0
#define TIMER1  1
#define TIMER2  2
#define TIMER3  3

/*
 *===========================================================================
 *                            STRUCTURES AND TYPEDEFS
 *===========================================================================
 */

typedef struct tm_tick {
    uint32_t tc;  /**< TC register value, for TIMER1 it is in seconds */
    uint32_t pc;  /**< PC register value, for TIMER1 it is in tens of nano seconds */
} TM_TICK;

/*
 *===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================
 */
 

extern uint32_t timer_irq_init      (uint8_t n_timer);  /* interrupt-driven */
extern uint32_t timer_freerun_init  (uint8_t n_timer);  /* free running     */
extern int      get_tick            (TM_TICK *tk, uint8_t n_timer); 

#endif /* ! _TIMER_H_ */

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
 
