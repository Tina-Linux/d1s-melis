
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : prog_ui.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_PROG_UIPARA_H_
#define  __MOVIE_PROG_UIPARA_H_

#include "beetles_app.h"

#define MOVIE_PROG_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[MOVIE_PROG_MAX_ICON_RES_NUM];
    HTHEME res_hdl[MOVIE_PROG_MAX_ICON_RES_NUM];
} movie_prog_rect_t;

typedef enum
{
    movie_prog_icon_prog_bg = 0,
    movie_prog_icon_prog_left,
    movie_prog_icon_prog_mid,
    movie_prog_icon_prog_cursor,
    movie_prog_icon_num
} movie_prog_icon_t;

typedef struct
{
    __s32             pipe;
    movie_prog_rect_t uipara_lyr;
    movie_prog_rect_t uipara_frm;
    movie_prog_rect_t uipara_bg;
    movie_prog_rect_t uipara_icon[movie_prog_icon_num];
    movie_prog_rect_t uipara_prog_cur_time[3];
    movie_prog_rect_t uipara_prog_cur_time_dot[2];
    movie_prog_rect_t uipara_prog_total_time[3];
    movie_prog_rect_t uipara_prog_total_time_dot[2];
} movie_prog_uipara_t;

movie_prog_uipara_t *movie_prog_get_uipara(__s32 rotate);

#endif

