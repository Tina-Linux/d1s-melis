/*
************************************************************************************************************************
*                                            display format in register table
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : reg_display.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.08.26
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
* Gary.Wang      2010.08.26        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __REG_DISPLAY_H__
#define  __REG_DISPLAY_H__
#include <kconfig.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum tag_LION_DISP_TVFORMAT
{
    LION_DISP_TV_PAL,
    LION_DISP_TV_NTSC,
    LION_DISP_TV_PAL_SVIDEO,
    LION_DISP_TV_NTSC_SVIDEO,
    LION_DISP_TV_PAL_CVBS_SVIDEO,
    LION_DISP_TV_NTSC_CVBS_SVIDEO,

    LION_DISP_TV_480I,
    LION_DISP_TV_576I,
    LION_DISP_TV_480P,
    LION_DISP_TV_576P,
    LION_DISP_TV_720P_50HZ,
    LION_DISP_TV_720P_60HZ,
    LION_DISP_TV_1080I_50HZ,
    LION_DISP_TV_1080I_60HZ,
    LION_DISP_TV_1080P_24HZ,
    LION_DISP_TV_1080P_50HZ,
    LION_DISP_TV_1080P_60HZ,

    LION_DISP_HDMI_480I,
    LION_DISP_HDMI_576I,
    LION_DISP_HDMI_480P,
    LION_DISP_HDMI_576P,
    LION_DISP_HDMI_720P_50HZ,
    LION_DISP_HDMI_720P_60HZ,
    LION_DISP_HDMI_1080I_50HZ,
    LION_DISP_HDMI_1080I_60HZ,
    LION_DISP_HDMI_1080P_24HZ,
    LION_DISP_HDMI_1080P_50HZ,
    LION_DISP_HDMI_1080P_60HZ,

    LION_DISP_VGA_H1680_V1050,
    LION_DISP_VGA_H1440_V900,
    LION_DISP_VGA_H1360_V768,
    LION_DISP_VGA_H1280_V1024,
    LION_DISP_VGA_H1024_V768,
    LION_DISP_VGA_H800_V600,
    LION_DISP_VGA_H640_V480,
    LION_DISP_VGA_H1440_V900_RB,
    LION_DISP_VGA_H1680_V1050_RB,
    LION_DISP_VGA_H1920_V1080_RB,
    LION_DISP_VGA_H1920_V1080,

    LION_DISP_LCD,
    LION_DISP_LCD_TV_NTSC,
    LION_DISP_LCD_TV_PAL,
    LION_DISP_UNKNOWN = -1

}
__lion_disp_format_e;

#ifdef __cplusplus
}
#endif

#endif     //  ifndef __REG_DISPLAY_H__

/* end of reg_display.h */
