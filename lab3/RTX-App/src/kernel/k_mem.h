/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2022 Yiqing Huang
 *
 *          This software is subject to an open source license and
 *          may be freely redistributed under the terms of MIT License.
 ****************************************************************************
 */

/**************************************************************************//**
 * @file        k_mem.h
 * @brief       Kernel Memory Management API Header File
 *
 * @version     V1.2021.04
 * @authors     Yiqing Huang
 * @date        2021 APR 
 *
 * @note        skeleton code
 *
 *****************************************************************************/


#ifndef K_MEM_H_
#define K_MEM_H_
#include "k_inc.h"
#include "lpc1768_mem.h"        // board memory map
#include "bit_array.h"
#include "tester.h"

//initialize global

extern bitArray array_RAM1;
extern bitArray array_RAM2;

extern freeList_t free_list_RAM1 [8];
extern freeList_t free_list_RAM2 [11];

extern U8 bitarray_RAM1[((2*(RAM1_SIZE)/32 -1)/8)];
extern U8 bitarray_RAM2[((2*(RAM2_SIZE)/32 -1)/8)];

/*
 * ------------------------------------------------------------------------
 *                             FUNCTION PROTOTYPES
 * ------------------------------------------------------------------------
 */
// kernel API that requires mpool ID
mpool_t k_mpool_create  (int algo, U32 strat, U32 end);
void   *k_mpool_alloc   (mpool_t mpid, size_t size);
int     k_mpool_dealloc (mpool_t mpid, void *ptr);
int     k_mpool_dump    (mpool_t mpid);

int     k_mem_init      (int algo);
void printListLevelInOrder(freeList_t  *list, int level, U8 totalLevels, int *count);
// declare newly added functions here


/*
 * ------------------------------------------------------------------------
 *                             FUNCTION MACROS
 * ------------------------------------------------------------------------
 */

#endif // ! K_MEM_H_

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

