/**
 * @brief The KCD Task Template File
 * @note  The file name and the function name can be changed
 * @see   tasks.h
 */

#include "rtx.h"
#include "free_list.h"
#include "k_msg.h"
#include "common.h"

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

// TODO: make it utility class function?
void printToConsole(char *data, U32 data_len) {
    char *to_send = mem_alloc(6 + data_len);
    RTX_MSG_HDR *header_ts = (RTX_MSG_HDR *)to_send;
    char *data_ts = (char *)(to_send);
    data_ts += 6;

    header_ts->length = data_len + 6;
    header_ts->sender_tid = TID_KCD;
    header_ts->type = DISPLAY;

    *data_ts = *data;
    send_msg(TID_CON, to_send);

    mem_dealloc(to_send);
}


void task_kcd(void)
{
		printf("KCD TASK CALLED \r\n");
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
		printf("KCD TASK RECEIVED A MESSAGE \r\n");
        // printToConsole("!", 1);
        RTX_MSG_HDR *header = (RTX_MSG_HDR *)msg_buf; 
        char *data = (char *)(msg_buf);
        data += 6;
				printf("KCD DATA WE GOT: %x \r\n", *data);

        if (header->type == KCD_REG)
        {
            assignTaskId(data[0], header->sender_tid);
        }
        else if (header->type == KEY_IN)
        {
            printf("GOT INPUT OF KEY!!!!!!!!\r\n");
            if (*(data) == '\r') // TODO: Is enter check correct?
            { 
                if (cmd[0] == '%')
                {
                    if (cmd[1] == 'L') {
                        // TODO:
                    } else {
                        U8 taskId = getTaskId(cmd[1]);

                        if (taskId != UNDEF) {
                            char *to_send = mem_alloc(6 + len);
                            RTX_MSG_HDR *header_ts = (RTX_MSG_HDR *)to_send;
                            char *data_ts = (char *)(to_send);
                            data_ts += 6;

                            header_ts->length = len + 6;
                            header_ts->sender_tid = TID_KCD;
                            header_ts->type = KCD_CMD;

                            for (int i = 0; i < len - 1; i++) {
                                data_ts[i] = cmd[i + 1];
                            }

                            send_msg(taskId, to_send);

                            mem_dealloc(to_send);

                            len = 0;
                        } else {
                            printToConsole("Command not found", 18);
                        }
                    }

                } 
                else {
                    printToConsole("Invalid Command", 15);
                }
									
                // Enter on Console
                printToConsole("\r", 1);

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

    mem_dealloc(msg_buf); // will probably never run

}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
