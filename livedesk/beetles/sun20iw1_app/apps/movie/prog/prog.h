/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : prog.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-06  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_PROG_H__
#define  __MOVIE_PROG_H__

#include "beetles_app.h"
#include "prog_ui.h"

//发往父窗口的消息
typedef enum
{
    movie_prog_scene_msg_timeout = 0,
    movie_prog_scene_msg_num
} movie_prog_scene_msg_t;

//父窗口发往该窗口的消息
typedef enum
{
    movie_prog_scene_msg_null = GUI_MSG_ + 1

} movie_prog_scene_recv_msg_t;

typedef struct
{
    H_WIN hparent;
    __s32 scene_id;
} movie_prog_create_para_t;

void *movie_prog_scene_create(movie_prog_create_para_t *create_para);
__s32 movie_prog_scene_delete(void *handle);
__s32 movie_prog_scene_set_focus(void *handle);
H_WIN movie_prog_scene_get_hwnd(void *handle);
__s32 movie_prog_scene_enable_auto_hide(void *handle, __bool enable_hide);
__s32 movie_prog_scene_update_ui(void *handle);

#endif

