/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    :  ebook_uipara.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#include "app_loading_uipara.h"


static app_loading_uipara_t app_loading_uipara;
#define LOADING_WIDTH   100
#define LOADING_HEIGHT  100

app_loading_uipara_t *get_app_loading_uipara(void)
{
    __u32 width = 0, height = 0;
    dsk_display_get_size(&width, &height);
    app_loading_uipara.loading_bmp_pos.x = (width - LOADING_WIDTH) / 2;
    app_loading_uipara.loading_bmp_pos.y = (height - LOADING_HEIGHT) / 2 - 50;
    app_loading_uipara.loading_bmp_pos.w = LOADING_WIDTH;
    app_loading_uipara.loading_bmp_pos.h = LOADING_HEIGHT;
    app_loading_uipara.loading_txt_pos.x = app_loading_uipara.loading_bmp_pos.x - 50;
    app_loading_uipara.loading_txt_pos.y = app_loading_uipara.loading_bmp_pos.y + app_loading_uipara.loading_bmp_pos.h + 10;
    app_loading_uipara.loading_txt_pos.w = 2 * LOADING_WIDTH;
    app_loading_uipara.loading_txt_pos.h = 30;
    return &app_loading_uipara;
}

