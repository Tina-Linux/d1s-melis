/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                             shell application
*
*                         Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : GUI_BLOCK_I.h
* By      : Derek
* Version : V1.0
* Tiem    : 2021-1-12 9:49:02
*********************************************************************************************************
*/
#ifndef __GUI_BLOCK_I_H__
#define __GUI_BLOCK_I_H__

//#include "epdk.h"
//#include "..\\gui_i.h"

//#define  pthread_mutex_t    __krnl_event_t*


typedef struct GUI_BLOCK
{
    __u32                       fb_addr[3];     //framebuffer address
    __u32                       fb_width;       //framebuffer width
    __u32                       fb_height;      //framebuffer height
    __pixel_rgbfmt_t            fb_fmt;         //framebuffer format
    __u8                        fb_byte_seq;    //0:argb; 1:bgra
    RECT                        block_rect;     //block rect
    H_LYR                       block_swlyr;    //block software layer
    __bool                      new_fb;         //True:new create fb; False:fb from other framebuffer
    RECT                        clip_rect;      //clip rect
    pthread_mutex_t             lock;
} __gui_block_t;

#endif
