/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_track.h
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
#ifndef  __robin_track_h
#define  __robin_track_h


/***************************************************************************************************
*Name        : robin_get_track_info
*Prototype   : __s32  robin_get_track_info( __audstream_profile_t  *track_info_p )
*Arguments   : track_info_p   output. a buffer to store track info.
*Return      : ==0     succeed
*              !=0     fail
*Description : get the track information of the current playing file.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_track_info(__audstream_profile_t  *track_info_p);

/***************************************************************************************************
*Name        : robin_select_track
*Prototype   : __s32  robin_select_track( __u32 track_index )
*Arguments   : track_index   input. the track index among the track list gotten by robin_get_track_info().
*Return      : ==0     succeed
*              !=0     fail
*Description : select a track among the tack list.
*Other       :
***************************************************************************************************/
extern __s32  robin_select_track(__u32 track_index);

/***************************************************************************************************
*Name        : robin_get_track_index
*Prototype   : __s32  robin_get_track_index( void )
*Arguments   : void
*Return      : the current track index among the track list gotten by robin_get_track_info(). If return -1,
*              that means there is no track or some error occurs.
*Description : get the current track index among the track list gotten by robin_get_track_info().
*Other       :
***************************************************************************************************/
extern __s32  robin_get_track_index(void);


#endif     //  ifndef __robin_track_h

/* end of robin_track.h */
