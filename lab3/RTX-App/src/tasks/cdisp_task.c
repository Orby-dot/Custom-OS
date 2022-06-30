/**
 * @brief The Console Display Task Template File 
 * @note  The file name and the function name can be changed
 * @see   k_tasks.h
 */

#include "rtx.h"

void task_cdisp(void)
{
    k_mbx_create(CON_MBX_SIZE);

    while(1) {
    /* not implemented yet */

    // CALLED when something to output (character)

    // sends messsage to UART0_IRQ_Handler to output

    // enables TX Interrupt
    }
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

