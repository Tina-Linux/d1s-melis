/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/


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
