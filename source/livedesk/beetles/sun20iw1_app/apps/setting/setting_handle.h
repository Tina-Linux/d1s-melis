/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : setting_general.h
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __SETTING_HANDLE_ADJUST_H_
#define __SETTING_HANDLE_ADJUST_H_

#include "apps.h"
typedef struct tag_setting_handle_para
{
    GUI_FONT *font;

    H_LYR layer;
    __s32 focus_id;
    __s32 old_focus;

    __s32 call_status; // 0 为设置程序调用,1为游戏调用

} setting_handle_para_t;

H_WIN setting_handle_win_create(H_WIN h_parent, setting_handle_para_t *para);
__s32 setting_handle_win_delete(H_WIN list_win);


#endif

