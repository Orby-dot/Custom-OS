/**
 * @brief The KCD Task Template File
 * @note  The file name and the function name can be changed
 * @see   tasks.h
 */

#include "rtx.h"
#include "free_list.h"
#include "k_msg.h"
#include "common.h"
#include "k_task.h"
#include "printf.h"

#define UNDEF 20

// only lowercase
U8 lowercase[26]; // array of strings
U8 uppercase[26]; // array of strings
U8 digits[10];    // array of strings

U8 getTaskId(char cmd)
{
    if (cmd >= '0' && cmd <= '9')
    {
        return digits[cmd - '0'];
    }
    else if (cmd >= 'A' && cmd <= 'Z')
    {
        return uppercase[cmd - 'A'];
    }
    else if (cmd >= 'a' && cmd <= 'z')
    {
        return lowercase[cmd - 'a'];
    }

    return UNDEF;
}

U8 assignTaskId(char cmd, U8 sender_tid)
{
    if (cmd >= '0' && cmd <= '9')
    {
        digits[cmd - '0'] = sender_tid;
    }
    else if (cmd >= 'A' && cmd <= 'Z')
    {
        uppercase[cmd - 'A'] = sender_tid;
    }
    else if (cmd >= 'a' && cmd <= 'z')
    {
        lowercase[cmd - 'a'] = sender_tid;
    }

    return UNDEF;
}

int countDigits (int x) {
    if (x < 10) return 1;
    if (x < 100) return 2;
    if (x < 1000) return 3;
    return 4;
}

// TODO: make it utility class function?
void printToConsole(char *data, U32 data_len)
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

void printLTbuffer(task_t *buffer, int count)
{
    char *tmp_str = k_mpool_alloc(MPID_IRAM1, 100);
    
    char *tmp_str_cursor = tmp_str;
    tmp_str_cursor[0] = '\n';
    tmp_str_cursor[1] = '\r';
    tmp_str_cursor += 2;
    int total_len = 2; // including initial new line
    for (int i = 0; i < count; i++)
    {
        sprintf(tmp_str_cursor, "%x-%d ", buffer[i], g_tcbs[buffer[i]].state);
        tmp_str_cursor += 2 + countDigits(g_tcbs[buffer[i]].state) + 1;
        total_len += 2 + countDigits(g_tcbs[buffer[i]].state) + 1;
    }
    tmp_str[total_len] = '\n';
    tmp_str[total_len + 1] = '\r';
    tmp_str[total_len + 2] = '\0';

    printToConsole(tmp_str, total_len + 3); // + 3 for \n\r\0
    
    k_mpool_dealloc(MPID_IRAM1, tmp_str);
}

void printLMbuffer(task_t *buffer, int count)
{

    char *tmp_str = k_mpool_alloc(MPID_IRAM1, 100);
    
    char *tmp_str_cursor = tmp_str;
    tmp_str_cursor[0] = '\n';
    tmp_str_cursor[1] = '\r';
    tmp_str_cursor += 2;
    int total_len = 2; // including initial new line
    for (int i = 0; i < count; i++)
    {
        sprintf(tmp_str_cursor, "%x-%d-%d ", buffer[i], g_tcbs[buffer[i]].state, k_mbx_get(buffer[i]));
        tmp_str_cursor += 2 + countDigits(g_tcbs[buffer[i]].state) + 1 + countDigits(k_mbx_get(buffer[i])) + 1;
        total_len +=2 + countDigits(g_tcbs[buffer[i]].state) + 1 + countDigits(k_mbx_get(buffer[i])) + 1;
    }
    tmp_str[total_len] = '\n';
    tmp_str[total_len + 1] = '\r';
    tmp_str[total_len + 2] = '\0';

    printToConsole(tmp_str, total_len + 3); // + 3 for \n\r\0
    
    k_mpool_dealloc(MPID_IRAM1, tmp_str);
}

void prnCNF() {
    char *cnf = mem_alloc(22);
    sprintf(cnf, "\n\rCommand not found\n\r");
    printToConsole(cnf, 22);
    mem_dealloc(cnf);
}

void task_kcd(void)
{
    //printf("KCD TASK CALLED \n\r");
    for (int i = 0; i < 26; i++)
    {
        lowercase[i] = UNDEF;
        uppercase[i] = UNDEF;
        if (i < 10)
            digits[i] = UNDEF;
    }

    // request a mailbox of size KCD_MBX_SIZE
    mbx_create(KCD_MBX_SIZE);

    char *msg_buf = mem_alloc(KCD_CMD_BUF_SIZE); // is repeatedly overwritten

    char cmd[KCD_CMD_BUF_SIZE]; // TODO:
    int len = 0;

    while (1)
    {

        // infinite call recv_msg
        recv_msg(msg_buf, KCD_CMD_BUF_SIZE);
        RTX_MSG_HDR *header = (RTX_MSG_HDR *)msg_buf;
        char *data = (char *)(msg_buf);
        data += 6;
        //printf("KCD DATA WE GOT: %x \n\r", *data);

        if (header->type == KCD_REG)
        {
            assignTaskId(data[0], header->sender_tid);
        }
        else if (header->type == KEY_IN)
        {
            //printf("GOT INPUT OF KEY!!!!!!!!\n\r");
            if (*(data) == '\r' || *(data) == '\n') // TODO: Is enter check correct?
            {
                if (cmd[0] == '%')
                {
                    if (cmd[1] == 'L' && len == 3)
                    {
                        if (cmd[2] == 'T')
                        {
                            task_t buffer[10];
                            int ret = k_tsk_ls(buffer, 10);
                            //printf("*****TASK_LS: %d\n\r", ret);
                            printLTbuffer(buffer, ret);
                        }
                        else if (cmd[2] == 'M')
                        {
                            task_t buffer[10];
                            int ret = k_mbx_ls(buffer, 10);
                            //printf("*****MSG_LS: %d\n\r", ret);
                            printLMbuffer(buffer, ret);
                        }
                        else
                        {
                            prnCNF();
                        }
                    }
                    else if (cmd[1] == 'L' && len != 3)
                    {
                        char *invalid_command = mem_alloc(20);
                        sprintf(invalid_command, "\n\rInvalid Command\n\r");
                        printToConsole(invalid_command, 20);
                        mem_dealloc(invalid_command);
                    }
                    else
                    { 
                        U8 taskId = getTaskId(cmd[1]);

                        if (taskId != UNDEF)
                        {
                            char *to_send = mem_alloc(6 + len);
                            RTX_MSG_HDR *header_ts = (RTX_MSG_HDR *)to_send;
                            char *data_ts = (char *)(to_send);
                            data_ts += 6;

                            header_ts->length = len + 6;
                            header_ts->sender_tid = TID_KCD;
                            header_ts->type = KCD_CMD;

                            for (int i = 0; i < len - 1; i++)
                            {
                                data_ts[i] = cmd[i + 1];
                            }

                            send_msg(taskId, to_send); //TODO: any uart edge case handling here?

                            char *csnt = mem_alloc(17);
                            sprintf(csnt, "\n\rCommand Sent\n\r");
                            printToConsole(csnt, 17);
                            mem_dealloc(csnt);

                            mem_dealloc(to_send);

                            len = 0;
                        }
                        else
                        {
                            prnCNF();
                        }
                    }
                }
                else
                {
                    char *invalid_command = mem_alloc(20);
                    sprintf(invalid_command, "\n\rInvalid Command\n\r");
                    printToConsole(invalid_command, 20);
                    mem_dealloc(invalid_command);
                }

                for (int i = 0; i < KCD_CMD_BUF_SIZE; i++)
                {
                    cmd[i] = NULL;
                }
                len = 0;
            }
            else
            {
                cmd[len] = *(data);
                printToConsole(data, 1);
                len++;
            }
        }
        else
        {
            // "ignores the rest"
        }
    }

    // mem_dealloc(msg_buf); // will probably never run
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
