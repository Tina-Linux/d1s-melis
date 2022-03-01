/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_track.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.26
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
* Gary.Wang      2009.09.26       1.1.0        build the file
*
************************************************************************************************************************
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
