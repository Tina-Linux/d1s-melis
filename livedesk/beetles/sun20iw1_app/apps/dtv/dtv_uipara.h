/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, JOHN, China
*                                           All Rights Reserved
*
* File    : movie_uipara.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef _DVBPLAY_UIPARA_H_
#define _DVBPLAY_UIPARA_H_

#include "apps.h"

//#include "dtv_ui_i.h"

typedef struct MOVIE_UIPARA_LAYER
{
    __pixel_rgbfmt_t    fb_fmt;                         //图层格式
    SIZE    fb_size;                        //framebuffer大小
    RECT    src_rect;                       //srcwin区域
    RECT    scn_rect;                       //scnwin区域
    __u32  reserved[32];                    //预留
} __movie_uipara_layer_t;

typedef struct MOVIE_UIPARA
{
    __movie_uipara_layer_t  spsc_lyrwin;   //lyrwin
    __movie_uipara_layer_t  sub_lyrwin;     //sub layer win

    RECT                    preview;    //预览大小
} __movie_uipara_t;


__movie_uipara_t *movie_get_uipara(void);

#endif //_MOVIE_UIPARA_H_
