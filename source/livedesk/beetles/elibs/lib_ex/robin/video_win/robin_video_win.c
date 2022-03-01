/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_video_win.c
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
#ifndef  __robin_video_win_c
#define  __robin_video_win_c


#include "robin_i.h"



/***************************************************************************************************
*Name        : robin_set_video_win
*Prototype   : __s32  robin_set_video_win( __s32 x, __s32 y, __s32 width, __s32 height )
*Arguments   : x        input.
*              y        input.
*              width    input.
*              height   input.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the rectangle of the video window.
*Other       :
***************************************************************************************************/
__s32  robin_set_video_win(__s32 x, __s32 y, __s32 width, __s32 height)
{
    RECT  image_win;
    image_win.x      = x;
    image_win.y      = y;
    image_win.width  = width;
    image_win.height = height;

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_VID_WINDOW, 0, &image_win) != EPDK_OK)
    {
        __wrn("Fail in setting video window information to cedar!");
        return -1;
    }

    return 0;
}



/***************************************************************************************************
*Name        : robin_get_video_win
*Prototype   : __s32 robin_get_video_win( RECT *rect_p )
*Arguments   : rect_p         output. store the rectangle information of the video window.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the rectangle information of the video window.
*Other       :
***************************************************************************************************/
__s32 robin_get_video_win(RECT *rect_p)
{
    RECT  *rect_i;

    if (rect_p == NULL)
    {
        return -1;
    }

    rect_i = (RECT *)(unsigned long)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_VID_WINDOW, 0, NULL);

    if (rect_i == NULL)
    {
        return -1;
    }

    g_memcpy(rect_p, rect_i, sizeof(RECT));
    return 0;
}



#endif     //  ifndef __robin_video_win_c

/* end of robin_video_win.c */
