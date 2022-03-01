/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : prog_bar.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-06  create this file
**************************************************************************************************************
*/

#ifndef __INIT_PROG_BAR_H__
#define  __INIT_PROG_BAR_H__

#include "apps.h"
#include "prog_bar_ui.h"

//发往父窗口的消息
typedef enum
{
    init_prog_bar_scene_msg_timeout = 0,
    init_prog_bar_scene_msg_prog_barset,
    init_prog_bar_scene_msg_num
} init_prog_bar_scene_msg_t;

typedef struct
{
    H_LYR hlyr;
    H_WIN hparent;
    __s32 scene_id;
    __s32 min_val;
    __s32 max_val;
    __s32 cur_val;
} init_prog_bar_create_para_t;

void *init_prog_bar_scene_create(init_prog_bar_create_para_t *create_para);
__s32 init_prog_bar_scene_delete(void *handle);
__s32 init_prog_bar_scene_set_focus(void *handle);
__s32 init_prog_bar_scene_get_id(void *handle);
__s32 init_prog_bar_scene_update_data(void *handle, init_prog_bar_create_para_t *create_para);



#endif

