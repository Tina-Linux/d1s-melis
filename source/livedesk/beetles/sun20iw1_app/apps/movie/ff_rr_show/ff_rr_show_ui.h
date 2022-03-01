
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : ff_rr_show_ui.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_FF_RR_SHOW_UIPARA_H_
#define  __MOVIE_FF_RR_SHOW_UIPARA_H_

#include "beetles_app.h"

#define INTERVAL_NUM 8 //ÎªÅ¼Êý

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
} movie_ff_rr_show_rect_t;

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 a;
    __s32 b;
} movie_ff_rr_show_ellipse_t;

typedef struct
{
    movie_ff_rr_show_ellipse_t ellipse;
    __s32 interval[INTERVAL_NUM];
} movie_ff_rr_show_interval_t;

typedef struct
{
    __s32 pipe;
    movie_ff_rr_show_rect_t uipara_lyr;
    movie_ff_rr_show_rect_t uipara_frm;
} movie_ff_rr_show_uipara_t;

movie_ff_rr_show_uipara_t *movie_ff_rr_show_get_uipara(__s32 rotate);
movie_ff_rr_show_interval_t *movie_ff_rr_show_get_interval_uipara(__s32 rotate);

#endif
