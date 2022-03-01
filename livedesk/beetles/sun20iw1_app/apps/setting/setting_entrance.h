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

#ifndef __SETTING_ENTRANCE_H_
#define __SETTING_ENTRANCE_H_

#include "app_setting_i.h"

typedef enum
{
    SETTING_ENTER_GENERAL = 0,
    SETTING_ENTER_POWER_MANAGEMENT,
    SETTING_ENTER_ALARM,
    SETTING_ENTER_HANDLE_ADJUST,

} setting_app_entrance_e;

typedef struct tag_setting_entrance_para_s
{
    GUI_FONT *font;

    H_LYR layer;
    __s32 focus_id;
    __s32 old_focus;

} setting_entrance_para_t;

typedef enum
{
    SETTING_ENTRANCE_CREATE_ALARM = GUI_MSG_USER_DEF + 1,
    SETTING_ENTRANCE_PAINT
} setting_entrance_cmd_t;

extern __s32 CFG_get_game_and_dict_flag(__s32 id);
H_WIN setting_entrance_win_create(H_WIN h_parent);
__s32 setting_entrance_win_delete(H_WIN list_win);
#endif
