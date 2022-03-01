
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : prog_bar_ui.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __INIT_PROG_BAR_UIPARA_H_
#define  __INIT_PROG_BAR_UIPARA_H_

#define INIT_PROG_BAR_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[INIT_PROG_BAR_MAX_ICON_RES_NUM];
    HTHEME res_hdl[INIT_PROG_BAR_MAX_ICON_RES_NUM];
} init_prog_bar_rect_t;

typedef enum
{
    init_prog_bar_icon_prog_bg = 0,
    init_prog_bar_icon_lprog_bar,
    init_prog_bar_icon_mprog_bar,
    init_prog_bar_icon_prog_cursor,
    init_prog_bar_icon_num
} init_prog_bar_icon_t;

typedef struct
{
    init_prog_bar_rect_t uipara_frm;
    init_prog_bar_rect_t uipara_prog_bar_bg;
    init_prog_bar_rect_t uipara_icon[init_prog_bar_icon_num];
    init_prog_bar_rect_t uipara_vol_bg;
    init_prog_bar_rect_t uipara_bright_bg;
} init_prog_bar_uipara_t;

init_prog_bar_uipara_t *init_prog_bar_get_uipara(__s32 rotate);

#endif

