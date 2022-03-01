/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_lyric.c
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
#ifndef  __DSK_WKM_LYRIC_C__
#define  __DSK_WKM_LYRIC_C__


#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : dsk_wkm_lyric_exist
*Prototype   : __bool dsk_wkm_lyric_exist( void )
*Arguments   : void
*Return      : EPDK_TRUE      there are lyrics corresponding to the current playing file.
*              EPDK_FALSE     there is NO lyric corresponding to the current playing file.
*Description : check whether there are lyrics corresponding to the current playing file or not.
*Other       :
****************************************************************************************************
*/
__bool dsk_wkm_lyric_exist(void)
{
    __u8   err;
    __bool ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return EPDK_FALSE;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_lyric_exist();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_lyric_format
*Prototype   : __cedar_lyric_fmt_t  dsk_wkm_get_lyric_format( void )
*Arguments   : void
*Return      : the lyric format of the current playing file. the formats are defined in mod_cedar.h,
*              see __cedar_lyric_fmt_t.
*Description : get the lyric format of the current playing file.
*Other       :
****************************************************************************************************
*/
__cedar_lyric_fmt_t  dsk_wkm_get_lyric_format(void)
{
    __u8   err;
    __cedar_lyric_fmt_t  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return CEDAR_LRC_TYPE_;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_lyric_format();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_lyric_item_count
*Prototype   : __s32  dsk_wkm_get_lyric_item_count( void )
*Arguments   : void
*Return      : the item count of the lyric corresponding to the current playing file. if return -1,
*              that means no lyric or lyric information has NOT been parsed completely.
*Description : get the item count of the lyric corresponding to the current playing file.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_get_lyric_item_count(void)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return EPDK_FALSE;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_lyric_item_count();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_lyric_item_list
*Prototype   : __s32  dsk_wkm_get_lyric_item_list( __cedar_subtitle_item_t  *lyric_item_list, __u32 item_count )
*Arguments   : lyric_item_list    output. an array of lyric item.
*              item_count         input. the item count of the array.
*Return      : the real count of items in the array. if return -1, that means no lyric or lyric
*              information has NOT been parsed completely.
*Description : get lyric item list.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_get_lyric_item_list(__cedar_subtitle_item_t  *lyric_item_list, __u32 item_count)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return EPDK_FALSE;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_lyric_item_list(lyric_item_list, item_count);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



#endif     //  ifndef __DSK_WKM_LYRIC_C__

/* end of dsk_wkm_lyric.c */
