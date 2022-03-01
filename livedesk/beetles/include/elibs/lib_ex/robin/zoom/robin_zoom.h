/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_zoom.h
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
#ifndef  __robin_zoom_h
#define  __robin_zoom_h
#include <kconfig.h>

typedef enum
{
    ROBIN_ZOOM_FULL_SCREEN_VIDEO_RATIO = 0,    // 全屏，按视频比例
    ROBIN_ZOOM_FULL_SCREEN_SCREEN_RATIO,       // 全屏，按屏幕比例
    ROBIN_ZOOM_ORIGIN_SIZE,                    // 原始尺寸
    ROBIN_ZOOM_4R3,                            // 强制4：3比例，可能会导致变形
    ROBIN_ZOOM_16R9,                           // 强制16：9比例，可能会导致变形
    ROBIN_ZOOM_FIT_VIEW,                       // 适合可见，只应用于TV显示。
    ROBIN_ZOOM_EXIT,
    ROBIN_ZOOM_UNKNOWN = -1
} robin_zoom_e;


/***************************************************************************************************
*Name        : robin_set_zoom
*Prototype   : __s32  robin_set_zoom( robin_zoom_e zoom )
*Arguments   : zoom      input. zoom mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set video zoom mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_zoom(robin_zoom_e zoom);

/***************************************************************************************************
*Name        : robin_get_zoom
*Prototype   : robin_zoom_e  robin_get_zoom( void )
*Arguments   : void
*Return      : the current zoom mode.
*Description : get the current zoom mode.
*Other       :
***************************************************************************************************/
extern robin_zoom_e  robin_get_zoom(void);


#endif     //  ifndef __robin_zoom_h

/* end of robin_zoom.h */
