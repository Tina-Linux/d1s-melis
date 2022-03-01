/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : app_setting_i.h
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_SETTINGS_I_H_
#define  __APP_SETTINGS_I_H_

#include "beetles_app.h"

#define SETTING_FOCUS_COLOR    GUI_BLACK//0xffea961d
#define SETTING_UNFOCUS_COLOR  GUI_BLACK

#define ADD_ALARM_SETTING   1
#define ENTRANCE_MENU_ID                (APP_SETTING_ID +100)
#define GENERAL_MENU_ID                 (APP_SETTING_ID +200)
#define GENERAL_FACTORY_DEFAULT_TIPS_ID (GENERAL_MENU_ID+10)    //恢复出厂设置时使用
#define GENERAL_PRODUCT_INFO_TIPS_ID    (GENERAL_MENU_ID+20)    //产品信息提示框
#define GENERAL_ADJUST_ID               (GENERAL_MENU_ID+30)
#define POWER_MANAGE_MENU_ID            (APP_SETTING_ID +300)
#define ALARM_MENU_ID                   (APP_SETTING_ID +310)
#define ALARM_SELECT_SONG_ID            (APP_SETTING_ID +320)
#define ALARM_CLOCK_ID                  (APP_SETTING_ID +330)
#define ALARM_NULL_DISK_TIPS_WIN_ID     (APP_SETTING_ID +340)   //没有磁盘提示框
#define HANDLE_MENU_ID                  (APP_SETTING_ID +400)
#define HANDLE_ADJUST_DIALOG_ID         (HANDLE_MENU_ID + 10)

#define SET_POWER_ITEM_START_X   800-490 //137
#define SET_POWER_ITEM_START_Y   0
#define SET_POWER_ITEM_H        106//106//49    
#define SET_POWER_ITEM_W        490//343


#define SET_ITEM_START_X     0//800-490 //137
#define SET_ITEM_START_Y     0
#define SET_ITEM_H          106//106//49    
#define SET_ITEM_W          300//490//343

#define ID_SWITCH_ITEM      0
#define ID_OP_UP            1
#define ID_OP_DOWN          2
#define ID_OP_ENTER         3
#define ID_OP_LEFT          4
#define ID_OP_RIGHT         5
#define ID_OP_EXIT          6
#define ID_ENTER_SMENU      7
#define ID_EXIT_SMENU       8
#define ID_SWITCH_TO_GENERAL 9
#define ID_SWITCH_TO_TIPS 10
#define ID_SWITCH_TO_ADJUST     11
#define ID_SWITCH_TO_ENTRANCE   12
#define ID_SWITCH_TO_ALARM_VIEW 13
#define ID_SWITCH_TO_ALARM_SELECT_SONG_WIN 14
#define ID_SWITCH_TO_ALARM_CLOCK_WIN        15
//////////////////////////////////////////////////////
#define ID_SWITCH_POWER_MANER       16
#define ID_SWITCH_GENERAL_SETTING       17
#define ID_SWITCH_HANDLE_ADJUST       18

#define TIPS_PRODUCT_INFO               0
#define TIPS_FACTORY_DEFAULT            1
#define APP_RESTORE_FLAG                1
typedef enum
{
    MSG_OP_UP = GUI_MSG_USER_DEF,
    MSG_OP_DOWN,
    MSG_OP_ENTER,
    MSG_OP_LEFT,
    MSG_OP_RIGHT,
    MSG_OP_EXIT,
    MSG_RESTORE_FACTORY,
    ALARM_MUSIC_PATH_MSG,
    ALARM_CLOCK_MSG
} set_msg_t;

void  main_cmd2parent(H_WIN hwin, __s32 id, unsigned long data1, unsigned long data2);

#endif



