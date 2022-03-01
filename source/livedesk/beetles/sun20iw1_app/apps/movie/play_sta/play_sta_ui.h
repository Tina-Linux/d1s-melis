
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : playsta_ui.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_PLAYSTA_UIPARA_H_
#define  __MOVIE_PLAYSTA_UIPARA_H_

#include "beetles_app.h"

#define MOVIE_PLAYSTA_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[MOVIE_PLAYSTA_MAX_ICON_RES_NUM];
    HTHEME res_hdl[MOVIE_PLAYSTA_MAX_ICON_RES_NUM];
} movie_playsta_rect_t;

typedef enum
{
    movie_playsta_icon_lplaysta = 0,
    movie_playsta_icon_num
} movie_playsta_icon_t;

typedef struct
{
    __s32                pipe;
    movie_playsta_rect_t uipara_lyr;
    movie_playsta_rect_t uipara_frm;
    movie_playsta_rect_t uipara_icon[movie_playsta_icon_num];
} movie_playsta_uipara_t;

movie_playsta_uipara_t *movie_playsta_get_uipara(__s32 rotate);

#endif

