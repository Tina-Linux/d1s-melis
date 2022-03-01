/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : fm_ScaleBar.h
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/08/19 15:27  create this file, implements the fundamental interface;
**************************************************************************************************************
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

