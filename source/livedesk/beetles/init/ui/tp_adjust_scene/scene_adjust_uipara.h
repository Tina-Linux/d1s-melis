/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              explorer app sample
*
*                               (c) Copyright 2006-2007, terry, China
*                                           All Rights Reserved
*
* File    : scene_adjust_uipara.h
* By      : terry.zeng
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __SCENE_ADJUST_UIPARA_H__
#define __SCENE_ADJUST_UIPARA_H__


typedef struct SCENE_ADJUST_UI_PARA_ITEM
{
    __u32   x;
    __u32   y;
    __u32   w;
    __u32   h;
    __u32   reserved[32];
} __scene_adjust_ui_para_item_t;

typedef struct SCENE_ADJUST_UI_PARA_LAYER
{
    __u8    fmt;
    __s32   x;
    __s32   y;
    __s32   w;
    __s32   h;
    __u32  reserved[32];
} __scene_adjust_ui_para_layer_t;

typedef struct SCENE_ADJUST_PARA
{
    __scene_adjust_ui_para_layer_t  adust_layer;
    __scene_adjust_ui_para_item_t   adjust_frm_win;
    __scene_adjust_ui_para_item_t   coor_point[5];
} scene_adjust_para_t;

scene_adjust_para_t *scene_adjust_get_uipara(void);

#endif

