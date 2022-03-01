
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : movie_ctrl_ui.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_SPSC_CTRL_UIPARA_H_
#define  __MOVIE_SPSC_CTRL_UIPARA_H_

#include "beetles_app.h"

#define MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM];
    HTHEME res_hdl[MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM];
    __s32 lang_id;
} movie_spsc_ctrl_rect_t;

typedef struct
{
    __s32 res_id;
    HTHEME res_hdl;
} movie_spsc_ctrl_res2hdl_t;

typedef enum
{
    movie_spsc_ctrl_res_id = 0,
    movie_spsc_ctrl_hdl
} movie_spsc_ctrl_res2hdl_e;

typedef enum
{
    movie_spsc_prog_icon_prog_bg = 0,
    movie_spsc_prog_icon_prog_left,
    movie_spsc_prog_icon_prog_mid,
    movie_spsc_prog_icon_prog_cursor,
    movie_spsc_prog_icon_num
} movie_prog_icon_t;


//定义spsc ctrl子场景的图标坐标
typedef struct
{
    movie_spsc_ctrl_rect_t  uipara_frm;
    __s32                   focus_icon_index;
    movie_spsc_ctrl_rect_t  uipara_bg;
    movie_spsc_ctrl_rect_t  uipara_icon[movie_spsc_ctrl_icon_num];
    movie_spsc_ctrl_rect_t  uipara_static_icon[movie_spsc_ctrl_static_icon_num];
    movie_spsc_ctrl_res2hdl_t             uipara_playmode_res[5];
    movie_spsc_ctrl_res2hdl_t             uipara_screenmode_res[5];
    movie_spsc_ctrl_res2hdl_t             uipara_playpause_res[2][MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM];
    movie_spsc_ctrl_rect_t  cur_time;
    movie_spsc_ctrl_rect_t  uipara_time;
    movie_spsc_ctrl_rect_t uipara_spsc_prog[movie_spsc_prog_icon_num];
} movie_spsc_ctrl_uipara_t;

movie_spsc_ctrl_uipara_t *movie_spsc_ctrl_get_uipara(__s32 rotate);

#endif

