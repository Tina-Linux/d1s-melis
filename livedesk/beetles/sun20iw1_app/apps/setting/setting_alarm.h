

#ifndef __SETTING_ALARM_H__
#define __SETTING_ALARM_H__


#include "app_setting_i.h"

typedef enum setting_alarm_bmp_e
{
    SETTING_ALARM_BIG_SETTING,
    SETTING_ALARM_BG,
    SETTING_ALARM_ITEM_F,
    SETTING_ALARM_ITEM_UNF,
    SETTING_ALARM_RET_F,
    SETTING_ALARM_RET_UNF,
    SETTING_ALARM_LEFT,
    SETTING_ALARM_RIGHT,
    SETTING_ALARM_LONG_STRING_BG,
    //////////////////////////////////////////////
    NEW_ALARM_FLAG_UF_BMP,
    NEW_ALARM_FLAG_F_BMP,
    NEW_ALARM_ITEM_L_FCS_BMP,
    NEW_ALARM_ITEM_L_UFCS_BMP,
    NEW_ALARM_SCROLL_BG_BMP,
    NEW_ALARM_SCROLL_BLOCK_BMP,

    NEW_ALARM_LEFT_BTN_UF_BMP,
    NEW_ALARM_RIGHT_BTN_UF_BMP,
    NEW_ALARM_LEFT_BTN_F_BMP,
    NEW_ALARM_RIGHT_BTN_F_BMP,
    NEW_ALARM_ITEM_R_FCS_BMP,
    NEW_ALARM_ITEM_R_UFCS_BMP,

    ////////////////////////////////////////////
    SETTING_ALARM_BMP_MAX
} setting_alarm_bmp;

typedef enum setting_alarm_item_e
{
    SET_ALARMSTATE,         //闹钟状态
    SET_ALARMTIME,          //闹钟时间
    SET_ALARM_REPEATTIME,   //重复次数
    SET_ALARM_RING,         //闹铃
    SET_ALARM_VOL,          //闹铃音量
    SET_ALARM_RUNTIME,      //闹铃持续时间
    SET_ALARM_DELAY,        //闹铃贪睡时间

    SET_ALARM_MAX

} setting_alarm_item;

typedef enum alarm_paint_index
{
    ID_PAINTING_ALARM_F1,
    ID_PAINTING_ALARM_F2,
    ID_PAINTING_ALARM_F3,
    ID_PAINTING_ALARM_F4,
} draw_alarm_index_e;


typedef enum setting_alarm_view_e
{
    SETTING_ALARM_SELECT_VIEW,
    SETTING_ALARM_SETTING_VIEW

} setting_alarm_view;

typedef struct tag_setting_alarm_attr
{
    reg_system_para_t  *reg_sys;
    GUI_FONT           *font;
    H_LYR               layer;

    H_LBAR              h_listbar;

    __s32               focus_id;
    __s32               old_focus;
    __s32               alarm_index;
    __s32               first_in_paint_list;


    setting_alarm_view  view;
    __hdle              long_str_handle;        //长字符串滚动句柄

    HTHEME              h_bmp_setting_alarm[SETTING_ALARM_BMP_MAX];

} setting_alarm_attr_t;


H_WIN setting_alarm_win_create(H_WIN h_parent, setting_alarm_attr_t *para);
__s32 setting_alarm_win_delete(H_WIN alarm_win);











#endif
