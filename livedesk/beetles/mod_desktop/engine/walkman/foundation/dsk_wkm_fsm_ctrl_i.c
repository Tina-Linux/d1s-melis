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
#ifndef  __DSK_WKM_FSM_CTRL_I_C__
#define  __DSK_WKM_FSM_CTRL_I_C__


#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_get_fsm_state
*Prototype   : __cedar_status_t __dsk_wkm_get_fsm_state( void )
*Arguments   : void
*Return      : the status of the walkman's finite status machine.
*Description : get the status of the walkman's finite status machine.
*Other       :
****************************************************************************************************
*/
__cedar_status_t __dsk_wkm_get_fsm_state(void)
{
    return (__cedar_status_t)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_STATUS, 0, NULL);
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_play_file
*Prototype   : __s32 __dsk_wkm_play_file( const char *file )
*Arguments   : file    input. the file to be played. full path.
*Return      : ==0     succeed
*              !=0     fail
*Description : play a file, and then the walkman is in "playing" status.
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_play_file(const char *file)
{
    /* stop old */
    __dsk_wkm_syn_op_stop();
    /* play new */
    {
        __s32  index;
        syn_op_ret_e ret;
        /* get index */
        index = __dsk_wkm_npl_file2index(file);

        if (index == -1)                          // not find the file in the now play list
        {
            //__dsk_wkm_npl_add_item( file );
            //index = __dsk_wkm_npl_file2index( file );
            __wrn("the file %s is not in npl!\n", file);
        }
        else
        {
            /* seek playing pointer */
            __dsk_wkm_npl_playing_seek(SEEK_SET, index);
        }

        /* play */
        ret = __dsk_wkm_syn_op_play_file(file);

        switch (ret)
        {
            case SYN_OP_RET_OK                 :
                goto ok;

            case SYN_OP_RET_TO_QUIT_DSK_WKM      :
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


/*
****************************************************************************************************
*Name        : __dsk_wkm_stop
*Prototype   : __s32 __dsk_wkm_stop( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : stop playing, and then the walkman is in "NOT playing" status.
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_stop(void)
{
    syn_op_ret_e ret;
    ret = __dsk_wkm_syn_op_stop();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_DSK_WKM      :
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



/*
****************************************************************************************************
*Name        : __dsk_wkm_pause
*Prototype   : __s32 __dsk_wkm_pause( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : pause playing
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_pause(void)
{
    syn_op_ret_e ret;
    ret = __dsk_wkm_syn_op_pause();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_DSK_WKM      :
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



/*
****************************************************************************************************
*Name        : __dsk_wkm_resume_play
*Prototype   : __s32 __dsk_wkm_resume_play( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : resume playing
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_resume_play(void)
{
    syn_op_ret_e ret;
    ret = __dsk_wkm_syn_op_resume_play();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_DSK_WKM      :
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



/*
****************************************************************************************************
*Name        : __dsk_wkm_ff
*Prototype   : __s32 __dsk_wkm_ff( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : start ff
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_ff(void)
{
    syn_op_ret_e ret;
    ret = __dsk_wkm_syn_op_ff();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_DSK_WKM      :
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



/*
****************************************************************************************************
*Name        : __dsk_wkm_rr
*Prototype   : __s32 __dsk_wkm_rr( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : start rr
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_rr(void)
{
    syn_op_ret_e ret;
    ret = __dsk_wkm_syn_op_rr();

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_DSK_WKM      :
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



/*
****************************************************************************************************
*Name        : __dsk_wkm_jump
*Prototype   : __s32 __dsk_wkm_jump( __u32 time )
*Arguments   : time    input. time, unit:ms.
*Return      : ==0     succeed
*              !=0     fail
*Description : start jump
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_jump(__u32 time)
{
    syn_op_ret_e ret;
    ret = __dsk_wkm_syn_op_jump(time);

    switch (ret)
    {
        case SYN_OP_RET_OK                 :
            goto ok;

        case SYN_OP_RET_TO_QUIT_DSK_WKM      :
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



/***************************************************************************************************
*Name        : __dsk_wkm_clear_feedback_msgQ
*Prototype   : void   __dsk_wkm_clear_feedback_msgQ( void )
*Arguments   : void
*Return      : void
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can clear the queue.
*Other       :
***************************************************************************************************/
void   __dsk_wkm_clear_feedback_msgQ(void)
{
    if (dsk_wkm_feedbackQ != NULL)
    {
        g_QFlush(dsk_wkm_feedbackQ);
    }
}



/***************************************************************************************************
*Name        : __dsk_wkm_get_feedback_msg
*Prototype   : feedback_msg_t  __dsk_wkm_get_feedback_msg( void )
*Arguments   : void
*Return      : error message. 0 means no message at present, while other values are the error
*              messages. The error messages are defined in mod_cedar.h, see __cedar_feedback_msg_t.
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can get the instant error message .
*Other       :
***************************************************************************************************/
feedback_msg_t  __dsk_wkm_get_feedback_msg(void)
{
    feedback_msg_t  ret;
    __u8            err;
    ret = (feedback_msg_t)g_QAccept(dsk_wkm_feedbackQ, &err);

    if (ret == NULL)
    {
        return (feedback_msg_t)0;
    }
    else
    {
        return ret;
    }
}



#endif     //  ifndef __DSK_WKM_FSM_CTRL_I_C__

/* end of dsk_wkm_fsm_ctrl_i.c */
