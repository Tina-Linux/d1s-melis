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
#ifndef  __DSK_WALKMAN_I_C__
#define  __DSK_WALKMAN_I_C__


#include "dsk_walkman_i.h"


__mp      *volatile   dsk_wkm_horc            = NULL;
HNPL        volatile    dsk_wkm_hnpl            = NULL;
__mp        *volatile   dsk_wkm_hced            = NULL;
g_Queue     volatile    dsk_wkm_cedar_msgQ      = NULL;
g_mutex_t   volatile    dsk_wkm_res_mutex       = NULL;

__pCBK_t    volatile    cb_auto_play_next       = NULL;
void        *volatile   cb_ctx_auto_play_next   = NULL;
__pCBK_t    volatile    cb_no_item_to_play      = NULL;
void        *volatile   cb_ctx_no_item_to_play  = NULL;

void        *volatile   dsk_wkm_attr            = NULL;

__bool      volatile    to_quit_dsk_wkm         = EPDK_FALSE;

char                    now_playing_file[FS_FULL_PATH_LEN_MAX];

static __u8   volatile  mid_ced = 0;
static __u8   volatile  mid_wlo = 0;
static __bool volatile  walkman_is_open    = EPDK_FALSE;
static __bool volatile  walkman_is_playing = EPDK_FALSE;
static __bool volatile  walkman_is_pause = EPDK_FALSE;
static __bool volatile  npl_is_dirty = EPDK_FALSE;


/*
****************************************************************************************************
*Name        : __dsk_wkm_orchid_open
*Prototype   : __s32 __dsk_wkm_orchid_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open orchid
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_orchid_open(void)
{
    if (dsk_wkm_horc == NULL)
    {

#if 0 /*langaojie temp test*/ // remove wrn
        dsk_wkm_horc = g_fopen(ORCHID_FILE_PATH, "r+");
#endif
        if (dsk_wkm_horc == NULL)
        {
            __err("Error in opening orchid!\n");
            return -1;
        }

        /* check valid */
        g_fioctrl(dsk_wkm_horc, ORC_CMD_ORCHID_IS_VALID, 0, 0);
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_orchid_close
*Prototype   : void  __dsk_wkm_orchid_close( void )
*Arguments   : void
*Return      : void
*Description : close orchid
*Other       :
****************************************************************************************************
*/
void  __dsk_wkm_orchid_close(void)
{
    if (dsk_wkm_horc != NULL)
    {
#if 0/*langaojie temp test*/  //remove wrn
        //g_fclose(dsk_wkm_horc);
#endif
        dsk_wkm_horc = NULL;
    }
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_create
*Prototype   : __s32 __dsk_wkm_npl_create( orc_media_type_e media_type, orc_npl_work_mode_e  work_mode )
*Arguments   : media_type     input. media type, such as audio, video and so on
*              work_mode      input. work mode.
*Return      : ==0    succeed
*              !=0    fail
*Description : create now playlist
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_npl_create(orc_media_type_e  media_type, orc_npl_work_mode_e  work_mode)
{
    if (dsk_wkm_hnpl == NULL)
    {
        orc_arg_npl_t  arg;
        arg.media_type = media_type;
        arg.work_mode  = work_mode;
        dsk_wkm_hnpl = (HNPL)g_fioctrl(dsk_wkm_horc, ORC_CMD_CONSTRUCT_NOW_PLAYLIST, 0, (void *)&arg);

        if (dsk_wkm_hnpl == NULL)
        {
            __err("Error in opening now play list!\n");
            return -1;
        }
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_delete
*Prototype   : void  __dsk_wkm_npl_delete( void )
*Arguments   : void
*Return      : void
*Description : delete now playlist
*Other       :
****************************************************************************************************
*/
void  __dsk_wkm_npl_delete(void)
{
    if (dsk_wkm_hnpl != NULL)
    {
        g_fioctrl(dsk_wkm_horc, ORC_CMD_DESTROY_NOW_PLAYLIST, 0, (void *)dsk_wkm_hnpl);
        dsk_wkm_hnpl = NULL;
    }
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_cedar_open
*Prototype   : __s32 __dsk_wkm_cedar_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open cedar
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_cedar_open(void)
{
    if (dsk_wkm_hced == NULL)
    {
        /* load cedar */
        mid_ced = esMODS_MInstall(CEDAR_FILE_PATH, 0);

        if (mid_ced == 0)
        {
            __err("Error in loading cedar module!\n");
            return -1;
        }

        dsk_wkm_hced = esMODS_MOpen(mid_ced, 0);

        if (dsk_wkm_hced == NULL)
        {
            __err("Error in opening a handle of cedar!\n");
            esMODS_MUninstall(mid_ced);
            mid_ced = 0;
            return -1;
        }
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_cedar_close
*Prototype   : void  __dsk_wkm_cedar_close( void )
*Arguments   : void
*Return      : void
*Description : close cedar
*Other       :
****************************************************************************************************
*/
void  __dsk_wkm_cedar_close(void)
{
    if (dsk_wkm_hced != NULL)
    {
        esMODS_MClose(dsk_wkm_hced);
        dsk_wkm_hced = NULL;
    }

    if (mid_ced != 0)
    {
        esMODS_MUninstall(mid_ced);
        mid_ced = 0;
    }
}



/***************************************************************************************************
*Name        : __dsk_wkm_get_feedback_msgQ
*Prototype   : __s32  __dsk_wkm_get_feedback_msgQ( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : get the feedback message queue handle.
*Other       :
***************************************************************************************************/
__s32  __dsk_wkm_get_feedback_msgQ(void)
{
    if (dsk_wkm_cedar_msgQ == NULL)
    {
        dsk_wkm_cedar_msgQ = (g_Queue) (unsigned long)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_MESSAGE_CHN, 0, NULL);

        if (dsk_wkm_cedar_msgQ == NULL)
        {
            __err("Error in getting cedar error channel.\n");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : __dsk_wkm_res_mutex_create
*Prototype   : __s32 __dsk_wkm_res_mutex_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : create cedar mutex
*Other       :
***************************************************************************************************/
__s32 __dsk_wkm_res_mutex_create(void)
{
    if (dsk_wkm_res_mutex == NULL)
    {
        dsk_wkm_res_mutex = g_create_mutex();

        if (dsk_wkm_res_mutex == NULL)
        {
            __err("Error in creating dsk_wkm_res_mutex!\n");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : __dsk_wkm_res_mutex_delete
*Prototype   : void  __dsk_wkm_res_mutex_delete( void )
*Arguments   : void
*Return      : void
*Description : delete cedar mutex
*Other       :
***************************************************************************************************/
void  __dsk_wkm_res_mutex_delete(void)
{
    __u8  err;

    if (dsk_wkm_res_mutex != NULL)
    {
        g_delete_mutex(dsk_wkm_res_mutex, &err);
        dsk_wkm_res_mutex = NULL;
    }
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_set_open_state
*Prototype   : void  __dsk_wkm_set_open_state( __bool is_open )
*Arguments   : is_open  input. the walkman's open state.
*Return      : void
*Description : record the open state.
*Other       :
****************************************************************************************************
*/
void  __dsk_wkm_set_open_state(__bool is_open)
{
    walkman_is_open = is_open;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_get_open_state
*Prototype   : __bool  __dsk_wkm_get_open_state( void )
*Arguments   : void
*Return      : the walkman's open state
*Description : get the open state.
*Other       :
****************************************************************************************************
*/
__bool  __dsk_wkm_get_open_state(void)
{
    return walkman_is_open;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_set_playing_state
*Prototype   : void  __dsk_wkm_set_pause_state( __bool is_pause )
*Arguments   : is_pause  input. the walkman's pause state.
*Return      : void
*Description : record the pause state.
*Other       :
****************************************************************************************************
*/
void  __dsk_wkm_set_pause_state(__bool is_pause)
{
    walkman_is_pause = is_pause;
}

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_open_state
*Prototype   : __bool  __dsk_wkm_get_pause_state( void )
*Arguments   : void
*Return      : the walkman's open state
*Description : get the pause state.
*Other       :
****************************************************************************************************
*/
__bool  __dsk_wkm_get_pause_state(void)
{
    return walkman_is_pause;
}


/*
****************************************************************************************************
*Name        : __dsk_wkm_set_playing_state
*Prototype   : void  __dsk_wkm_set_playing_state( __bool is_playing )
*Arguments   : is_playing  input. the walkman's play state.
*Return      : void
*Description : record the play state.
*Other       :
****************************************************************************************************
*/
void  __dsk_wkm_set_playing_state(__bool is_playing)
{
    walkman_is_playing = is_playing;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_get_playing_state
*Prototype   : __bool  __dsk_wkm_get_playing_state( void )
*Arguments   : void
*Return      : the walkman's play state
*Description : get the play state.
*Other       :
****************************************************************************************************
*/
__bool  __dsk_wkm_get_playing_state(void)
{
    return walkman_is_playing;
}

__s32 __dsk_wkm_set_attr(void *attr)
{
    dsk_wkm_attr = attr;
    return 0;
}

void *__dsk_wkm_get_attr(void)
{
    return dsk_wkm_attr;
}

void  __dsk_wkm_set_npl_dirty_flag(__bool is_dirty)
{
    npl_is_dirty = is_dirty;
}


__bool  __dsk_wkm_get_npl_dirty_flag(void)
{
    return npl_is_dirty;
}
#endif     //  ifndef __DSK_WALKMAN_I_C__

/* end of dsk_walkman_i.c */
