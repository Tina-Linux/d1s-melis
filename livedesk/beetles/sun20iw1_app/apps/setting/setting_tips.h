/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : msgbox.h
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __SETTING_TIPS_H_
#define  __SETTING_TIPS_H_

#include "app_setting_i.h"

typedef struct tag_setting_tips_para
{
    GUI_FONT *font;
    H_LYR     layer;
} setting_tip_para_t;

H_WIN setting_tips_win_create(H_WIN h_parent, setting_tip_para_t *para);
H_LYR setting_tips_layer_create(void);
__s32  setting_tips_win_delete(H_WIN tips_win);

#endif
