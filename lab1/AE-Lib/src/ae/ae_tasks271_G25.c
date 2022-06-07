/*
 ****************************************************************************
 *
 *                     Copyright ʕ◕ᴥ◕ ʔ PuffyPanda#1588
 *                          All rights (not) reserved.
 *
 ****************************************************************************
 */

#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae.h"
#include "ae_util.h"
#include "ae_tasks_util.h"

/*
 *===========================================================================
 *                             MACROS
 *===========================================================================
 */
    
#define NUM_TESTS       1       // number of tests
#define NUM_INIT_TASKS  1       // number of tasks during initialization
/*
 *===========================================================================
 *                             GLOBAL VARIABLES 
 *===========================================================================
 */

TASK_INIT   g_init_tasks[NUM_INIT_TASKS];
const char   PREFIX[]      = "G99-TS200";
const char   PREFIX_LOG[]  = "G99-TS200-LOG";
const char   PREFIX_LOG2[] = "G99-TS200-LOG2";

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

/*
 *===========================================================================
 *                             HELPER FUNCTIONS
 *===========================================================================
 */

void printRTXtaskInfo(RTX_TASK_INFO *buffer) {
    printf("----- TASK GET OUTPUT ----- \r\n");
    printf("TID: %u\r\n", buffer->tid);
    printf("PRIO: x%x -- PRIV: %u -- STATE: %u \r\n", buffer->prio, buffer->priv, buffer->state);
    // print ptask: TODO:
    printf("K stack_size: x%x -- sp: %u -- sp_base: %u\r\n", buffer->k_stack_size, buffer->k_sp, buffer->k_sp_base);
    printf("U stack_size: x%x -- sp: %u -- sp_base: %u\r\n", buffer->u_stack_size, buffer->u_sp, buffer->u_sp_base);
    printf("----- -----\r\n");
}


void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num)
{
    *p_num = NUM_INIT_TASKS;
    *pp_tasks = g_init_tasks;
    set_ae_tasks(*pp_tasks, *p_num);
}

// initial task configuration
void set_ae_tasks(TASK_INIT *tasks, int num)
{
    for (int i = 0; i < num; i++ ) {                                                 
        tasks[i].u_stack_size = PROC_STACK_SIZE;    
        tasks[i].prio = LOW;
        tasks[i].priv = 1;
    }
    tasks[0].priv  = 1;
    tasks[0].ptask = &priv_task1;
    tasks[1].priv  = 0;
    tasks[1].ptask = &task1;
    
    init_ae_tsk_test();
}

void init_ae_tsk_test(void)
{
    g_ae_xtest.test_id = 0;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests = NUM_TESTS;
    g_ae_xtest.num_tests_run = 0;
    
    for ( int i = 0; i< NUM_TESTS; i++ ) {
        g_tsk_cases[i].p_ae_case = &g_ae_cases[i];
        g_tsk_cases[i].p_ae_case->results  = 0x0;
        g_tsk_cases[i].p_ae_case->test_id  = i;
        g_tsk_cases[i].p_ae_case->num_bits = 0;
        g_tsk_cases[i].pos = 0;  // first avaiable slot to write exec seq tid
        // *_expt fields are case specific, deligate to specific test case to initialize
    }
    printf("%s: START\r\n", PREFIX);
}

void update_ae_xtest(int test_id)
{
    g_ae_xtest.test_id = test_id;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests_run++;
}

void gen_req0(int test_id)
{
    // bits[0:1] for two tsk_create, [2:5] for 4 tsk_yield return value checks
    g_tsk_cases[test_id].p_ae_case->num_bits = 6;  
    g_tsk_cases[test_id].p_ae_case->results = 0;
    g_tsk_cases[test_id].p_ae_case->test_id = test_id;
    g_tsk_cases[test_id].len = 16; // assign a value no greater than MAX_LEN_SEQ
    g_tsk_cases[test_id].pos_expt = 6;
       
    g_ae_xtest.test_id = test_id;
    g_ae_xtest.index = 0;
    g_ae_xtest.num_tests_run++;
}

int test0_start(int test_id)
{

    // Checking first privileged task information
    RTX_TASK_INFO buffer;
    tsk_get(1, &buffer);
    printRTXtaskInfo(&buffer);

    int ret_val = 10;
    task_t tid1;
    task_t tid2;
    
    gen_req0(test_id);
    
    U8      *p_index    = &(g_ae_xtest.index);
    int     sub_result  = 0;
    
    //test 0-[0]
    *p_index = 0;
    strcpy(g_ae_xtest.msg, "creating a HIGH prio task that runs task2 function");
    ret_val = tsk_create(&tid1, &task2, HIGH, 0x199);  /*create a user task */
    sub_result = (ret_val == RTX_OK) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);    
    if ( ret_val != RTX_OK ) {
        sub_result = 0;
        test_exit();
    }

    // Checking 2nd task (unpriv)
    tsk_get(tid1, &buffer);
    printRTXtaskInfo(&buffer);

    //test 0-[1]
    (*p_index)++;
    strcpy(g_ae_xtest.msg, "creating a HIGH prio task that runs task3 function");
    ret_val = tsk_create(&tid2, &task3, HIGH, 0x201);  /*create a user task */
    sub_result = (ret_val == RTX_OK) ? 1 : 0;
    process_sub_result(test_id, *p_index, sub_result);
    if ( ret_val != RTX_OK ) {
        test_exit();
    }

    // Checking 3rd task (unpriv)
    tsk_get(tid2, &buffer);
    printRTXtaskInfo(&buffer);

    task_t *p_seq_expt = g_tsk_cases[test_id].seq_expt;
    for ( int i = 0; i < 6; i += 3 ) {
        p_seq_expt[i]   = tsk_gettid();;
        p_seq_expt[i+1] = tid1;
        p_seq_expt[i+2] = tid2;
    }
    
    return RTX_OK;
}


int update_exec_seq(int test_id, task_t tid) 
{

    U8 len = g_tsk_cases[test_id].len;
    U8 *p_pos = &g_tsk_cases[test_id].pos;
    task_t *p_seq = g_tsk_cases[test_id].seq;
    p_seq[*p_pos] = tid;
    (*p_pos)++;
    (*p_pos) = (*p_pos)%len;  // preventing out of array bound
    return RTX_OK;
}

/**************************************************************************//**
 * @brief       a task that prints AAAAA, BBBBB on each line.
 *              it yields the cpu after each line
 *****************************************************************************/

void priv_task1(void)
{
    printf("===== priv_task1 begins =====\r\n");

    task_t tid = tsk_gettid();
    int test_id = 0;
    U8      *p_index    = &(g_ae_xtest.index);
    int     sub_result  = 0;
    
    printf("%s: TID = %d, priv_task1: starting test0\r\n", PREFIX_LOG2, tid);
    strcpy(g_ae_xtest.msg, "Four same priority tasks yielding cpu test");
    test0_start(test_id);
    
    for ( int i = 0; i < 2; i++) {
        char out_char = 'A' + i%26;
        for ( int j = 0; j < 5; j++) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\r\n");       
        printf("%s: TID = %d, priv_task1: yielding cpu\r\n", PREFIX_LOG2, tid );
        update_exec_seq(test_id, tid);
        int ret = tsk_yield();
        // 0-[2:3]
        sprintf(g_ae_xtest.msg, "priv_task1 calling tsk_yield - %d", i+1);
        sub_result = ( ret == RTX_OK ) ? 1 : 0;
        (*p_index)++;
        process_sub_result(test_id, *p_index, sub_result);
    }
    tsk_yield();
    tsk_yield();

    tsk_exit();
}

/**************************************************************************//**
 * @brief:      a task that prints 00000, 11111 and 22222 on each line.
 *              It yields the cpu every after each line is printed.
 *****************************************************************************/

void task1(void)
{
    printf("===== task1 begins =====\r\n");
    int test_id = 0;
    U8      *p_index    = &(g_ae_xtest.index);
    int     sub_result  = 0;
    
    task_t tid = tsk_gettid();    
    
    for ( int i = 0; i < 2; i++) {
        char out_char = '1' + i%26;
        for ( int j = 0; j < 5; j++) {
            uart1_put_char(out_char);
        }
        uart1_put_string("\r\n");
        printf("%s: TID = %d, task1: yielding cpu\r\n", PREFIX_LOG2, tid);
        update_exec_seq(test_id, tid);
        int ret = tsk_yield();
        sprintf(g_ae_xtest.msg, "task1 calling tsk_yield - %d", i+1);
        sub_result = ( ret == RTX_OK ) ? 1 : 0;
        (*p_index)++;
        process_sub_result(test_id, *p_index, sub_result);
    }
    
    tsk_set_prio(tid, MEDIUM);
    printf("%s: TID = %d, task1: I should not run \r\n", PREFIX_LOG2, tid);
    update_exec_seq(test_id, tid);

    tsk_exit();
}

/**
 * @brief: a dummy task2
 */
void task2(void)
{
    printf("===== task2 begins =====\r\n");
    task_t tid = tsk_gettid();
    int    test_id = 0;
    U8     *p_index    = &(g_ae_xtest.index);
    int    sub_result  = 0;
    
    printf("%s: TID = %d, task2: entering\r\n", PREFIX_LOG2, tid);
    for ( int i = 0; i < 2; i++) {
        printf("%s: TID = %d, task2: yielding cpu \r\n", PREFIX_LOG2, tid);
        update_exec_seq(test_id, tid);
        tsk_yield();
        int ret = tsk_yield();
        // 0-[4]
        sprintf(g_ae_xtest.msg, "task2 calling tsk_yield - %d", i+1);
        sub_result = ( ret == RTX_OK ) ? 1 : 0;
        (*p_index)++;
        process_sub_result(test_id, *p_index, sub_result);
    }
    tsk_set_prio(tid, LOW);
    printf("%s: TID = %d, task2: I should not run \r\n", PREFIX_LOG2, tid);
    update_exec_seq(test_id, tid);
    tsk_yield();

    tsk_exit();
}

/**
 * @brief: a dummy task3
 */
void task3(void)
{
    printf("===== task3 begins =====\r\n");

    int    test_id = 0;
    task_t tid = tsk_gettid();
    U8     *p_index    = &(g_ae_xtest.index);
    int    sub_result  = 0;
    
    printf("%s: TID = %d, task3: entering \r\n", PREFIX_LOG2, tid);
    
    for ( int i = 0; i < 2; i++) {
        printf("%s: TID = %d, task3: yielding cpu \r\n", PREFIX_LOG2, tid);
        update_exec_seq(test_id, tid);
        tsk_yield();
        int ret = tsk_yield();
        // 0-[5]
        sprintf(g_ae_xtest.msg, "task3 calling tsk_yield - %d", i+1);
        sub_result = ( ret == RTX_OK ) ? 1 : 0;
        (*p_index)++;
        process_sub_result(test_id, *p_index, sub_result);
    }
    tsk_set_prio(tid, LOWEST);
    printf("%s: TID = %d, task3: I should not run \r\n", PREFIX_LOG2, tid);
    update_exec_seq(test_id, tid);
    

    
    test_exit();
    
    tsk_yield(); // should trigger null task forever

    tsk_exit(); // is never called, presumably
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
