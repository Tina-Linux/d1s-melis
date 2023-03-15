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
