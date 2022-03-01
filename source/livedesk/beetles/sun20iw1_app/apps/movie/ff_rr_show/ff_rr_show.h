/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : ff_rr_show.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-06  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_FF_RR_SHOW_H__
#define  __MOVIE_FF_RR_SHOW_H__

#include "beetles_app.h"
#include "ff_rr_show_ui.h"

typedef enum
{
    FF_RR_SHOW_SPEED_1  = 0x8,      //X8
    FF_RR_SHOW_SPEED_2  = 0x10,     //X16
    FF_RR_SHOW_SPEED_3  = 0x20,     //X32
    FF_RR_SHOW_SPEED_4  = 0x40      //X64
} MOVIE_FF_RR_SHOW_SPEED;

typedef enum
{
    FF_RR_SHOW_DIRECTION_LEFT,
    FF_RR_SHOW_DIRECTION_RIGHT
} MOVIE_FF_RR_SHOW_DIRECTION;

typedef struct
{
    H_WIN hparent;
    __s32 scene_id;
    __s32 sub_state;
} movie_ff_rr_show_create_para_t;

typedef struct
{
    __u32 text_id;
    __s32 direction;
    __u32 ff_rr_speed;
} movie_ff_rr_show_info_t;

void *movie_ff_rr_show_scene_create(movie_ff_rr_show_create_para_t *create_para);
__s32 movie_ff_rr_show_scene_delete(void *handle);
__s32 movie_ff_rr_show_scene_update_string(void *handle, movie_ff_rr_show_info_t *show_info);


#endif

