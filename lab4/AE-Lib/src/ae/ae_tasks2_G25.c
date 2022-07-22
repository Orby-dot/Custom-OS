/**
 * Real Time Task ERRNO
 */ 

/**************************************************************************//**
 * @file        ae_dummy_tasks_p1.c
 * @brief       dummy tasks for P1
 *
 * @version     V1.2022.05
 * @authors     Yiqing Huang
 * @date        2022 MAY
 *
 * @note        Each task is in an infinite loop. These Tasks never terminate.
 *
 *****************************************************************************/

#include "ae_tasks.h"
#include "uart_polling.h"
#include "printf.h"
#include "ae_util.h"
#include "ae_tasks_util.h"

#define     BUF_LEN         100
#define     MY_MSG_TYPE     100     // some customized message type, better move it to common_ext.h

#define     NUM_TESTS       1       // number of tests
#define     NUM_INIT_TASKS  2       // number of tasks during initialization

TASK_INIT   g_init_tasks[NUM_INIT_TASKS];

U8 g_buf1[BUF_LEN];
U8 g_buf2[BUF_LEN];
task_t g_tasks[MAX_TASKS];



const char   PREFIX[]      = "G25-TS11";
const char   PREFIX_LOG[]  = "G25-TS11-LOG";
const char   PREFIX_LOG2[] = "G25-TS11-LOG2";

AE_XTEST     g_ae_xtest;                // test data, re-use for each test
AE_CASE      g_ae_cases[NUM_TESTS];
AE_CASE_TSK  g_tsk_cases[NUM_TESTS];

/* The following arrays can also be dynamic allocated to reduce ZI-data size
 *  They do not have to be global buffers (provided the memory allocator has no bugs)
 */
 
task_t g_tasks[MAX_TASKS];
task_t g_tids[MAX_TASKS];


void set_ae_init_tasks (TASK_INIT **pp_tasks, int *p_num)
{
    *p_num = NUM_INIT_TASKS;
    *pp_tasks = g_init_tasks;
    set_ae_tasks(*pp_tasks, *p_num);
}

void set_ae_tasks(TASK_INIT *tasks, int num)
{
    for (int i = 0; i < num; i++ ) {                                                 
        tasks[i].u_stack_size = PROC_STACK_SIZE;    
        tasks[i].prio = HIGH;
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
        g_tsk_cases[i].p_ae_case->num_bits = 10;
        g_tsk_cases[i].pos = 0;  // first avaiable slot to write exec seq tid
        // *_expt fields are case specific, deligate to specific test case to initialize
    }
    printf("%s: START\r\n", PREFIX);
}

/**************************************************************************//**
 * @brief       a task that prints AAAAA, BBBBB, CCCCC,...., ZZZZZ on each line.
 *              It yields the cpu every 6 lines are printed.
 *****************************************************************************/

void priv_task1(void) {
    int ret_val = 10;
    mbx_t mbx_id = -1;
    task_t tid = tsk_gettid();
    task_t tid1; 
    char *buf = NULL;           // this is a user dynamically allocated buffer
    
    mbx_id = mbx_create(BUF_LEN);  // create a mailbox for itself
    
    
    ret_val = tsk_create(&tid1, task2, HIGH, PROC_STACK_SIZE);
    
    tsk_set_prio(tid, HIGH);
    
		tsk_yield();
    tsk_exit();
    
}

/**************************************************************************//**
 * @brief:      a task that prints 00000, 11111, 22222,....,99999 on each line.
 *              It yields the cpu every 6 lines are printed
 *              before printing these lines indefinitely, it does the following:
 *              - creates a mailbox
 *              - registers %G command with KCD 
 *              - blocking receive 
 *              - allocates some memory
 *              - deallocates some memory
 *              - calls memory dump function
 *****************************************************************************/
void task1(void)
{
    task_t tid = tsk_gettid();
    
    size_t msg_hdr_size = sizeof(struct rtx_msg_hdr);
    
    mbx_create(BUF_LEN);                    // create a mailbox for itself

    
    // code to process received messages omitted


    int *ptr1 = mem_alloc(sizeof(int));
    //printf("ptr = 0x%x\r\n", ptr1); 
    mem_dealloc(ptr1);
    mem_dump();


        printf("%s: ERRNO TEST IN TASK 1\r\n", PREFIX);

        U8      *p_index    = &(g_ae_xtest.index);
        int     sub_result  = 0;
                
        TIMEVAL tv;    

        tv.sec  = 0;
        tv.usec = 0;

        int ret = 7;
        
        // 0 - 0 
        ret = rt_tsk_set(&tv);

        *p_index = 0;
        strcpy(g_ae_xtest.msg, "Checking errno when rt_tsk_set with 0 period");
        sub_result = (ret == RTX_ERR && errno == EINVAL) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);


        // 0 - 1
        tv.sec  = 0;
        tv.usec = 350;
        ret = rt_tsk_set(&tv);

        (*p_index)++;
        strcpy(g_ae_xtest.msg, "Checking errno when rt_tsk_set with non-divisible period");
        sub_result = (ret == RTX_ERR && errno == EINVAL) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);

        // 0 - 2
        ret = rt_tsk_susp();

        (*p_index)++;
        strcpy(g_ae_xtest.msg, "Checking errno when rt_tsk_susp on non-rt task");
        sub_result = (ret == RTX_ERR && errno == EPERM) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);

        // 0 - 3
        TIMEVAL buffy;
        task_t my_tid = tsk_gettid();
        ret = rt_tsk_get(my_tid, &buffy); 

        (*p_index)++;
        strcpy(g_ae_xtest.msg, "Checking rt_tsk_get fails since I'm not a rt task.");
        sub_result = (ret == RTX_ERR && errno==EPERM) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);

        // 0 - 4
        ret = tsk_set_prio(my_tid, PRIO_RT);

        (*p_index)++;
        strcpy(g_ae_xtest.msg, "Checking set_prio to PRIO_RT fails for a non-RT task.");
        sub_result = (ret == RTX_ERR && errno == EPERM) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);

        // 0 - 5        
        tv.sec  = 1;
        tv.usec = 0;
        ret = rt_tsk_set(&tv);

        (*p_index)++;
        strcpy(g_ae_xtest.msg, "Checking valid rt_tsk_set");
        sub_result = (ret == RTX_OK) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);

        // 0 - 6
        tv.sec  = 1;
        tv.usec = 0;
        ret = rt_tsk_set(&tv);

        (*p_index)++;
        strcpy(g_ae_xtest.msg, "Checking errno when rt_tsk_set called twice");
        sub_result = (ret == RTX_ERR && errno == EPERM) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);

        // 0 - 7
        ret = rt_tsk_get(99, &buffy); 

        (*p_index)++;
        strcpy(g_ae_xtest.msg, "Checking rt_tsk_get fails for invalid tid.");
        sub_result = (ret == RTX_ERR && errno==EINVAL) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);

        // 0 - 8
        ret = rt_tsk_get(my_tid, &buffy); 

        (*p_index)++;
        strcpy(g_ae_xtest.msg, "Checking rt_tsk_get passes for me, now that I'm RT :).");
        sub_result = (ret == RTX_OK && buffy.sec == 1 && buffy.usec == 0) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);

        // 0 - 9
        ret = tsk_set_prio(my_tid, LOWEST);

        (*p_index)++;
        strcpy(g_ae_xtest.msg, "Checking set_prio fails for a RT task.");
        sub_result = (ret == RTX_ERR && errno == EPERM) ? 1 : 0;
        process_sub_result(0, *p_index, sub_result);

        g_ae_xtest.num_tests_run++;

        test_exit();

        tsk_exit();         // terminating the task
}
/**
 * @brief: a dummy task2
 */
void task2(void)
{
    tsk_exit();
}

/**
 * @brief: a dummy task3
 */
void task3(void)
{
    tsk_exit();
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
