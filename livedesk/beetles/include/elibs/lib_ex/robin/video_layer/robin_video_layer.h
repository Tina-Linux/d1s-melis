/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_video_layer.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.28
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
* Gary.Wang      2009.09.28       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_video_layer_h
#define  __robin_video_layer_h


/***************************************************************************************************
*Name        : robin_request_video_layer
*Prototype   : __s32  robin_request_video_layer( const RECT *rect_p, __s32 pipe, __s32 prio )
*Arguments   : rect_p    input. the coordinate and size of the video layer.
*              pipe      input. layer pipe.
*              prio      input. layer priority.
*Return      : the video layer handle. if return NULL, means failing.
*Description : request the video layer.
*Other       :
***************************************************************************************************/
extern __hdle  robin_request_video_layer(const RECT *rect_p, __s32 pipe, __s32 prio);

/***************************************************************************************************
*Name        : robin_release_video_layer
*Prototype   : void   robin_release_video_layer( __hdle hlay_video )
*Arguments   : hlay_video     input. the video layer handle, gotten by call robin_request_video_layer().
*Return      : void
*Description : release the video layer.
*Other       :
***************************************************************************************************/
extern void   robin_release_video_layer(__hdle hlay_video);


#endif     //  ifndef __robin_video_layer_h

/* end of robin_video_layer.h */
