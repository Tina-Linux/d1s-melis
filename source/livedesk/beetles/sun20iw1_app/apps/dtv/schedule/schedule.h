/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : schedules.c
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __APP_SCHEDULES_H__
#define  __APP_SCHEDULES_H__
#include "schedule_i.h"
#include "schedule_list.h"


typedef enum
{
    SCHEDULE_GENERAL = 0,
    SCHEDULE_POWER_MANAGE,
    SCHEDULE_TIPS,
    SCHEDULE_ALARM,
} schedule_root_type_t;
typedef struct tag_schedule_para
{
    GUI_FONT *schedule_font;

    H_LYR layer;
    schedule_root_type_t root_type;
    __s32 focus_id;
    __s32 old_focus;

} schedule_para_t;

/*
typedef struct tag_volume_para
{
    char    total_space_txt[128]  ; //总磁盘容量
    char       free_space_txt[128] ;        //剩余磁盘容量
}volume_para_t ;
*/

typedef struct tag_schedule_ctrl
{
    H_WIN       h_frm_list;//详细信息

    H_WIN       layer;

    __s32       focus_id;//当前选择的条目
    __s32       old_focus;

    __s32       destroy_timerid;
    GUI_FONT    *schedule_font;

} schedule_ctrl_t;



H_WIN dtv_schedule_create(H_WIN h_parent, H_LYR layer);

#endif
