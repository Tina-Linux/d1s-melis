/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : miniplay_uipara.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef _MINI_MUSIC_UIPARA_H_
#define _MINI_MUSIC_UIPARA_H_

typedef struct MINIPLAY_UI_PARA_ITEM
{
    __u32   x;
    __u32   y;
    __u32   width;
    __u32   height;
    __u32   reserved[32];
} __miniplay_ui_para_item_t;

typedef struct LYRIC_UI_PARA_LAYER
{
    __pixel_rgbfmt_t    fb_fmt;             //图层格式
    SIZE    fb_size;                        //framebuffer大小
    RECT    src_rect;                       //scrwin区域
    RECT    scn_rect;                       //srcwin区域
    __u32   reserved[32];                   //预留
} __miniplay_ui_para_layer_t;

typedef struct LYRIC_PARA
{
    __miniplay_ui_para_layer_t  para_lyrwin;        //lyrwin
    __miniplay_ui_para_item_t   para_frmwin;        //framewin
    __miniplay_ui_para_item_t   para_songname;      //歌曲名控件参数
    __miniplay_ui_para_item_t   para_slide;         //进度条控件参数
    __miniplay_ui_para_item_t   para_pre;           //前一首控件参数
    __miniplay_ui_para_item_t   para_next;          //下一首控件参数
    __miniplay_ui_para_item_t   para_play;          //播放控件参数
    __miniplay_ui_para_item_t   para_time;          //播放时间控件参数
    __miniplay_ui_para_item_t   para_exit;          //停止控件，停止音乐播放
    __miniplay_ui_para_item_t   para_quit;          //推出控件，转到全模式播放界面
    __miniplay_ui_para_item_t   para_bg;            //背景
} __miniplay_uipara_t;

__miniplay_uipara_t *miniplay_get_uipara(void);

#endif //_MINIPLAY_UIPARA_H_
