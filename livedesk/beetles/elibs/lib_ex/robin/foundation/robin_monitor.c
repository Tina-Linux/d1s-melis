/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_monitor.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.19
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.11.19       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_monitor_c
#define  __robin_monitor_c


#include "foundation_i.h"


static  __u32        monitor_id  = 0;
static  __bool       not_handled = EPDK_FALSE;
static  robin_msg_t  msg_not_handled;
char                 g_robin_current_file[ROBIN_MNT_FILE_LEN_MAX] = {0};


static void   monitor(void *arg_p);
static __bool handle_msg(robin_msg_t *msg_p);

__s32  robin_monitor_create(__u8 prio)
{
    /* create monitor thread */
    if (monitor_id == 0)
    {
        monitor_id = esKRNL_TCreate(monitor, NULL, 0x2000, (EPOS_curid << 8) | prio);

        if (monitor_id == 0)
        {
            __err("Error in creating \"robin monitor\" thread.");
            return -1;
        }
    }

    return 0;
}



void   robin_monitor_delete(void)
{
    /* delete monitor thread */
    if (monitor_id != 0)
    {
        while (esKRNL_TDelReq(monitor_id) != OS_TASK_NOT_EXIST)
        {
            g_delay(10);
        }

        monitor_id = 0;
        __wrn("\"robin monitor\" thread has been deleted.");
    }
}

__s32  robin_monitor_get_cur_file(char *file, __u32 size)
{
    __s32  len;

    if (size > 0 && NULL != file)
    {
        file[0] = '\0';
    }

    if (g_robin_current_file[0] == '\0')
    {
        __wrn("error current file ....");
        return -1;
    }

    g_strcpy(file, g_robin_current_file);
    return 0;
}

void robin_wait_no_file(void)
{
    __u8  err;

    /* wait media file queue empty */
    while (1)
    {
        if (get_msg_nr_of_q(robin_cmdQ) == 0)
        {
            break;
        }

        g_delay(10);
    }

    /* wait monitor */
    __msg("%s *****before g_pend_mutex( robin_cmd_mutex, &err );*****", __FUNCTION__);
    g_pend_mutex(robin_cmd_mutex, &err);
    __msg("%s *****after g_pend_mutex( robin_cmd_mutex, &err );*****", __FUNCTION__);
    g_post_mutex(robin_cmd_mutex);
}

static void monitor(void *arg_p)
{
    __u8         err;
    robin_msg_t  robin_msg;
    robin_msg_t *msg_p = NULL;

    while (1)
    {
        g_pend_mutex(robin_cmd_mutex, &err);

        if (get_msg_nr_of_q(robin_cmdQ) != 0)
        {
            msg_p = (robin_msg_t *)g_QAccept(robin_cmdQ, &err);

            if (msg_p != NULL)
            {
                eLIBs_memset(&robin_msg, 0x00, sizeof(robin_msg_t));
                g_memcpy(&robin_msg, msg_p, sizeof(robin_msg_t));

                if (handle_msg(&robin_msg))
                {
                    g_post_mutex(robin_cmd_mutex);
                    goto exit;
                }
            }
            else
            {
                if (not_handled)            // some commands have not been handled
                {
                    eLIBs_memset(&robin_msg, 0x00, sizeof(robin_msg_t));
                    g_memcpy(&robin_msg, &msg_not_handled, sizeof(robin_msg_t));
                    not_handled = EPDK_FALSE;

                    if (handle_msg(&robin_msg))
                    {
                        g_post_mutex(robin_cmd_mutex);
                        goto exit;
                    }
                }
            }
        }

        g_post_mutex(robin_cmd_mutex);
        {
            //cedar的错误信息可能在播放过程发送上来，需要不断查询消息通道
            long  msg;
            msg = (long)g_QAccept(robin_cedar_msgQ, &err);

            if (msg != NULL)
            {
                /* feedback msg */
                if (msg == CEDAR_FEDBAK_NO_ERROR)
                {
                }
                else
                {
                    g_QFlush(robin_feedbackQ);
                    g_QPost(robin_feedbackQ, (unsigned long)msg);          // only feed back error message
                }
            }
        }

        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            goto exit;
        }

        g_delay(20);
    }

exit:
    g_robin_current_file[0] = '\0';
    esKRNL_TDel(OS_PRIO_SELF);
}



static __bool handle_msg(robin_msg_t *msg_p)
{
    __s32  ret;

    switch (msg_p->id)
    {
        case ROBIN_CMD_PLAY_FILE :
        {
            __s32 len;
            robin_msg_t *msg_q = NULL;
            __u8  err;

            /* stop fsm */
            if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL) != CEDAR_STAT_STOP)
            {
                ret = syn_op_stop();

                switch (ret)
                {
                    case SYN_OP_RET_SEND_CMD_ERR       :
                        g_memcpy(&msg_not_handled, msg_p, sizeof(robin_msg_t));
                        not_handled = EPDK_TRUE;
                        break;

                    case SYN_OP_RET_TO_QUIT_ROBIN      :
                        return EPDK_TRUE;

                    case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
                    case SYN_OP_RET_OK                 :
                    case SYN_OP_RET_OVERTIME           :
                    default :
                        break;
                }
            }

            /* check new file */
            msg_q = (robin_msg_t *)g_QAccept(robin_cmdQ, &err);

            if (msg_q != NULL)
            {
                if (msg_q->id == ROBIN_CMD_PLAY_FILE)
                {
                    g_memcpy(msg_p, msg_q, sizeof(robin_msg_t));
                }
                else
                {
                    g_memcpy(&msg_not_handled, msg_q, sizeof(robin_msg_t));
                    not_handled = EPDK_TRUE;
                }
            }

            /* set new media file to be played */
            esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_MEDIAFILE, 0, &(msg_p->file_info));
            /* save the current media file */
            len = min(ROBIN_MNT_FILE_LEN_MAX, g_strlen(msg_p->file_info.file_path));
            g_strncpy(g_robin_current_file, msg_p->file_info.file_path, len);
            g_robin_current_file[len] = '\0';
            /* play new media file */
            ret = syn_op_play();

            switch (ret)
            {
                case SYN_OP_RET_SEND_CMD_ERR       :
                    g_memcpy(&msg_not_handled, msg_p, sizeof(robin_msg_t));
                    not_handled = EPDK_TRUE;
                    break;

                case SYN_OP_RET_TO_QUIT_ROBIN      :
                    return EPDK_TRUE;

                case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
                case SYN_OP_RET_OK                 :
                case SYN_OP_RET_OVERTIME           :
                default :
                    break;
            }

            //__here__;
            break;
        }

        case ROBIN_CMD_PLAY      :
        {
            //__here__;
            ret = syn_op_play();

            //__here__;
            switch (ret)
            {
                case SYN_OP_RET_SEND_CMD_ERR       :
                    g_memcpy(&msg_not_handled, msg_p, sizeof(robin_msg_t));
                    not_handled = EPDK_TRUE;
                    break;

                case SYN_OP_RET_TO_QUIT_ROBIN      :
                    return EPDK_TRUE;

                case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
                case SYN_OP_RET_OK                 :
                case SYN_OP_RET_OVERTIME           :
                default :
                    break;
            }

            break;
        }

        case ROBIN_CMD_PAUSE     :
        {
            //__here__;
            ret = syn_op_pause();

            //__here__;
            switch (ret)
            {
                case SYN_OP_RET_SEND_CMD_ERR       :
                    g_memcpy(&msg_not_handled, msg_p, sizeof(robin_msg_t));
                    not_handled = EPDK_TRUE;
                    break;

                case SYN_OP_RET_TO_QUIT_ROBIN      :
                    return EPDK_TRUE;

                case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
                case SYN_OP_RET_OK                 :
                case SYN_OP_RET_OVERTIME           :
                default :
                    break;
            }

            break;
        }

        case ROBIN_CMD_STOP      :
        {
            //__here__;
            ret = syn_op_stop();

            //__here__;
            switch (ret)
            {
                case SYN_OP_RET_SEND_CMD_ERR       :
                    g_memcpy(&msg_not_handled, msg_p, sizeof(robin_msg_t));
                    not_handled = EPDK_TRUE;
                    break;

                case SYN_OP_RET_TO_QUIT_ROBIN      :
                    return EPDK_TRUE;

                case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
                case SYN_OP_RET_OK                 :
                case SYN_OP_RET_OVERTIME           :
                default :
                    break;
            }

            break;
        }

        case ROBIN_CMD_FF        :
        {
            //__here__;
            ret = syn_op_ff();

            //__here__;
            switch (ret)
            {
                case SYN_OP_RET_SEND_CMD_ERR       :
                    g_memcpy(&msg_not_handled, msg_p, sizeof(robin_msg_t));
                    not_handled = EPDK_TRUE;
                    break;

                case SYN_OP_RET_TO_QUIT_ROBIN      :
                    return EPDK_TRUE;

                case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
                case SYN_OP_RET_OK                 :
                case SYN_OP_RET_OVERTIME           :
                default :
                    break;
            }

            break;
        }

        case ROBIN_CMD_RR        :
        {
            //__here__;
            ret = syn_op_rr();

            //__here__;
            switch (ret)
            {
                case SYN_OP_RET_SEND_CMD_ERR       :
                    g_memcpy(&msg_not_handled, msg_p, sizeof(robin_msg_t));
                    not_handled = EPDK_TRUE;
                    break;

                case SYN_OP_RET_TO_QUIT_ROBIN      :
                    return EPDK_TRUE;

                case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
                case SYN_OP_RET_OK                 :
                case SYN_OP_RET_OVERTIME           :
                default :
                    break;
            }

            break;
        }

        case ROBIN_CMD_JUMP      :
        {
            //__here__;
            //__here__;
            ret = syn_op_jump(msg_p->data);

            switch (ret)
            {
                case SYN_OP_RET_SEND_CMD_ERR       :
                    g_memcpy(&msg_not_handled, msg_p, sizeof(robin_msg_t));
                    not_handled = EPDK_TRUE;
                    break;

                case SYN_OP_RET_TO_QUIT_ROBIN      :
                    return EPDK_TRUE;

                case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
                case SYN_OP_RET_OK                 :
                case SYN_OP_RET_OVERTIME           :
                default :
                    break;
            }

            break;
        }

        default :
        {
            __wrn("%d is NOT a valid robin command.", msg_p->id);
            break;
        }
    }

    return EPDK_FALSE;
}
#endif     //  ifndef __robin_monitor_c

/* end of robin_monitor.c */
