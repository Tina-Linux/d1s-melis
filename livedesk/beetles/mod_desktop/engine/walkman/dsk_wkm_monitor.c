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
#ifndef  __dsk_wkm_monitor_c
#define  __dsk_wkm_monitor_c

#include "dsk_walkman_i.h"
#include <mediainfo/eLIBs_GetAudioFmt.h>

#define MNT_FILE_LEN_MAX                511
static volatile __u32  monitor_id  = 0;
static volatile __bool forbid_autoplay = EPDK_TRUE;
static __bool monitor_has_run = EPDK_FALSE;
static void   monitor(void *arg_p);
static __bool _is_filter_file(const char *file);
static __s32 _request_monitor_res(void);
static void  _release_monitor_res(void);

__s32  dsk_wkm_monitor_create(__u8 prio)
{
    /* create monitor thread */
    if (monitor_id == 0)
    {
        __u32 i;
        monitor_has_run = EPDK_FALSE;
        monitor_id = esKRNL_TCreate(monitor, NULL, 0x2000, (EPOS_curid << 8) | prio);

        if (monitor_id == 0)
        {
            __err("Error in creating \"dsk_walkman monitor\" thread.");
            return -1;
        }

        /* wait monitor to run */
        for (i = 0; i < 100; i++)
        {
            if (monitor_has_run)
            {
                break;
            }

            g_delay(10);
        }

        forbid_autoplay = EPDK_TRUE;
    }

    return 0;
}



void   dsk_wkm_monitor_delete(void)
{
    /* delete monitor thread */
    if (monitor_id != 0)
    {
        while (esKRNL_TDelReq(monitor_id) != OS_TASK_NOT_EXIST)
        {
            g_delay(10);
        }

        monitor_id = 0;
        __msg("\"dsk_walkman monitor\" thread has been deleted.");
    }
}
__s32  __dsk_wkm_monitor_enable(void)
{
    forbid_autoplay = EPDK_FALSE;
    return 0;
}



__s32  __dsk_wkm_monitor_disable(void)
{
    forbid_autoplay = EPDK_TRUE;
    return 0;
}

static void monitor(void *arg_p)
{
    __u8   err;

    /* request resouce */
    if (_request_monitor_res())
    {
        goto exit;
    }

    monitor_has_run = EPDK_TRUE;
    __dsk_wkm_set_open_state(EPDK_TRUE);

    while (1)
    {
        g_pend_mutex(dsk_wkm_res_mutex, &err);

        if (!forbid_autoplay)
        {
            if (__dsk_wkm_get_pause_state() == EPDK_FALSE
                && __dsk_wkm_get_fsm_state() == CEDAR_STAT_STOP)
            {
                __s32  index;

                if (__dsk_wkm_npl_is_dirty() || __dsk_wkm_get_npl_dirty_flag())
                {
                    __dsk_wkm_npl_clear_dirty_flag();
                    __dsk_wkm_set_npl_dirty_flag(EPDK_FALSE);
                    __dsk_wkm_monitor_disable();
                    g_post_mutex(dsk_wkm_res_mutex);
                    continue;
                }

                /* auto play the next */
                index = __dsk_wkm_npl_get_next();

                if (index != -1)
                {
                    char  file[FS_FULL_PATH_LEN_MAX];
                    /* get the file path */
                    __dsk_wkm_npl_index2file(index, file);

                    if (!_is_filter_file(file))
                    {
                        /* call hook */
                        if (cb_auto_play_next != NULL)
                        {
                            esKRNL_CallBack(cb_auto_play_next, cb_ctx_auto_play_next);
                        }

                        /* play */
                        __dsk_wkm_syn_op_play_file(file);
                        eLIBs_strcpy(now_playing_file, file);
                    }
                }
                else
                {
                    if (__dsk_wkm_npl_get_total_count() > 0 && __dsk_wkm_npl_get_cur() != -1)       // no item to play
                    {
                        /* state change to "NOT playing" */
                        forbid_autoplay = EPDK_TRUE;
                        __dsk_wkm_set_playing_state(EPDK_FALSE);
                        __dsk_wkm_set_pause_state(EPDK_FALSE);

                        /* call hook */
                        if (cb_no_item_to_play != NULL)
                        {
                            esKRNL_CallBack(cb_no_item_to_play, cb_ctx_no_item_to_play);
                        }
                    }
                }
            }
        }

        g_post_mutex(dsk_wkm_res_mutex);

        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            goto exit;
        }

        g_delay(20);
    }

exit:
    _release_monitor_res();
    __dsk_wkm_set_open_state(EPDK_FALSE);
    esKRNL_TDel(OS_PRIO_SELF);
}



static __bool _is_filter_file(const char *file)
{
    __sw_audio_format_e audio_format;
    eLIBs_GetAudioFormat(file, (int *)&audio_format);

    if (audio_format == SW_AUDIO_FORMAT_APE
        || audio_format == SW_AUDIO_FORMAT_FLAC
        || audio_format == SW_AUDIO_FORMAT_OGG)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}



static __s32  _request_monitor_res(void)
{
    if (__dsk_wkm_feedbackQ_create())
    {
        goto error;
    }

    if (__dsk_wkm_cedar_open())
    {
        goto error;
    }

    if (__dsk_wkm_get_feedback_msgQ())
    {
        goto error;
    }

    return 0;
error:
    _release_monitor_res();
    return -1;
}



static void  _release_monitor_res(void)
{
    __dsk_wkm_cedar_close();
    dsk_wkm_cedar_msgQ = NULL;
    __dsk_wkm_feedbackQ_delete();
}




#endif     //  ifndef __dsk_wkm_monitor_c

/* end of dsk_wkm_monitor.c */
