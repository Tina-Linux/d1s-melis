/*
************************************************************************************************************************
*                                                        dsk_walkman
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : __dsk_wkm_syn_op.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.22
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
* Gary.Wang      2009.11.22       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  ____dsk_wkm_syn_op_c
#define  ____dsk_wkm_syn_op_c


#include "foundation_i.h"


#define OVERTIME_THRESHOLD              200



syn_op_ret_e __dsk_wkm_syn_op_play_file(const char *file)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    /* clear cedar message Queue */
    g_QFlush(dsk_wkm_cedar_msgQ);
    /* set new media file to be played */
    esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_MEDIAFILE, 0, (void *)file);
    /* send play command */
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_PLAY, 0, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting play cmd.\n");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_PLAY)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(dsk_wkm_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(dsk_wkm_feedbackQ);
                g_QPost(dsk_wkm_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_dsk_wkm)
        {
            return SYN_OP_RET_TO_QUIT_DSK_WKM;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



syn_op_ret_e __dsk_wkm_syn_op_stop(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

    if (ret == CEDAR_STAT_STOP)
    {
        return SYN_OP_RET_OK;
    }

    /* clear cedar message Queue */
    g_QFlush(dsk_wkm_cedar_msgQ);
    /* send stop command */
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_STOP, 0, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting stop cmd.\n");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_STOP)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(dsk_wkm_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(dsk_wkm_feedbackQ);
                g_QPost(dsk_wkm_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_dsk_wkm)
        {
            return SYN_OP_RET_TO_QUIT_DSK_WKM;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



syn_op_ret_e __dsk_wkm_syn_op_resume_play(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

    if (ret == CEDAR_STAT_PLAY)
    {
        return SYN_OP_RET_OK;
    }

    /* clear cedar message Queue */
    g_QFlush(dsk_wkm_cedar_msgQ);
    /* send play command */
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_PLAY, 0, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting stop cmd.\n");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_PLAY)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(dsk_wkm_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(dsk_wkm_feedbackQ);
                g_QPost(dsk_wkm_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_dsk_wkm)
        {
            return SYN_OP_RET_TO_QUIT_DSK_WKM;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



syn_op_ret_e __dsk_wkm_syn_op_ff(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

    if (ret == CEDAR_STAT_FF)
    {
        return SYN_OP_RET_OK;
    }

    /* clear cedar message Queue */
    g_QFlush(dsk_wkm_cedar_msgQ);
    /* send ff command */
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_FF, 0, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting stop cmd.\n");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_FF)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(dsk_wkm_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(dsk_wkm_feedbackQ);
                g_QPost(dsk_wkm_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_dsk_wkm)
        {
            return SYN_OP_RET_TO_QUIT_DSK_WKM;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



syn_op_ret_e __dsk_wkm_syn_op_rr(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

    if (ret == CEDAR_STAT_RR)
    {
        return SYN_OP_RET_OK;
    }

    /* clear cedar message Queue */
    g_QFlush(dsk_wkm_cedar_msgQ);
    /* send rr command */
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_REV, 0, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting stop cmd.\n");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_RR)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(dsk_wkm_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(dsk_wkm_feedbackQ);
                g_QPost(dsk_wkm_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_dsk_wkm)
        {
            return SYN_OP_RET_TO_QUIT_DSK_WKM;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



syn_op_ret_e __dsk_wkm_syn_op_pause(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

    if (ret == CEDAR_STAT_PAUSE)
    {
        return SYN_OP_RET_OK;
    }

    /* clear cedar message Queue */
    g_QFlush(dsk_wkm_cedar_msgQ);
    /* send pause command */
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_PAUSE, 0, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting stop cmd.\n");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_PAUSE)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(dsk_wkm_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(dsk_wkm_feedbackQ);
                g_QPost(dsk_wkm_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_dsk_wkm)
        {
            return SYN_OP_RET_TO_QUIT_DSK_WKM;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



syn_op_ret_e __dsk_wkm_syn_op_jump(__u32 time)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    /* clear cedar message Queue */
    g_QFlush(dsk_wkm_cedar_msgQ);
    /* send jump command */
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_JUMP, time, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting stop cmd.\n");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_PLAY || ret == CEDAR_STAT_STOP)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(dsk_wkm_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(dsk_wkm_feedbackQ);
                g_QPost(dsk_wkm_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_dsk_wkm)
        {
            return SYN_OP_RET_TO_QUIT_DSK_WKM;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



#endif     //  ifndef ____dsk_wkm_syn_op_c

/* end of __dsk_wkm_syn_op.c */
