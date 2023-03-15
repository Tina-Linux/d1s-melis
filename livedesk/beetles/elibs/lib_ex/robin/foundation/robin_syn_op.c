/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef  __robin_syn_op_c
#define  __robin_syn_op_c


#include "foundation_i.h"
#include <kconfig.h>

#define OVERTIME_THRESHOLD              120

extern char g_robin_current_file[ROBIN_MNT_FILE_LEN_MAX];


__s32 syn_op_play(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    /* clear cedar message Queue */
    g_QFlush(robin_cedar_msgQ);
    __msg("before CEDAR_CMD_PLAY");
    /* send play command */

    ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_PLAY, 0, NULL);
    __msg("after CEDAR_CMD_PLAY");

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting play cmd.");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        __msg("********before CEDAR_CMD_GET_STATUS************");
        ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL);
        __msg("********after CEDAR_CMD_GET_STATUS************");

        //__here__;
        if (ret == CEDAR_STAT_PLAY)
        {
            //__here__;
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(robin_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                //__here__;
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(robin_feedbackQ);
                g_QPost(robin_feedbackQ, (unsigned long)msg);          // only feed back error message
                //__here__;
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_robin)
        {
            //__here__;
            return SYN_OP_RET_TO_QUIT_ROBIN;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            //__here__;
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}

__s32 syn_op_play_file(const char *file, const __cedar_tag_inf_t *tag_info_p)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    __s32  len;
    __cedar_media_file_inf  file_info;
    g_strcpy(file_info.file_path, file);

    if (tag_info_p != NULL)
    {
        g_memcpy(&(file_info.tag_inf), tag_info_p, sizeof(__cedar_tag_inf_t));
        file_info.tag_inf_validflag = 1;
    }
    else
    {
        file_info.tag_inf_validflag = 0;
    }

    /* clear cedar message Queue */
    g_QFlush(robin_cedar_msgQ);
    __msg("before CEDAR_CMD_PLAY");
    /* set new media file to be played */
    esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_MEDIAFILE, 0, (void *)&file_info);
    /* save the current media file */
    len = min(ROBIN_MNT_FILE_LEN_MAX - 1, g_strlen(file));
    g_strncpy(g_robin_current_file, file, len);
    g_robin_current_file[len] = '\0';
    /* send play command */
    ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_PLAY, 0, NULL);
    __msg("after CEDAR_CMD_PLAY");

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting play cmd.");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        __msg("********before CEDAR_CMD_GET_STATUS************");
        ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL);
        __msg("********after CEDAR_CMD_GET_STATUS************");
        //__here__;

        if (ret == CEDAR_STAT_PLAY)
        {
            //__here__;
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(robin_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                //__here__;
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(robin_feedbackQ);
                g_QPost(robin_feedbackQ, (unsigned long)msg);          // only feed back error message
                //__here__;
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_robin)
        {
            //__here__;
            return SYN_OP_RET_TO_QUIT_ROBIN;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            //__here__;
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}

__s32 syn_op_stop(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    /* send stop command */

    if (0 != robin_hced)
    {
        ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_STOP, 0, NULL);

        if (ret != EPDK_OK)
        {
            __wrn("Fail in setting stop cmd.");
            return SYN_OP_RET_SEND_CMD_ERR;
        }
    }

    /* clear cedar message Queue */
    if (0 != robin_cedar_msgQ)
    {
        g_QFlush(robin_cedar_msgQ);
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        if (0 == robin_hced)
        {
            __wrn("robin_hced = 0 ");
            return SYN_OP_RET_OK;
        }

        ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_STOP)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(robin_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(robin_feedbackQ);
                g_QPost(robin_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_robin)
        {
            return SYN_OP_RET_TO_QUIT_ROBIN;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}


__s32 syn_op_pause(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    /* clear cedar message Queue */
    g_QFlush(robin_cedar_msgQ);
    /* send pause command */
    ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_PAUSE, 0, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting pause cmd.");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_PAUSE)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(robin_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(robin_feedbackQ);
                g_QPost(robin_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_robin)
        {
            return SYN_OP_RET_TO_QUIT_ROBIN;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



__s32 syn_op_ff(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    /* clear cedar message Queue */
    g_QFlush(robin_cedar_msgQ);
    /* send ff command */
    ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_FF, 0, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting ff cmd.");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_FF)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(robin_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(robin_feedbackQ);
                g_QPost(robin_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_robin)
        {
            return SYN_OP_RET_TO_QUIT_ROBIN;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



__s32 syn_op_rr(void)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    /* clear cedar message Queue */
    g_QFlush(robin_cedar_msgQ);
    /* send rr command */
    ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_REV, 0, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting rr cmd.");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_RR)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(robin_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(robin_feedbackQ);
                g_QPost(robin_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_robin)
        {
            return SYN_OP_RET_TO_QUIT_ROBIN;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }
}



__s32 syn_op_jump(__u32 time)
{
    __s32  ret;
    __u8   err;
    long  msg;
    __u32  counter;
    /* clear cedar message Queue */
    g_QFlush(robin_cedar_msgQ);
    /* send play command */
    ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_JUMP, time, NULL);

    if (ret != EPDK_OK)
    {
        __wrn("Fail in setting jump cmd.");
        return SYN_OP_RET_SEND_CMD_ERR;
    }

    /* check cedar status until some message is gotten */
    counter = 0;

    while (1)
    {
        ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_PLAY || ret == CEDAR_STAT_STOP)
        {
            return SYN_OP_RET_OK;
        }

        msg = (long)g_QAccept(robin_cedar_msgQ, &err);

        if (msg != NULL)
        {
            /* feedback msg */
            if (msg == CEDAR_FEDBAK_NO_ERROR)
            {
                return SYN_OP_RET_OK;
            }
            else
            {
                g_QFlush(robin_feedbackQ);
                g_QPost(robin_feedbackQ, (unsigned long)msg);          // only feed back error message
                return SYN_OP_RET_CEDAR_FEEDBACK_ERR;
            }
        }

        if (to_quit_robin)
        {
            return SYN_OP_RET_TO_QUIT_ROBIN;
        }

        if (++counter >= OVERTIME_THRESHOLD)
        {
            return SYN_OP_RET_OVERTIME;
        }

        g_delay(5);
    }

    return SYN_OP_RET_OK;
}




#endif     //  ifndef __robin_syn_op_c

/* end of robin_syn_op.c */
