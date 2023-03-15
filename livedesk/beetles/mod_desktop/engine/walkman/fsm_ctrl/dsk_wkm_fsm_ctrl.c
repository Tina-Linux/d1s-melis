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
#ifndef  __DSK_WKM_FSM_CTRL_C__
#define  __DSK_WKM_FSM_CTRL_C__


#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : dsk_wkm_get_fsm_state
*Prototype   : __cedar_status_t dsk_wkm_get_fsm_state( void )
*Arguments   : void
*Return      : the status of the walkman's finite status machine.
*Description : get the status of the walkman's finite status machine.
*Other       :
****************************************************************************************************
*/
__cedar_status_t dsk_wkm_get_fsm_state(void)
{
    __cedar_status_t  ret;
    __u8              err;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return CEDAR_STAT_STOP;
    }

    if (__dsk_wkm_get_playing_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT playing.\n");
        return CEDAR_STAT_STOP;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    /* get cedar status */
    ret = __dsk_wkm_get_fsm_state();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_play_file
*Prototype   : __s32 dsk_wkm_play_file( const char *file )
*Arguments   : file    input. the file to be played. full path.
*Return      : ==0     succeed
*              !=0     fail
*Description : play a file, and then the walkman is in "playing" status.
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_play_file(const char *file)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    /* disable monitor */
    __dsk_wkm_monitor_disable();
    ret = __dsk_wkm_play_file(file);
    __msg("ret = %d\n", ret);
    /* enable monitor */
    __dsk_wkm_monitor_enable();
    /* set playing state */
    __dsk_wkm_set_playing_state(EPDK_TRUE);
    __dsk_wkm_set_pause_state(EPDK_FALSE);
    __dsk_wkm_set_npl_dirty_flag(EPDK_FALSE);
    eLIBs_strcpy(now_playing_file, file);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}

__s32 dsk_wkm_play_next(void)
{
    __s32   ret;
    __s32   index;
    char    filename[768];
    orc_play_mode_e play_mode = dsk_wkm_get_play_mode();

    switch (play_mode)
    {
        case ORC_PLAY_MODE_RANDOM:
        {
            index = dsk_wkm_npl_get_next();
            break;
        }

        case ORC_PLAY_MODE_ONLY_ONCE :
        case ORC_PLAY_MODE_ROTATE_ONE:
        case ORC_PLAY_MODE_ROTATE_ALL:
        case ORC_PLAY_MODE_SEQUENCE  :
        default:
        {
            index = dsk_wkm_npl_get_cur() + 1;

            if (index > dsk_wkm_npl_get_total_count() - 1)
            {
                index = 0;
            }

            break;
        }
    }

    dsk_wkm_npl_index2file(index, filename);
    ret = dsk_wkm_play_file(filename);
    return ret;
}

__s32 dsk_wkm_play_prev(void)
{
    __s32   ret;
    __s32   index;
    char    filename[768];
    orc_play_mode_e play_mode = dsk_wkm_get_play_mode();

    switch (play_mode)
    {
        case ORC_PLAY_MODE_RANDOM:
        {
            index = dsk_wkm_npl_get_next();
            break;
        }

        case ORC_PLAY_MODE_ONLY_ONCE :
        case ORC_PLAY_MODE_ROTATE_ONE:
        case ORC_PLAY_MODE_ROTATE_ALL:
        case ORC_PLAY_MODE_SEQUENCE  :
        default:
        {
            index = dsk_wkm_npl_get_cur() - 1;

            if (index < 0)
            {
                index = dsk_wkm_npl_get_total_count() - 1;
            }

            break;
        }
    }

    dsk_wkm_npl_index2file(index, filename);
    ret = dsk_wkm_play_file(filename);
    return ret;
}

/*
****************************************************************************************************
*Name        : dsk_wkm_stop
*Prototype   : __s32 dsk_wkm_stop( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : stop playing, and then the walkman is in "NOT playing" status.
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_stop(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    /* disable monitor */
    __dsk_wkm_monitor_disable();
    /* stop */
    ret = __dsk_wkm_stop();
    /* set playing state */
    __dsk_wkm_set_playing_state(EPDK_FALSE);
    __dsk_wkm_set_pause_state(EPDK_FALSE);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_pause
*Prototype   : __s32 dsk_wkm_pause( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : pause playing
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_pause(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    if (__dsk_wkm_get_playing_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT playing.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    __dsk_wkm_set_pause_state(EPDK_TRUE);
    /* pause */
    ret = __dsk_wkm_pause();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_resume_play
*Prototype   : __s32 dsk_wkm_resume_play( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : resume playing
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_resume_play(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    if (__dsk_wkm_get_playing_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT playing.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    __dsk_wkm_set_pause_state(EPDK_FALSE);
    /* resume play */
    ret = __dsk_wkm_resume_play();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_ff
*Prototype   : __s32 dsk_wkm_ff( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : start ff
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_ff(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    if (__dsk_wkm_get_playing_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT playing.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    /* ff */
    ret = __dsk_wkm_ff();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_rr
*Prototype   : __s32 dsk_wkm_rr( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : start rr
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_rr(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    if (__dsk_wkm_get_playing_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT playing.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    /* rr */
    ret = __dsk_wkm_rr();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_jump
*Prototype   : __s32 dsk_wkm_jump( __u32 time )
*Arguments   : time    input. time, unit:ms.
*Return      : ==0     succeed
*              !=0     fail
*Description : start jump
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_jump(__u32 time)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    if (__dsk_wkm_get_playing_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT playing.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    /* jump */
    ret = __dsk_wkm_jump(time);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_is_playing
*Prototype   : __bool dsk_wkm_is_playing( void )
*Arguments   : void
*Return      : if some file is being played, return EPDK_TRUE; else return EPDK_FALSE.
*Description : check whether the desktop walkman is playing or not.
*Other       :
****************************************************************************************************
*/
__bool dsk_wkm_is_playing(void)
{
    if (__dsk_wkm_get_playing_state() == EPDK_TRUE)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}


__bool dsk_wkm_get_pause_state(void)
{
    __u8   err;
    __bool state;
    g_pend_mutex(dsk_wkm_res_mutex, &err);
    state = __dsk_wkm_get_pause_state();
    g_post_mutex(dsk_wkm_res_mutex);
    return state;
}

/***************************************************************************************************
*Name        : dsk_wkm_clear_feedback_msgQ
*Prototype   : void   dsk_wkm_clear_feedback_msgQ( void )
*Arguments   : void
*Return      : void
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can clear the queue.
*Other       :
***************************************************************************************************/
void   dsk_wkm_clear_feedback_msgQ(void)
{
    __u8            err;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    __dsk_wkm_clear_feedback_msgQ();
    g_post_mutex(dsk_wkm_res_mutex);
}



/***************************************************************************************************
*Name        : dsk_wkm_get_feedback_msg
*Prototype   : feedback_msg_t  dsk_wkm_get_feedback_msg( void )
*Arguments   : void
*Return      : error message. 0 means no message at present, while other values are the error
*              messages. The error messages are defined in mod_cedar.h, see __cedar_feedback_msg_t.
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can get the instant error message .
*Other       :
***************************************************************************************************/
feedback_msg_t  dsk_wkm_get_feedback_msg(void)
{
    feedback_msg_t  ret;
    __u8            err;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return (feedback_msg_t)0;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_feedback_msg();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



#endif     //  ifndef __DSK_WKM_FSM_CTRL_C__

/* end of dsk_wkm_fsm_ctrl.c */
