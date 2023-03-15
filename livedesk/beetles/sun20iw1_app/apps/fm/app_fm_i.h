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

#ifndef __APP_FM_I_H__
#define  __APP_FM_I_H__

#include "beetles_app.h"

#include "app_fm.h"
#include "fm_uipara.h"

#define APP_FM_MOD              "APP FM MOD"


#define FM_MAINWND_NAME         "FM_MAINWND_WND"
#define FM_MAINWND_ID            (APP_FM_ID + 1)

#define ID_FM_MAINWND_NOTIFY_TOUCHUP 0x100
#define ID_FM_MAINWND_CLICKUP       0X101
#define ID_FM_MAINWND_CLICKDOWN       0X102
#define ID_FM_MAINWND_CLICKREPEAT       0X103

#define ID_FM_IS_AUTO_SEARCH_DLG (FM_MAINWND_ID + 1)
#define ID_FM_SAVE_FREQ_DLG      (FM_MAINWND_ID + 2)



typedef enum
{
    CMD_AUTOSRH_FINDCH = GUI_MSG_USER_DEF,
    CMD_AUTOSRH_FINDCHFAIL,
    CMD_AUTOSRH_OVER,
    FM_COMMAND,
    //..CMD_UPDATE,
} __fmplay_msg_t;

enum
{
    SRHMODE_IDLE,
    SRHMODE_AUTO,
    SRHMODE_MANUAL,
};

enum
{
    ID_FM_CMD_SHOW_SSET = 0,
    ID_FM_CMD_HIDE_SSET,
    ID_FM_CMD_CREATE_SSET,
    ID_FM_CMD_DESTROY_SSET,
    ID_FM_CMD_SHOW_ASEARCH_DLG,
};

#define FM_HIDE_THUMB          0
#define FM_SHOW_THUMB_N        1
#define FM_SHOW_THUMB_F        2
#define IsFmShowThumbBtn(_v)   ((_v))
#define IsFmFocusThumbBtn(_v)  ((_v) == FM_SHOW_THUMB_F)


typedef struct  //TODO: 精简某些量
{

    __s32 cur_max_freq; //当前最大频率
    __s32 cur_min_freq; //当前最小频率

    __s32 channel_id;     //indicate current channel number
    __u32 cur_freq;       //indicate current frequence
    __u32 channel_count;  //indicate total channel

    __s32 search_mode;    //indicate search mode : auto or manual

    __s32 is_record;
    //  dsk_radio_band_mode_t band_mode;

    __s32 thumb_show;     // thumb btn,0 -- 不显示, 1 -- 显示为非焦点, 2 -- 显示为焦点
    __s32 audio_mode;
} FM_CTRLDATA_T;

typedef struct tag_fm_wnd_t
{
    //__u32 search_freq;                          //搜索频率
    H_WIN     h_main_wnd;
    H_LYR     h_main_lyr;

    __sset_create_para_t       *sset_create_para;
    H_WIN                       h_sset;

    H_WIN     h_dlg;

    FM_CTRLDATA_T fm_ctrl;

    GUI_FONT *font;
    H_WIN     root_type;
    H_WIN h_frm_alarm_coming ;  //闹钟定时时间到的时候弹出的对话框的窗口句柄
} FM_WND_T;
//////////////////////////////////////////////////////////////////////////
extern H_WIN g_FMManWnd;

#define FM_GetManWnd()                   (g_FMManWnd)
#define FM_GetWndPara(_p, _t, _hwnd)     (_p = (_t *)GUI_WinGetAttr(_hwnd))

#define FM_UpdateUI(_hwnd)               do{ __msg("FM_UpdateUI\n"); GUI_WinUpdate(_hwnd, EPDK_TRUE);} while (0)
#define UI_UpdateFMNow(_hwnd)            SEND_MSG(GUI_MSG_PAINT, _hwnd, _hwnd, 0, 0)

//////////////////////////////////////////////////////////////////////////
extern __s32 notify_to_close_fm(H_WIN hwnd, __s32 id, __s32 para);
//////////////////////////////////////////////////////////////////////////
#endif

