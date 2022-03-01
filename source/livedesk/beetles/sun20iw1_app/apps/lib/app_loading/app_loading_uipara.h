/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : ebook_uipara.ch
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __APP_LOADING_UIPARA_H__
#define  __APP_LOADING_UIPARA_H__

#include "beetles_app.h"

typedef struct tag_ebook_res
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
} app_loading_res_t;


typedef struct tag_ebook_uipara
{
    app_loading_res_t   loading_bmp_pos;
    app_loading_res_t   loading_txt_pos;
} app_loading_uipara_t;

app_loading_uipara_t *get_app_loading_uipara(void);

#endif

