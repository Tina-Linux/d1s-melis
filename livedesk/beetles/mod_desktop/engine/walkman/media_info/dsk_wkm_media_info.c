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
#ifndef  __DSK_WKM_MEDIA_INFO_C__
#define  __DSK_WKM_MEDIA_INFO_C__


#include "dsk_walkman_i.h"

__s32  dsk_wkm_get_now_playing_file(char *filename)
{
    if (filename == NULL)
    {
        __msg("filename buffer invalid!\n");
        return EPDK_FAIL;
    }

    eLIBs_strcpy(filename, now_playing_file);
    return EPDK_OK;
}

/*
****************************************************************************************************
*Name        : dsk_wkm_get_tag
*Prototype   : __s32  dsk_wkm_get_tag( __cedar_tag_inf_t  *tag_info_p )
*Arguments   : tag_info_p   output. a buffer to store the instant tag information.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the instant tag information.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_get_tag(__cedar_tag_inf_t  *tag_info_p)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_tag(tag_info_p);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_cur_time
*Prototype   : __s32  dsk_wkm_get_cur_time( void )
*Arguments   : void
*Return      : the current playing time, unit : ms. -1 means error.
*Description : get the current playing time.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_get_cur_time(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_cur_time();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_total_time
*Prototype   : __s32  dsk_wkm_get_total_time( void )
*Arguments   : void
*Return      : the total time of the current playing file, unit : ms. -1 means error.
*Description : get the total time of the current playing file.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_get_total_time(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_total_time();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_audio_encoding
*Prototype   : __cedar_audio_fmt_t  dsk_wkm_get_audio_encoding( void )
*Arguments   : void
*Return      : the audio encoding of the current playing file. The encoding value are defined in
*              mod_cedar.h, see __cedar_audio_fmt_t.
*Description : get the audio encoding of the current playing file.
*Other       :
****************************************************************************************************
*/
__cedar_audio_fmt_t  dsk_wkm_get_audio_encoding(void)
{
    __u8   err;
    __cedar_audio_fmt_t  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return CEDAR_ABS_TYPE_;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_audio_encoding();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_audio_bps
*Prototype   : __u32  dsk_wkm_get_audio_bps( void )
*Arguments   : void
*Return      : the audio instant bit rate of the current playing file.
*Description : get the audio instant bit rate of the current playing file.
*Other       :
****************************************************************************************************
*/
__u32  dsk_wkm_get_audio_bps(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return 0;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_audio_bps();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_audio_sample_rate
*Prototype   : __u32  dsk_wkm_get_audio_sample_rate( void )
*Arguments   : void
*Return      : the sample rate of the current playing file.
*Description : get the sample rate of the current playing file.
*Other       :
****************************************************************************************************
*/
__u32  dsk_wkm_get_audio_sample_rate(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return 0;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_audio_sample_rate();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_audio_info
*Prototype   : __s32  dsk_wkm_get_audio_info( const char *file, dsk_wkm_audio_info_t *info_p )
*Arguments   : file      input.
*              info_p    output. a buffer to store the audio info.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the information of a audio file.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_get_audio_info(const char *file, dsk_wkm_audio_info_t *info_p)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_audio_info(file, info_p);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_album_cover
*Prototype   : __s32  dsk_wkm_get_album_cover( const char *file, void *buf, const SIZE *size_p,
*                                                dsk_wkm_pic_zoom_e zoom )
*Arguments   : file      input.
*              buf       output. to store the album cover matrix. ARGB mode.
*              size_p    input. The width and height info of the buffer
*              zoom      input. zoom mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the album cover matrix of a audio file.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_get_album_cover(const char *file, void *buf, const SIZE *size_p, dsk_wkm_pic_zoom_e zoom)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_album_cover(file, buf, size_p, zoom);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



#endif     //  ifndef __DSK_WKM_MEDIA_INFO_C__

/* end of dsk_wkm_media_info.c */
