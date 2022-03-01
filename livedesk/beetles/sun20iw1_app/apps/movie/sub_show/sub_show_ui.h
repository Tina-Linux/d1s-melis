
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : sub_ui.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_SUB_SHOW_UIPARA_H_
#define  __MOVIE_SUB_SHOW_UIPARA_H_

#include "beetles_app.h"

#define MOVIE_SUB_SHOW_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
} movie_sub_show_rect_t;

typedef enum
{
    movie_sub_show_pos_bottom = 0,
    movie_sub_show_pos_mid,
    movie_sub_show_pos_top,
    movie_sub_show_pos_num
} movie_sub_show_pos_t;

typedef struct
{
    __s32 pipe;
    movie_sub_show_rect_t uipara_lyr[movie_sub_show_pos_num];
    movie_sub_show_rect_t uipara_frm;
} movie_sub_show_uipara_t;

movie_sub_show_uipara_t *movie_sub_show_get_uipara(__s32 rotate);

#endif
