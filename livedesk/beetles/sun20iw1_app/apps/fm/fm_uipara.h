/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : fm_uipara.h
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/07/25 10:31  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __FM_UIPARA_H__E5B6392A_B497_4020_ADC5_7C344A883270__
#define __FM_UIPARA_H__E5B6392A_B497_4020_ADC5_7C344A883270__

#include "beetles_app.h"
//////////////////////////////////////////////////////////////////////////
typedef struct
{
    TOUCH_UI_RECT rt;
    TOUCH_UI_RECT scale_bg;  // 标尺背景位置
    TOUCH_UI_DPOS scale;     // 标尺 X 范围
    TOUCH_UI_RTSZ btn_thumb; // 按钮大小
    TOUCH_UI_INT  cursor_y;  // 游标的Y值
    TOUCH_UI_INT  txt_y;     // 文本开始y值

    __s32         clr_txt_n; //普通文本颜色

} FM_UI_MAINWND;

//ui参数
typedef struct
{
    RECT rt;
    FM_UI_MAINWND main;
} FM_UI_PARA_T;

//////////////////////////////////////////////////////////////////////////
extern FM_UI_PARA_T *get_fm_uipara(void);

#endif //__FM_UIPARA_H__E5B6392A_B497_4020_ADC5_7C344A883270__
//End of this file

