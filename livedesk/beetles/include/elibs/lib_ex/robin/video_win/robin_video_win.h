/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_video_win.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.27
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
* Gary.Wang      2009.09.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_video_win_h
#define  __robin_video_win_h


/***************************************************************************************************
*Name        : set_video_window
*Prototype   : __s32  set_video_window( __s32 x, __s32 y, __s32 width, __s32 height )
*Arguments   : x        input.
*              y        input.
*              width    input.
*              height   input.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the rectangle of the video window.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_video_win(__s32 x, __s32 y, __s32 width, __s32 height);

/***************************************************************************************************
*Name        : robin_get_video_win
*Prototype   : __s32 robin_get_video_win( RECT *rect_p )
*Arguments   : rect_p         output. store the rectangle information of the video window.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the rectangle information of the video window.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_video_win(RECT *rect_p);



#endif     //  ifndef __robin_video_win_h

/* end of robin_video_win.h */
