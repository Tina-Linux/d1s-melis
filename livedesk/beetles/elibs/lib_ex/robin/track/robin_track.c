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
#ifndef  __robin_track_c
#define  __robin_track_c


#include "robin_i.h"
#include <kconfig.h>



/***************************************************************************************************
*Name        : robin_get_track_info
*Prototype   : __s32  robin_get_track_info( __audstream_profile_t  *track_info_p )
*Arguments   : track_info_p   output. a buffer to store track info.
*Return      : ==0     succeed
*              !=0     fail
*Description : get the track information of the current playing file.
*Other       :
***************************************************************************************************/
__s32  robin_get_track_info(__audstream_profile_t  *track_info_p)
{
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (track_info_p == NULL)
    {
        return -1;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_AUDSTREAM_PROFILE, 0, track_info_p) == EPDK_OK)
    {
        g_post_mutex(robin_cedar_mutex);
        return 0;
    }
    else
    {
        g_post_mutex(robin_cedar_mutex);
        return -1;
    }
}



/***************************************************************************************************
*Name        : robin_select_track
*Prototype   : __s32  robin_select_track( __u32 track_index )
*Arguments   : track_index   input. the track index among the track list gotten by robin_get_track_info().
*Return      : ==0     succeed
*              !=0     fail
*Description : select a track among the tack list.
*Other       :
***************************************************************************************************/
__s32  robin_select_track(__u32 track_index)
{
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SEL_AUDSTREAM, track_index, NULL) == EPDK_OK)
    {
        g_post_mutex(robin_cedar_mutex);
        return 0;
    }
    else
    {
        g_post_mutex(robin_cedar_mutex);
        return -1;
    }
}



/***************************************************************************************************
*Name        : robin_get_track_index
*Prototype   : __s32  robin_get_track_index( void )
*Arguments   : void
*Return      : the current track index among the track list gotten by robin_get_track_info(). If return -1,
*              that means there is no track or some error occurs.
*Description : get the current track index among the track list gotten by robin_get_track_info().
*Other       :
***************************************************************************************************/
__s32  robin_get_track_index(void)
{
    __s32  ret;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = (__s32)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_AUDSTREAM, 0, NULL);
    g_post_mutex(robin_cedar_mutex);
    return ret;
}



#endif     //  ifndef __robin_track_c
