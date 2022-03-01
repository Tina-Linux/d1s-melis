/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              explorer app sample
*
*                               (c) Copyright 2006-2007, terry, China
*                                           All Rights Reserved
*
* File    : screen_lock.h
* By      : terry.zeng
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __SCREEN_LOCK_H__
#define __SCREEN_LOCK_H__

typedef struct
{
    __s32   fb_w;
    __s32   fb_h;

    __s32   src_x;
    __s32   src_y;
    __s32   src_w;
    __s32   src_h;

    __s32   scn_x;
    __s32   scn_y;
    __s32   scn_w;
    __s32   scn_h;

    __u8    pipe;
} __scnlock_layer_t;

typedef struct
{
    __scnlock_layer_t       screen_lock_layer;
    RECT                    frm_rect;
    __u32                   text;
    RECT                    text_rect;
    __u32                   screen_lock_icon;
    __u32                   screen_unlock_start_icon;
    __u32                   screen_unlock1_icon;
    __u32                   screen_unlock2_icon;
    __u32                   screen_unlock_end_icon;
    RECT                    lock_icon_rect;
} __screen_lock_para_t;


#define ID_SCREEN_LOCK      3036


#define SCREEN_LOCK_EXIT    0x4001

H_WIN create_screen_lock(H_WIN parent_win);

__s32 delete_screen_lock(H_WIN win);


#endif
