/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : spsc_ctrl.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-06 Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __MOVIE_SPSC_CTRL_H__
#define  __MOVIE_SPSC_CTRL_H__

#include "beetles_app.h"

//定义spsc ctrl子场景的图标类型
typedef enum
{
    //movie_spsc_ctrl_icon_prev=0,
    //movie_spsc_ctrl_icon_next,
    //movie_spsc_ctrl_icon_playpause,
    movie_spsc_ctrl_icon_infor = 0,
    movie_spsc_ctrl_icon_tvout,
    movie_spsc_ctrl_icon_num
} movie_spsc_ctrl_icon_t;

//spsc ctrl scene发往父窗口的消息
typedef enum
{
    spsc_ctrl_scene_msg_prev = 0,
    spsc_ctrl_scene_msg_next,
    spsc_ctrl_scene_msg_rr,
    spsc_ctrl_scene_msg_ff,
    spsc_ctrl_scene_msg_playpause,
    spsc_ctrl_scene_msg_timeset,
    spsc_ctrl_scene_msg_timeout,
    spsc_ctrl_scene_msg_info,
    spsc_ctrl_scene_msg_tvout,
    spsc_ctrl_scene_msg_switch,
    spsc_ctrl_scene_msg_num
} movie_spsc_ctrl_scene_msg_t;

//定义spsc ctrl子场景的静态控件类型
typedef enum
{
    movie_spsc_ctrl_static_icon_playmode = 0, //循环模式
    movie_spsc_ctrl_static_icon_channel,      //声道
    movie_spsc_ctrl_static_icon_channel_text,
    movie_spsc_ctrl_static_icon_screen_mode,   //比例
    movie_spsc_ctrl_static_icon_file_info,     //文件信息
    movie_spsc_ctrl_static_icon_video_info,    //视频信息
    movie_spsc_ctrl_static_icon_subtitle,      //字幕开关
    movie_spsc_ctrl_static_icon_subtitle_text,
    movie_spsc_ctrl_static_icon_filename,      //文件名
    movie_spsc_ctrl_static_icon_num
} movie_spsc_ctrl_static_icon_t;

//父窗口发往该窗口的消息
typedef enum
{
    movie_spsc_ctrl_scene_update_fileinfo = GUI_MSG_ + 1,
    movie_spsc_ctrl_scene_draw_difuse

} movie_spsc_ctrl_scene_recv_msg_t;


typedef struct
{
    // 输入参数
    H_WIN hparent;
    __s32 scene_id;
    H_LYR hlyr;
    __s32 sub_state;

    //内部参数

} movie_spsc_ctrl_create_para_t;

void *movie_spsc_ctrl_scene_create(movie_spsc_ctrl_create_para_t *create_para);
__s32 movie_spsc_ctrl_scene_set_focus(void *handle);
__s32 movie_spsc_ctrl_scene_delete(void *handle);
H_WIN movie_spsc_ctrl_scene_get_hwnd(void *handle);

#endif

