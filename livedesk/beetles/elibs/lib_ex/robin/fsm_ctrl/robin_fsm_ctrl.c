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
#ifndef  __robin_fsm_ctrl_c
#define  __robin_fsm_ctrl_c

#include "robin_i.h"
#include <kconfig.h>


/***************************************************************************************************
*Name        : robin_set_cmd_play_file
*Prototype   : __s32  robin_set_cmd_play_file( const char *file, const __cedar_tag_inf_t *tag_info_p )
*Arguments   : file           input. the file to be played.
*              tag_info_p     input. the tag info of the file. If there is no tag info, input should
*                             be NULL.
*Return      : ==0       succeed
*              !=0       fail
*Description : set play a file cmd. This operation will send the "play file" command to FSM but can't
*              enable FSM to be in "play" status immediately, in other words, this is an asynchronous
*              operation.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_play_file(const char *file, const __cedar_tag_inf_t *tag_info_p)
{
    robin_msg_t  *msg_p = NULL;

    if (file == NULL)
    {
        __err("%s filename is null", __FUNCTION__);
        return EPDK_FAIL;
    }

    if (EPDK_FALSE == robin_is_open())
    {
        __err("EPDK_FALSE != robin_is_open()");
        return EPDK_FAIL;
    }

    dsk_set_auto_off_time(0);
    dsk_amplifier_onoff(1);//打开功放

    msg_p = get_robin_msg_buf();
    msg_p->id = ROBIN_CMD_PLAY_FILE;
    g_strcpy(msg_p->file_info.file_path, file);

    if (tag_info_p != NULL)
    {
        g_memcpy(&(msg_p->file_info.tag_inf), tag_info_p, sizeof(__cedar_tag_inf_t));
        msg_p->file_info.tag_inf_validflag = 1;
    }
    else
    {
        msg_p->file_info.tag_inf_validflag = 0;
    }

    g_QFlush(robin_cmdQ);
    g_QPost(robin_cmdQ, (unsigned long)msg_p);
    return 0;
}

/***************************************************************************************************
*Name        : robin_play_file
*Prototype   : __s32  robin_play_file( const char *file, const __cedar_tag_inf_t *tag_info_p )
*Arguments   : file           input. the file to be played.
*              tag_info_p     input. the tag info of the file. If there is no tag info, input should
*                             be NULL.
*Return      : ==0       succeed
*              !=0       fail
*Description :
*              this is an synchronous
*              operation.
*Other       :
***************************************************************************************************/
__s32 robin_play_file(const char *file, const __cedar_tag_inf_t *tag_info_p)
{
    if (EPDK_FALSE == robin_is_open())
    {
        __wrn("robin is not open ....");
        return EPDK_FAIL;
    }

    dsk_set_auto_off_time(0);
    dsk_amplifier_onoff(0);//关闭功放

    /* stop old */
    syn_op_stop();

    dsk_amplifier_onoff(1);//打开功放

    /* play new */
    {
        syn_op_ret_e ret;
        /* play */
        ret = syn_op_play_file(file, tag_info_p);

        switch (ret)
        {
            case SYN_OP_RET_OK                 :
                goto ok;

            case SYN_OP_RET_TO_QUIT_ROBIN      :
                goto ok;

            case SYN_OP_RET_SEND_CMD_ERR       :
                goto error;

            case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
                goto error;

            case SYN_OP_RET_OVERTIME           :
            default :
                goto error;
        }

ok:
        return 0;
error:
        return -1;
    }
}

/***************************************************************************************************
*Name        : robin_set_cmd_play
*Prototype   : __s32  robin_set_cmd_play( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set play cmd. This operation will send the "play" command to FSM and wait the FSM
*              to be in "play" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_play(void)
{
    __s32  ret;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    dsk_set_auto_off_time(0);
    dsk_amplifier_onoff(1);//打开功放

    //__here__;
    g_pend_mutex(robin_cedar_mutex, &err);
    //__here__;
    robin_wait_no_file();
    //__here__;
    ret = syn_op_play();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_ROBIN      :
            goto ok;

        case SYN_OP_RET_SEND_CMD_ERR       :
            goto error;

        case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
            goto error;

        case SYN_OP_RET_OVERTIME           :
        default :
            goto error;
    }

ok:
    g_post_mutex(robin_cedar_mutex);
    return 0;
error:
    g_post_mutex(robin_cedar_mutex);
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_stop
*Prototype   : __s32  robin_set_cmd_stop( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set stop cmd. This operation will send the "stop" command to FSM and wait the FSM
*              to be in "stop" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_stop(void)
{
    __s32  ret;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    {
        reg_system_para_t *para = NULL;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para != NULL)
        {
            dsk_set_auto_off_time(para->poweroff);
            __wrn("para->poweroff=%d", para->poweroff);
        }
    }

    dsk_amplifier_onoff(0);//停止播放时关闭功放

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = syn_op_stop();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_ROBIN      :
            goto ok;

        case SYN_OP_RET_SEND_CMD_ERR       :
            goto error;

        case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
            goto error;

        case SYN_OP_RET_OVERTIME           :
        default :
            goto error;
    }

ok:
    g_post_mutex(robin_cedar_mutex);
    return 0;
error:
    g_post_mutex(robin_cedar_mutex);
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_pause
*Prototype   : __s32  robin_set_cmd_pause( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set pause cmd. This operation will send the "pause" command to FSM and wait the FSM
*              to be in "pause" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_pause(void)
{
    __s32  ret;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        __wrn("robin is not open");
        return EPDK_FAIL;
    }

    {
        reg_system_para_t *para = NULL;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para != NULL)
        {
            dsk_set_auto_off_time(para->poweroff);
            __wrn("para->poweroff=%d", para->poweroff);
        }
    }

    dsk_amplifier_onoff(0);//停止播放时关闭功放

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = syn_op_pause();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_ROBIN      :
            __wrn("SYN_OP_RET_TO_QUIT_ROBIN");
            goto ok;

        case SYN_OP_RET_SEND_CMD_ERR       :
            __wrn("SYN_OP_RET_SEND_CMD_ERR");
            goto error;

        case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
            __wrn("SYN_OP_RET_CEDAR_FEEDBACK_ERR");
            goto error;

        case SYN_OP_RET_OVERTIME           :
            __wrn("SYN_OP_RET_OVERTIME");

        default :
            goto error;
    }

ok:
    g_post_mutex(robin_cedar_mutex);
    return 0;
error:
    g_post_mutex(robin_cedar_mutex);
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_ff
*Prototype   : __s32  robin_set_cmd_ff( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set ff cmd. This operation will send the "ff" command to FSM and wait the FSM
*              to be in "ff" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_ff(void)
{
    __s32  ret;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    dsk_amplifier_onoff(0);//未播放时关闭功放

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = syn_op_ff();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_ROBIN      :
            goto ok;

        case SYN_OP_RET_SEND_CMD_ERR       :
            goto error;

        case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
            goto error;

        case SYN_OP_RET_OVERTIME           :
        default :
            goto error;
    }

ok:
    g_post_mutex(robin_cedar_mutex);
    return 0;
error:
    g_post_mutex(robin_cedar_mutex);
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_rr
*Prototype   : __s32  robin_set_cmd_rr( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set rr cmd. This operation will send the "rr" command to FSM and wait the FSM
*              to be in "rr" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_rr(void)
{
    __s32  ret;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    dsk_amplifier_onoff(0);//未播放时关闭功放

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = syn_op_rr();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_ROBIN      :
            goto ok;

        case SYN_OP_RET_SEND_CMD_ERR       :
            goto error;

        case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
            goto error;

        case SYN_OP_RET_OVERTIME           :
        default :
            goto error;
    }

ok:
    g_post_mutex(robin_cedar_mutex);
    return 0;
error:
    g_post_mutex(robin_cedar_mutex);
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_jump
*Prototype   : __s32  robin_set_cmd_jump( __u32 time )
*Arguments   : time      time. unit : ms.
*Return      : ==0       succeed
*              !=0       fail
*Description : set jump cmd. This operation will send the "jump" command to FSM.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_jump(__u32 time)
{
    __s32  ret;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = syn_op_jump(time);

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_ROBIN      :
            goto ok;

        case SYN_OP_RET_SEND_CMD_ERR       :
            goto error;

        case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
            goto error;

        case SYN_OP_RET_OVERTIME           :
        default :
            goto error;
    }

ok:
    g_post_mutex(robin_cedar_mutex);
    return 0;
error:
    g_post_mutex(robin_cedar_mutex);
    return -1;
}



/***************************************************************************************************
*Name        : robin_get_fsm_status
*Prototype   : __cedar_status_t  robin_get_fsm_status( void )
*Arguments   : void
*Return      : the current status of the fsm, such as "play", "pause" and so on. The status are
*              defined in mod_cedar.h, see __cedar_status_t.
*Description : get the current status of the fsm.
*Other       :
***************************************************************************************************/
__cedar_status_t  robin_get_fsm_status(void)
{
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        __wrn("robin is not open .... ");
        return CEDAR_STAT_;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    g_post_mutex(robin_cedar_mutex);
    __inf("*******before CEDAR_CMD_GET_STATUS*********");
    return (__cedar_status_t)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL);
}



/***************************************************************************************************
*Name        : robin_clear_feedback_msgQ
*Prototype   : void   robin_clear_feedback_msgQ( void )
*Arguments   : void
*Return      : void
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can clear the queue.
*Other       :
***************************************************************************************************/
void   robin_clear_feedback_msgQ(void)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return ;
    }

    if (robin_feedbackQ != NULL)
    {
        g_QFlush(robin_feedbackQ);
    }
}



/***************************************************************************************************
*Name        : robin_get_feedback_msg
*Prototype   : feedback_msg_t  robin_get_feedback_msg( void )
*Arguments   : void
*Return      : error message. 0 means no message at present, while other values are the error
*              messages. The error messages are defined in mod_cedar.h, see __cedar_feedback_msg_t.
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can get the instant error message .
*Other       :
***************************************************************************************************/
feedback_msg_t  robin_get_feedback_msg(void)
{
    feedback_msg_t  ret;
    __u8            err;

    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    //__here__;
    ret = (feedback_msg_t)g_QAccept(robin_feedbackQ, &err);

    //__here__;
    if (ret == NULL)
    {
        return (feedback_msg_t)0;
    }
    else
    {
        return ret;
    }
}



/***************************************************************************************************
*Name        : robin_get_fsm_cur_file
*Prototype   : __s32  robin_get_fsm_cur_file( char *file, __u32 size )
*Arguments   : file      output. a buffer to store the fsm's current file.
*              size      input. The size of the buffer.
*Return      : ==0       succeed.
*              !=0       no file.
*Description : get the fsm's current file.
*Other       :
***************************************************************************************************/
__s32  robin_get_fsm_cur_file(char *file, __u32 size)
{

    if (EPDK_FALSE == robin_is_open())
    {
        __wrn("robin is not open ....");
        return EPDK_FAIL;
    }

    return robin_monitor_get_cur_file(file, size);
}



/***************************************************************************************************
*Name        : robin_fsm_is_busy
*Prototype   : __bool robin_fsm_is_busy( void );
*Arguments   : void
*Return      : EPDK_TRUE   the fsm is busy.
*              EPDK_FALSE  the fsm is not busy.
*Description : check whether the fsm is busy or not.
*Other       :
***************************************************************************************************/
__bool robin_fsm_is_busy(void)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FALSE;
    }

    if (get_msg_nr_of_q(robin_cmdQ) == 0)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}



/***************************************************************************************************
*Name        : robin_enable_raw_data
*Prototype   : __s32 robin_enable_raw_data( __bool flag )
*Arguments   : flag      input. enable or disable flag.
*Return      : ==0       succeed
*              !=0       fail
*Description : enable or disable raw data output.
*Other       :
***************************************************************************************************/
__s32 robin_enable_raw_data(__bool flag)
{
    __u8   err;
    __s32  ret;

    if (EPDK_FALSE == robin_is_open())
    {
        __err("robin not open");
        return EPDK_FAIL;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_AUDIO_RAW_DATA_ENABLE, flag, NULL);
    g_post_mutex(robin_cedar_mutex);
    return ret;
}

__s32 robin_play_aux_file(const char *file, __cedar_play_aux_wav_mode_t mode)
{
    __u8   err;
    __s32  ret;
    char path[1024] = {0};

    if (EPDK_FALSE == robin_is_open())
    {
        __err("robin not open");
        return EPDK_FAIL;
    }

    if (NULL == file)
    {
        __err("file name is empty...");
        return EPDK_FAIL;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    eLIBs_strcpy(path, file);
    ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_PLAY_AUX_WAV_FILE, mode, (void *)path);
    g_post_mutex(robin_cedar_mutex);
    return ret;
}
#endif //  ifndef __robin_fsm_ctrl_c
/* end of robin_fsm_ctrl.c */
