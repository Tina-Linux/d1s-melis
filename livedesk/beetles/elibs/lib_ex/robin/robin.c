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
#ifndef  __robin_c
#define  __robin_c


#include "robin_i.h"
#include <kconfig.h>

#define ROBIN_MONITOR_PRIO              KRNL_priolevel3


static robin_mode_e  cur_robin_mode = ROBIN_MODE_UNKNOWN;

static __bool   robin_open_state  = EPDK_FALSE;


static void  _release_res(void);


/***************************************************************************************************
*Name        : robin_open
*Prototype   : __s32  robin_open( robin_mode_e robin_mode, void *arg_p )
*Arguments   : robin_mode    input. the work mode of the robin module.
*              arg_p       input. arguments, should be NULL at present.
*Return      : ==0    succeed
*              !=0    fail
*Description : open robin.
*Other       :
***************************************************************************************************/
__s32  robin_open(robin_mode_e robin_mode, robin_open_arg_t *arg_p)
{
    if (EPDK_TRUE == robin_is_open())
    {
        __wrn("robin already open ");
        return EPDK_OK;
    }

    if (arg_p)
    {
        if (0 == arg_p->reserve_mem_size)
        {
            arg_p->reserve_mem_size = 512 * 1024;
        }
        else
        {
            __wrn("reserve_mem_size != 0 ");
        }
    }

    __wrn("-----robin_mode = %d--------", robin_mode);

    switch (robin_mode)
    {
        case ROBIN_MODE_AUDIO_MIN :
        {
            __wrn("===============ROBIN_MODE_AUDIO_MIN=========");
            npl_delete();

            if (npl_create(RAT_MEDIA_TYPE_AUDIO))
            {
                __wrn("=======================================================");
                goto error;
            }

            if (cedar_open(arg_p))
            {
                __wrn("=======================================================");
                goto error;
            }

            if (get_feedback_msgQ())
            {
                __wrn("=======================================================");
                goto error;
            }

            if (cmdQ_create())
            {
                __wrn("=======================================================");
                goto error;
            }

            if (robin_feedbackQ_create())
            {
                __wrn("=======================================================");
                goto error;
            }

            if (robin_monitor_create(ROBIN_MONITOR_PRIO))
            {
                __wrn("=======================================================");
                goto error;
            }

            if (robin_cedar_mutex_create())
            {
                __wrn("=======================================================");
                goto error;
            }

            if (robin_cmd_mutex_create())
            {
                __wrn("=======================================================");
                goto error;
            }

            __wrn("=========================end==============================");
        }
        break;

        case ROBIN_MODE_AUDIO_MAX :
            __wrn("===============ROBIN_MODE_AUDIO_MAX=========");
            npl_delete();

            if (npl_create(RAT_MEDIA_TYPE_AUDIO))
            {
                goto error;
            }

            if (cedar_open(arg_p))
            {
                goto error;
            }

            if (willow_open())
            {
                goto error;
            }

            if (get_feedback_msgQ())
            {
                goto error;
            }

            if (disp_open())
            {
                goto error;
            }

            if (cmdQ_create())
            {
                goto error;
            }

            if (robin_feedbackQ_create())
            {
                goto error;
            }

            if (robin_monitor_create(ROBIN_MONITOR_PRIO))
            {
                goto error;
            }

            if (robin_cedar_mutex_create())
            {
                goto error;
            }

            if (robin_cmd_mutex_create())
            {
                goto error;
            }

            break;

        case ROBIN_MODE_VIDEO_MIN :
        case ROBIN_MODE_VIDEO_MAX :
            __wrn("===============ROBIN_MODE_VIDEO_MIN=========");

            if (orchid_open())
            {
                goto error;
            }

            if (npl_create(RAT_MEDIA_TYPE_VIDEO))
            {
                goto error;
            }

            if (cedar_open(arg_p))
            {
                goto error;
            }

            if (get_feedback_msgQ())
            {
                goto error;
            }

            if (disp_open())
            {
                goto error;
            }

            if (cmdQ_create())
            {
                goto error;
            }

            if (robin_feedbackQ_create())
            {
                goto error;
            }

            if (robin_monitor_create(ROBIN_MONITOR_PRIO))
            {
                goto error;
            }

            if (robin_cedar_mutex_create())
            {
                goto error;
            }

            if (robin_cmd_mutex_create())
            {
                goto error;
            }

            break;

        default :
            goto error;
    }

    __wrn("robin open ok ");
    cur_robin_mode = robin_mode;
    to_quit_robin  = EPDK_FALSE;
    robin_open_state  = EPDK_TRUE;
    return 0;
error:
    __wrn("robin open error ...............");
    _release_res();
    cur_robin_mode = ROBIN_MODE_UNKNOWN;
    to_quit_robin  = EPDK_FALSE;
    return -1;
}



/***************************************************************************************************
*Name        : robin_close
*Prototype   : void  robin_close( void )
*Arguments   : void
*Return      : void
*Description : close robin.
*Other       :
***************************************************************************************************/
void  robin_close(void)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return ;
    }

    to_quit_robin  = EPDK_TRUE;
    robin_open_state  = EPDK_FALSE;
    _release_res();
    cur_robin_mode = ROBIN_MODE_UNKNOWN;
}



/***************************************************************************************************
*Name        : robin_set_mode
*Prototype   : __s32  robin_set_mode( robin_mode_e robin_mode )
*Arguments   : robin_mode    input. the work mode of the robin module.
*Return      : ==0    succeed
*              !=0    fail
*Description : set the work mode of the robin module.
*Other       :
***************************************************************************************************/
__s32  robin_set_mode(robin_mode_e robin_mode, robin_open_arg_t *arg_p)
{
    if (arg_p)
    {
        if (0 == arg_p->reserve_mem_size)
        {
            arg_p->reserve_mem_size = 512 * 1024;
        }
    }

    switch (robin_mode)
    {
        case ROBIN_MODE_AUDIO_MIN :
            disp_close();
            willow_close();
            npl_delete();

            if (orchid_open())
            {
                goto error;
            }

            if (npl_create(RAT_MEDIA_TYPE_AUDIO))
            {
                goto error;
            }

            if (cedar_open(arg_p))
            {
                goto error;
            }

            if (get_feedback_msgQ())
            {
                goto error;
            }

            if (cmdQ_create())
            {
                goto error;
            }

            if (robin_feedbackQ_create())
            {
                goto error;
            }

            if (robin_monitor_create(ROBIN_MONITOR_PRIO))
            {
                goto error;
            }

            if (robin_cedar_mutex_create())
            {
                goto error;
            }

            if (robin_cmd_mutex_create())
            {
                goto error;
            }

            break;

        case ROBIN_MODE_AUDIO_MAX :
            npl_delete();

            if (orchid_open())
            {
                goto error;
            }

            if (npl_create(RAT_MEDIA_TYPE_AUDIO))
            {
                goto error;
            }

            if (cedar_open(arg_p))
            {
                goto error;
            }

            if (willow_open())
            {
                goto error;
            }

            if (get_feedback_msgQ())
            {
                goto error;
            }

            if (disp_open())
            {
                goto error;
            }

            if (cmdQ_create())
            {
                goto error;
            }

            if (robin_feedbackQ_create())
            {
                goto error;
            }

            if (robin_monitor_create(ROBIN_MONITOR_PRIO))
            {
                goto error;
            }

            if (robin_cedar_mutex_create())
            {
                goto error;
            }

            if (robin_cmd_mutex_create())
            {
                goto error;
            }

            break;

        case ROBIN_MODE_VIDEO_MIN :
        case ROBIN_MODE_VIDEO_MAX :
            willow_close();

            if (orchid_open())
            {
                goto error;
            }

            if (npl_create(RAT_MEDIA_TYPE_VIDEO))
            {
                goto error;
            }

            if (cedar_open(arg_p))
            {
                goto error;
            }

            if (get_feedback_msgQ())
            {
                goto error;
            }

            if (disp_open())
            {
                goto error;
            }

            if (cmdQ_create())
            {
                goto error;
            }

            if (robin_feedbackQ_create())
            {
                goto error;
            }

            if (robin_monitor_create(ROBIN_MONITOR_PRIO))
            {
                goto error;
            }

            if (robin_cedar_mutex_create())
            {
                goto error;
            }

            if (robin_cmd_mutex_create())
            {
                goto error;
            }

            break;

        default :
            goto error;
    }

    cur_robin_mode = robin_mode;
    return 0;
error:
    _release_res();
    cur_robin_mode = ROBIN_MODE_UNKNOWN;
    return -1;
}



/***************************************************************************************************
*Name        : robin_get_mode
*Prototype   : robin_mode_e  robin_get_mode( void )
*Arguments   : void
*Return      : the work mode of the robin module.
*Description : get the work mode of the robin module.
*Other       :
***************************************************************************************************/
robin_mode_e  robin_get_mode(void)
{
    return cur_robin_mode;
}

__bool robin_is_open(void)
{
    return robin_open_state;
}

static void  _release_res(void)
{
    robin_monitor_delete();
    cmdQ_delete();
    robin_feedbackQ_delete();
    disp_close();
    willow_close();
    cedar_close();
    npl_delete();
    orchid_close();
    robin_cmd_mutex_delete();
    robin_cedar_mutex_delete();
}

#endif     //  ifndef __robin_c

/* end of robin.c */
