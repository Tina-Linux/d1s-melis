/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_vision_effect.c
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
#ifndef  __robin_vision_effect_c
#define  __robin_vision_effect_c


#include "robin_i.h"

typedef enum
{
    DISP_VIDEO_NATUAL       = 0,
    DISP_VIDEO_SOFT         = 1,
    DISP_VIDEO_VERYSOFT     = 2,
    DISP_VIDEO_SHARP        = 3,
    DISP_VIDEO_VERYSHARP    = 4
} __disp_video_smooth_t;
/***************************************************************************************************
*Name        : robin_set_vision_effect
*Prototype   : __s32  robin_set_vision_effect( __disp_video_smooth_t  vision_effect )
*Arguments   : vision_effect    input. vision effect mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set vision effect mode, such natual, sharp and so on.
*Other       :
***************************************************************************************************/
__s32  robin_set_vision_effect(__disp_video_smooth_t  vision_effect)
{
    return 0;
}



/***************************************************************************************************
*Name        : robin_get_vision_effect
*Prototype   : __disp_video_smooth_t  robin_get_vision_effect( void )
*Arguments   : void
*Return      : the current vision effect.
*Description : get the current vision effect .
*Other       :
***************************************************************************************************/
__disp_video_smooth_t  robin_get_vision_effect(void)
{
    return (__disp_video_smooth_t) 0;
}



#endif     //  ifndef __robin_vision_effect_c

/* end of robin_vision_effect.c */
