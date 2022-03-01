/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : spsc_ui.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_SPSC_UI_H__
#define  __MOVIE_SPSC_UI_H__

#include "beetles_app.h"

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
} movie_spsc_rect_t;

typedef struct
{
    __s32                pipe;
    movie_spsc_rect_t    spsc_scene_lyr;
} movie_spsc_uipara_t;

movie_spsc_uipara_t *movie_spsc_get_uipara(__s32 rotate);

#endif
