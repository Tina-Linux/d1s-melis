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
#ifndef __APP_DIALOG_I_H__59622CD7_8FAC_4BD0_85FA_66A07F0FF50F__
#define __APP_DIALOG_I_H__59622CD7_8FAC_4BD0_85FA_66A07F0FF50F__
#include "app_dialog.h"

//ui position
typedef struct tag_APP_DIALOG_UI_POS
{
    __pos_t title;
    __pos_t content;
    __pos_t btn_start;
    __pos_t icon;
} APP_DIALOG_UI_POS_T;

//ui gui size
typedef struct tag_APP_DIALOG_UI_SIZE
{
    __rectsz_t title;
    __rectsz_t content;
    __rectsz_t btn;
    __rectsz_t btn_sp;
    __rectsz_t icon;
} APP_DIALOG_UI_SIZE_T;

//ui操作控制参数
typedef struct tag_APP_DIALOG_UI_CTRL_PAPA
{
    __s32 focus_id;
} APP_DIALOG_UI_CTRL_PAPA_T;

typedef struct tag_APP_DIALOG_UI_COLOUR
{
    GUI_COLOR txt_n;
    GUI_COLOR txt_f;
} APP_DIALOG_UI_COLOUR_T;

//ui data
typedef struct tag_APP_DIALOG_UI
{
    RECT                       rt;   //dialog screen rect
    APP_DIALOG_UI_POS_T        pos;
    APP_DIALOG_UI_SIZE_T       size;
    APP_DIALOG_UI_CTRL_PAPA_T  ctrl;
    APP_DIALOG_UI_COLOUR_T     colour;
} APP_DIALOG_UI_T;

#define ADLG_STR_BTN_MEM_SIZE        (4 * 3)
#define ADLG_MAX_BTN_COUNT           3
//dialog resource
typedef struct tag_APP_DIALOG_RES
{
    load_bmp_res_t bmp_bg;
    load_bmp_res_t bmp_btn_n;
    load_bmp_res_t bmp_btn_f;
    load_bmp_res_t bmp_icon;
    char           str_title[ADLG_STR_TITLE_MEM_SIZE];
    char           str_content[ADLG_STR_CONTENT_MEM_SIZE];
    char           str_btn[ADLG_MAX_BTN_COUNT][ADLG_STR_BTN_MEM_SIZE];
    __s32          btn_result[ADLG_MAX_BTN_COUNT];
    __s16          btn_count;
    __s16          icon_enable;
    __s32          txt_clr;
} APP_DIALOG_RES_T;

//dialog window parameter
typedef struct tag_APP_DIALOG_PARA
{
    H_WIN               parent;
    H_WIN               hwnd;
    H_LYR               hlyr;
    __s32               ID;
    __s32               result;
    __u32               style;
    __u32               timeout;
    APP_DIALOG_UI_T     ui;
    APP_DIALOG_RES_T    res;
} APP_DIALOG_PARA_T;

typedef APP_DIALOG_PARA_T dialog_wnd_t;

#define APP_DIALOG        "APP_DIALOG"

#endif //__APP_DIALOG_I_H__59622CD7_8FAC_4BD0_85FA_66A07F0FF50F__
//End of this file
