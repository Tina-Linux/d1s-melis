/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : app_settings.c
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __APP_SETTINGS_H__
#define  __APP_SETTINGS_H__
#include "app_setting_i.h"

__s32 _setting_set_hbar_title(__s32 id);


typedef enum
{
    SETTING_GENERAL = 0,
    SETTING_POWER_MANAGE,
    SETTING_ALARM,
    SETTING_TIPS,
} setting_root_type_t;

typedef enum
{
    SETTING_DISPLAY_FOCUS_NONE = 0,
    SETTING_DISPLAY_FOCUS_LEFT,
    SETTING_DISPLAY_FOCUS_RIGHT,
} Setting_focus_t;

typedef struct tag_setting_para
{
    GUI_FONT *setting_font;

    H_LYR layer;
    setting_root_type_t root_type;
    __s32 focus_id;
    __s32 old_focus;

} setting_para_t;

typedef struct tag_volume_para
{
    char    total_space_txt[128]  ; //总磁盘容量
    char       free_space_txt[128] ;        //剩余磁盘容量
} volume_para_t ;

typedef struct tag_setting_ctrl
{
    H_WIN h_frm_general;
    H_WIN h_frm_power;
    H_WIN h_frm_adjust;
    H_WIN h_frm_product_info_win;   // 产品信息提示框
    H_WIN h_frm_dafault_dialog_win ;    // 恢复出厂设置对话框
    H_WIN h_frm_entrance;
    H_WIN h_frm_alarm;
    H_WIN h_frm_alarm_select_song ;
    H_WIN h_frm_alarm_clock ;
    H_WIN h_frm_alarm_coming ;  //闹钟定时时间到的时候弹出的对话框的窗口句柄
    H_WIN lyr_setting;
    H_WIN h_frm_handle;


    setting_root_type_t setting_type;
    GUI_FONT *setting_font;
    volume_para_t       volume_para ;
} setting_ctrl_t;



H_WIN app_setting_create(root_para_t *para);
__s32 app_setting_notify_delete_sub_dlg_scene(H_WIN hmanwin);

#endif

