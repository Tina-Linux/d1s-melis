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
#ifndef  __robin_subtitle_c
#define  __robin_subtitle_c


#include "robin_i.h"
#include <kconfig.h>



/***************************************************************************************************
*Name        : robin_subtitle_exist
*Prototype   : __bool robin_subtitle_exist( void )
*Arguments   : void
*Return      : EPDK_TRUE      there are subtitles corresponding to the current playing file.
*              EPDK_FALSE     there is NO subtitle corresponding to the current playing file.
*Description : check whether there are subtitles corresponding to the current playing file or not.
*Other       :
***************************************************************************************************/
__bool robin_subtitle_exist(void)
{
    __s32  subtitle_format;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FALSE;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    subtitle_format = esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_LBSTYPE, 0, NULL);

    if (subtitle_format == CEDAR_LRC_TYPE_NONE || subtitle_format == CEDAR_LRC_TYPE_NOTSPT)
    {
        g_post_mutex(robin_cedar_mutex);
        return EPDK_FALSE;
    }
    else
    {
        g_post_mutex(robin_cedar_mutex);
        return EPDK_TRUE;
    }
}



/***************************************************************************************************
*Name        : robin_get_subtitle_format
*Prototype   : __cedar_lyric_fmt_t  robin_get_subtitle_format( void )
*Arguments   : void
*Return      : the subtitle format of the current playing file. the formats are defined in mod_cedar.h,
*              see __cedar_lyric_fmt_t.
*Description : get the subtitle format of the current playing file.
*Other       :
***************************************************************************************************/
__cedar_lyric_fmt_t  robin_get_subtitle_format(void)
{
    __cedar_lyric_fmt_t  ret;
    __u8   err;
    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = (__cedar_lyric_fmt_t)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_LBSTYPE, 0, NULL);
    g_post_mutex(robin_cedar_mutex);
    return ret;
}



/***************************************************************************************************
*Name        : robin_get_subtitle_list
*Prototype   : __s32  robin_get_subtitle_list( __subtitle_profile_t  *subtitle_info_p )
*Arguments   : subtitle_info_p   output. a buffer to store subtitle info.
*Return      : ==0     subtttle list information has been parsed completely and return these info.
*              !=0     subtttle list information has NOT been parsed completely, call the function later.
*Description : get the subtitle list information of the current playing file.
*Other       :
***************************************************************************************************/
__s32  robin_get_subtitle_list(__subtitle_profile_t  *subtitle_info_p)
{
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (subtitle_info_p == NULL)
    {
        return -1;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_SUBTITLE_PROFILE, 0, subtitle_info_p) == EPDK_OK)
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
*Name        : robin_select_subtitle
*Prototype   : __s32  robin_select_subtitle( __u32 subtitle_index )
*Arguments   : subtitle_index   input. the subtitle index among the subtitle list gotten by
*                               robin_get_subtitle_list().
*Return      : ==0     succeed
*              !=0     fail
*Description : select a subtitle among the subtitle list.
*Other       :
***************************************************************************************************/
__s32  robin_select_subtitle(__u32 subtitle_index)
{
    __u8   err;
    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SELECT_SUBTITLE, subtitle_index, NULL) == EPDK_OK)
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
*Name        : robin_get_subtitle_index
*Prototype   : __s32  robin_get_subtitle_index( void )
*Arguments   : void
*Return      : the current subtitle index among the subtitle list gotten by robin_get_subtitle_list(). If
*              return -1, that means there is no subtitle or some error occurs.
*Description : get the current subtitle index among the subtitle list gotten by robin_get_subtitle_list().
*Other       :
***************************************************************************************************/
__s32  robin_get_subtitle_index(void)
{
    __s32  ret;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = (__s32)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_SUBTITLE, 0, NULL);
    g_post_mutex(robin_cedar_mutex);
    return ret;
}




/***************************************************************************************************
*Name        : robin_get_subtitle_item
*Prototype   : __s32  robin_get_subtitle_item( __u32 time, __cedar_subtitle_item_t *subtitle_item_p );
*Arguments   : time              input. unit : ms.
*              subtitle_item_p   output. the subtitle item information corresponding to the time.
*Return      : ==0    succeed
*              !=0    there is no subtitle item information corresponding to the time.
*Description : get the subtitle item information corresponding to the time.
*Other       :
***************************************************************************************************/
__s32  robin_get_subtitle_item(__u32 time, __cedar_subtitle_item_t *subtitle_item_p)
{
    __cedar_subtitle_item_t  *p = NULL;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (subtitle_item_p == NULL)
    {
        return -1;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    p = (__cedar_subtitle_item_t *)(unsigned long)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_SUB_INFO, time,
            (void *)CEDAR_GET_SUB_INF_ITEM);

    if (p == NULL)            // no subtitle information at present
    {
        g_post_mutex(robin_cedar_mutex);
        return -1;
    }
    else
    {
        g_memcpy(subtitle_item_p, p, sizeof(__cedar_subtitle_item_t));
        g_post_mutex(robin_cedar_mutex);
        return 0;
    }
}



#endif     //  ifndef __robin_subtitle_c
