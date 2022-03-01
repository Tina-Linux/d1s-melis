/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : fm_main.h
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/07/22 21:23  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __FM_MAIN_H__82602CA9_1D01_4EAE_AC6B_4E81FF493F92__
#define __FM_MAIN_H__82602CA9_1D01_4EAE_AC6B_4E81FF493F92__

#include "app_fm_i.h"
#include "fm_ScaleBar.h"
//////////////////////////////////////////////////////////////////////////
typedef struct
{
    load_bmp_res_t      bg;
    FM_FREQ_SCALE_RES_T scale;
} FM_MAINWND_RES_T;

typedef struct
{
    __s32 is_show_sset;
} FM_MAINWND_CTRLPARA_T;

typedef struct
{
    FM_MAINWND_RES_T            res;
    FM_MAINWND_CTRLPARA_T       ctrl_para;
    __s32      all_paint_flag;    //画全图标志
    FM_CTRLDATA_T              *fm_ctrl; //指向FM_WND_T的fm_ctrl
} FM_MAINWND_T;
//////////////////////////////////////////////////////////////////////////
H_WIN fm_mainfrw_create(H_WIN h_parent, H_LYR layer, void *para);

#endif //__FM_MAIN_H__82602CA9_1D01_4EAE_AC6B_4E81FF493F92__
//End of this file

