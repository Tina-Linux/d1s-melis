/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : sub.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-06  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_SUB_SHOW_H__
#define  __MOVIE_SUB_SHOW_H__

#include "beetles_app.h"
#include "sub_show_ui.h"

typedef struct
{
    H_WIN hparent;
    __s32 scene_id;
    __u32 sub_pos;
    __u32 sub_color;
    __u32 sub_size;
} movie_sub_show_create_para_t;

void *movie_sub_show_scene_create(movie_sub_show_create_para_t *create_para);
__s32 movie_sub_show_scene_delete(void *handle);

#endif

