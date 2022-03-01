/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_media_info.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.07
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.10.07        1.0         build the file
*
************************************************************************************************************************
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
