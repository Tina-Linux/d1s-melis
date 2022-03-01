/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : main_menu.h
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __MUSIC_MENU_H_
#define __MUSIC_MENU_H_



#include "beetles_app.h"
#include "music_menu_ui.h"
#define MUSIC_MENU_FRM_ID      (APP_MUSIC_ID+10)
#define MUSIC_MENU_ARTIST_INFO (APP_MUSIC_ID+12)
#define MUSIC_MENU_SWITCH_USB_TF    (APP_MUSIC_ID+13)

#define     MUSIC_AUTO_DELETE_MENU_CNT  (1800)  // 30234384


typedef enum
{
    MUSIC_MAIN_MENU_EQ = 0,
    MUSIC_MAIN_MENU_REPEAT,
    MUSIC_MAIN_MENU_TF_USB,
#ifndef MUSIC_VIEW_CHANGE_20160811
    MUSIC_MAIN_MENU_PRE,
    MUSIC_MAIN_MENU_NEXT,
    MUSIC_MAIN_MENU_PLAY,
#else
    MUSIC_MAIN_MENU_AB,
#endif
    MUSIC_MAIN_MENU_ARTIST_INFO,
    MUSIC_MAIN_MENU_RETURN,

    MUSIC_MAIN_MENU_MAX,
} music_main_menu_e;

#ifdef MUSIC_VIEW_CHANGE_20160811
typedef enum music_menu_AB_mode_s
{
    MUSIC_MENU_AB = 0,
    MUSIC_MENU_A,
    MUSIC_MENU_B,
} music_menu_AB_mode_e;
#endif
typedef enum
{
    MUSIC_SUB_MENU_EQ_NORMAL = 0,
    MUSIC_SUB_MENU_EQ_DDB,
    MUSIC_SUB_MENU_EQ_JAZZ,
    MUSIC_SUB_MENU_EQ_ROCK,
    MUSIC_SUB_MENU_EQ_POP,
    MUSIC_SUB_MENU_EQ_CLASSIC,
    MUSIC_SUB_MENU_EQ_VACAL,
    MUSIC_SUB_MENU_EQ_RETURN,

    MUSIC_SUB_MENU_EQ_MAX,
} music_sub_menu_eq;

typedef enum
{
    MUSIC_SUB_MENU_REPEAT_PLAY_ONCE = 0,
    MUSIC_SUB_MENU_REPEAT_ROTOTE_ONE,
    MUSIC_SUB_MENU_REPEAT_ROTOTE_ALL,
    MUSIC_SUB_MENU_REPEAT_PLAY_ALL,
    MUSIC_SUB_MENU_REPEAT_RANDOM,
    MUSIC_SUB_MENU_REPEAT_RETURN,

    MUSIC_SUB_MENU_REPEAT_MAX,
} music_sub_menu_repeat;

typedef enum music_menu_s
{
    MUSIC_MAIN_MENU = 0,
    MUSIC_SUB_MENU,

    MUSIC_OTHER_MENU,
} music_menu_e;


typedef enum music_menu_status_s
{
    MENU_UNFOCUSED = 0,
    MENU_FOCUSED,
    MENU_PRESSED,
    MENU_UNKNOW,
} music_menu_status_e;

typedef struct menu_audio_file_info_s
{
    char filename[RAT_MAX_FULL_PATH_LEN];
    char artist[ROBIN_ARTIST_SIZE];
    char album[ROBIN_ALBUM_SIZE];
    __s32 filesize;
    __s32 bitrate;
    __epdk_charset_enm_e  artist_charset ;
    __epdk_charset_enm_e  album_charset ;
} menu_audio_file_info_t;

typedef struct tag_music_menu_ui_
{
    __u32               memu_unfcs_id;
    __u32               menu_fcs_id;
    HTHEME              h_menu_unfcs;
    HTHEME              h_menu_fcs;
} music_menu_ui_t;

typedef struct music_menu_ctrl_s
{
    H_WIN                       h_framewin;                     //窗口句柄
    H_LYR                       layer;                     //前景图层
    __s32                       menu_flag;
    __s32                       main_menu_index;
    __s32                       menu_level;

    __s32                       eq_menu_index;
    __s32                       eq_sel_index;

    __s32                       playmode_menu_index;
    __s32                       playmode_sel_index;

    __s32                       root_type;
    //music_menu_ui_t               *ui;
    HTHEME                  h_menu_unfcs;
    HTHEME                  h_menu_fcs;
    HTHEME                  h_menuback;
    HTHEME                  h_sel;
    HTHEME                  h_unsel;
    GUI_FONT                *font;
    __bool  alarmComing;
    menu_audio_file_info_t   play_file_info;                //播放文件信息
} music_menu_ctrl_t;

signed long music_menu_create(H_WIN h_parent, __s32 root_type, __s32 menu_level, __s32 play_mode);
__s32 music_menu_destory(H_WIN mm_win);
#endif
