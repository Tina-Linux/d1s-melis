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
#ifndef __FM_SCALEBAR_H__4FC85D32_B239_4661_989A_60198FA9CF1C__
#define __FM_SCALEBAR_H__4FC85D32_B239_4661_989A_60198FA9CF1C__

#include "app_fm_i.h"
#include "fm_sset.h"

#define BTN_NORMAL 0
#define BTN_FOCUS  1
#define MAX_BTN_BMP 2
#define MAX_NUM  10

typedef struct
{
    load_bmp_res_t bmp_btn_thumb[MAX_BTN_BMP];
    load_bmp_res_t bmp_scale;
    load_bmp_res_t bmp_cursor_sel;
    load_bmp_res_t bmp_point;
    load_bmp_res_t bmp_l[MAX_NUM];
    load_bmp_res_t bmp_m[MAX_NUM];
    load_bmp_res_t bmp_b[MAX_NUM];
    load_bmp_res_t bmp_108MHZ;
    load_bmp_res_t bmp_87MHZ;
    load_bmp_res_t bmp_87_5MHZ;     //87.5MHZ
    load_bmp_res_t bmp_ch;
    load_bmp_res_t bmp_MHZ;
    load_bmp_res_t bmp_preset;
    load_bmp_res_t bmp_slash;
    load_bmp_res_t bmp_stereo;
    load_bmp_res_t bmp_mono;
    load_bmp_res_t bmp_update_bg;
    load_bmp_res_t bmp_update0_bg;
} FM_FREQ_SCALE_RES_T;
//TBAR MENU BACKGROUNG




///////////
extern __s32 fm_freq_scale_create_res(FM_FREQ_SCALE_RES_T *res);
extern __s32 fm_freq_scale_destroy_res(FM_FREQ_SCALE_RES_T *res);
extern __s32 draw_freq_scale(H_WIN hwnd, H_LYR layer, FM_UI_MAINWND *ui);
extern __s32 on_freq_scale_touch(H_WIN hwnd, __s16 x, __s16 y, __s32 touch_action);

/************************************************************************/
// 微调fm播放频率
// step -- 频率位移,
// return value -- 1:播放频率改变, 0:播放频率不变
/************************************************************************/
#define FM_FREQ_INCHING_VALUE      100
extern __s32 fm_freq_inching(FM_CTRLDATA_T *fm_ctrl, __s32 step);
__s32 draw_currentfreq_scale(H_WIN hwnd, H_LYR layer, FM_UI_MAINWND *ui);


#endif //__FM_SCALEBAR_H__4FC85D32_B239_4661_989A_60198FA9CF1C__
//End of this file
