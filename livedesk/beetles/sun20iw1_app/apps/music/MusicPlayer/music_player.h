/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __MUSIC_PLAYER_H__
#define __MUSIC_PLAYER_H__

#include "beetles_app.h"

#include "music_list.h"

#include "spectrum.h"

//#include "VolumeBar.h"

#include "music_lyrics.h"

typedef enum music_spectrum_cmd_s
{
    MUSIC_CLEAR_SPECTRUM = 0X88,
    MUSIC_REBUILD_SPECTRUM,

} music_spectrum_cmd_t;

typedef enum music_btn_status_s
{
    UNFOCUSED = 0,
    FOCUSED = 1,
    PRESSED = 2,
} music_btn_status_e;

typedef enum music_play_status_s
{
    MUSIC_PLAYING = 0,
    MUSIC_PAUSE   = 1,
    MUSIC_STOPED,
} music_play_status_e;
//Play mode

typedef enum music_menu_mode_s
{
    MUSIC_EXPLORER_MODE,
    MUSIC_SET_PLAYER_MODE,
} music_menu_mode_e;

typedef enum music_menu_ID_s
{
    SET_PLAY_MODE,
    SET_EQ_MODE,
    SET_BL_TIME,
    SET_PLAY_PREVIOUS,
    SET_PLAY_NEXT,
    SET_PLAY_PAUSE,
    SET_PLAY_BACKWARD,
    SET_PLAY_FORWARD,
} music_menu_ID_e;

typedef enum music_menu_touch_ID_s
{
    SET_PLAY_PAUSE_TOUCH = 0,
    SET_PLAY_BACKWARD_TOUCH,
    SET_PLAY_FORWARD_TOUCH,
    SET_PLAY_PREVIOUS_TOUCH,
    SET_PLAY_NEXT_TOUCH,
} music_menu_touch_ID_e;

typedef enum music_play_mode_s
{
    MUSIC_PALY_ALL = 0x0,               //播放所有文件，但不重复
    MUSIC_PLAY_ONE,
    MUSIC_PLAY_RANDOM,
    MUSIC_REPEAT_ALL,
    MUSIC_REPEAT_ONE,                   //重复播放一首
} music_play_mode_e;

//EQ mode
typedef enum music_EQ_mode_s
{
    MUSIC_EQ_NORMAL = 0,
    MUSIC_EQ_DBB,
    MUSIC_EQ_POP,
    MUSIC_EQ_ROCK,
    MUSIC_EQ_CLASSIC,
    MUSIC_EQ_JAZZ,
    MUSIC_EQ_VOCAL,
    MUSIC_EQ_DANCE,
    MUSIC_EQ_SOFT,

} music_EQ_mode_e;

typedef enum music_AB_mode_s
{
    MUSIC_AB = 0,
    MUSIC_A,
    MUSIC_B,
} music_AB_mode_e;

//background light last time
typedef enum music_BL_time_s
{
    MUSIC_BL_ON = 0,                    //Never off
    MUSIC_BL_LAST_10S,              //last 10second
    MUSIC_BL_LAST_30S,                  //last 30second
    MUSIC_BL_LAST_60S,                  //last 60second
    MUSIC_BL_LAST_90S,                  //last 90second
} music_BL_time_e;

typedef enum music_LYRICS_s
{
    MUSIC_SPECTRUM = 0,
    MUSIC_LYRICS,
    MUSIC_ALBUM,

    MUSIC_LYRICS_MAX,
} music_LYRICS_e;


typedef enum music_play_btn_s
{
    MUSIC_LIST_MENU_ICON = 0,
#ifndef MUSIC_VIEW_CHANGE_20160811
    MUSIC_MENU_ICON,
    MUSIC_AB_ICON,
    MUSIC_PLAY_BTN_ICON_MAX,
#endif
} music_play_btn_e;

typedef struct audio_file_info_s
{
    char filename[RAT_MAX_FULL_PATH_LEN];
    char artist[ROBIN_ARTIST_SIZE];
    char album[ROBIN_ALBUM_SIZE];
    __s32 filesize;
    __s32 bitrate;
    __epdk_charset_enm_e  artist_charset ;
    __epdk_charset_enm_e  album_charset ;
} audio_file_info_t;


typedef struct music_player_ctrl_s
{
    H_WIN                       h_framewin;                     //窗口句柄
    GUI_FONT                        *font;
    H_LYR                           music_layer;                    //前景图层
    //H_LYR                             background_layer;               //背景图层
    //void                              *core;
    __bool              visiable;                   //是否显示主界面，不显示表示背景播放状态

    __s32               root_type;                  //USB or SDCard

    rat_media_type_t    media_type;                 //搜索文件的类型
    H_LBAR              listbar_handle;
    music_list_ctrl_t   *list_ctrl;                 //文件浏览器
    __s32               menu_index;                 // 菜单和show_mode_menu

    music_menu_mode_e   menu_mode;                  //菜单模式, Explorer mode ,or setting player mode
    music_menu_ID_e     menu_id;                    //menu id in  setting mode ,
    music_menu_touch_ID_e       menu_touch_id;                  //menu id in  setting mode ,
    __cedar_status_t    setting_status;             //FF,正在设置快进
    //music_play_mode_e   play_mode;                    //播放模式
    rat_play_mode_e     play_mode;                  //播放模式
    music_EQ_mode_e     EQ_mode;                    //声效模式
    music_AB_mode_e     AB_mode;
    music_BL_time_e     BL_time;                    //背光持续时间
    //__s32             volume;                     //音量
    //power_level_e     battery_status;             //电池状态

    //__u32                 cur_file_total_time;        //当前播放文件总时间
    __u32               play_index;                 //当前播放文件索引号
    audio_file_info_t   play_file_info;             //播放文件信息

    H_LYRIC             h_lyrics;                   //歌词句柄
    music_LYRICS_e      showMode;                   //0 频谱, 1 歌词
    //spectrum_para_t       spectrum_para;              //频谱控制
    //H_WIN              h_spectrum;                    //spectrum控件句柄

    __u32               AutoExitMenuCnt;            //自动退出定时器
    __u32               CheckFocusItemCnt;          //检测焦点条目是否与当前播放条目一致
    __u32               RefreshPlayTimeCnt;         //刷新播放时间timercount
    __u32               RefreshSystimeCnt;          //刷新系统时间timercount
    __u32               RefreshLyricsTimerCnt;      //刷新歌词时间timercount
    //VolumeBar_t       *hVolumeBar;

    __bool              isTouchPlayingBar;
    __bool              isTouchMusicList;
    __bool              isTouchMusicListMove;
    __u32               curTouchPlayingBarTime;
    //music_rat_ctrl_t      rat;                        //文件搜索数据结构
    //explorer_view_mode_e view_mode;                   //显示模式,List or square
    __s32   explorer_play_music;                        //文件管理播放音乐
    char   *music_file_name;
    __bool  alarmComing;

    //数字点歌===================================
    __s32  ndigit_song_num; //数字点歌序号
    __u8   digit_timmer_id; //数字点歌timmer id号
    H_LYR  hdigit_layer;    //显示当前数字点歌数字的图层
    //=============================================

} music_player_ctrl_t; //NOTE: size <= 1024 bytes

#define     MUSIC_LIST_WIN_ID       (APP_MUSIC_ID+1)

//1代表10ms
#define     MUSIC_SYSTIMER_INTERVAL (100)                           // 1 秒钟来一次中断
#define     MUSIC_REFRESH_SYSTIME_CNT   (60)                            //每分钟调用显示时间一次


#define     MUSIC_TIMER_INTERVAL            (22)
#define     MUSIC_REFRESH_PLAYTIME_CNT      (80 / MUSIC_TIMER_INTERVAL)
#define     MUSIC_REFRESH_LYRICS_CNT        (100 / MUSIC_TIMER_INTERVAL)

#define     MUSIC_SPECTRUM_TIMER_INTERVAL   (15)

#define     MUSIC_CHECK_FOCUS_TIMER_INTERVAL (100*30)           //30秒钟没有操作，则返回到实际播放的条目
#define     MUSIC_CHECK_FOCUS_TIMER_CNT       (20)              //

#define     MUSIC_FF_RR_TIMER_INTERVAL       (300)                  //这个时间可以稍微定义大一些

//2秒自动退出菜单
#define     C_AUTO_EXIT_MENU_TIME   (35)



#define MUSIC_FF_SPEED      (CEDAR_FASTEST_FRSPEED/8)
#define MUSIC_RR_SPEED      (CEDAR_FASTEST_FRSPEED/4)


//#define       C_MUSIC_DEBUG

extern H_WIN music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para);

extern __s32 music_player_restore(H_WIN  music_frame_win, __s32 root_type);

__s32 __music_save_breakpoint_info(__s32 index);
__s32 __music_load_breakpoint_info(__s32 index, __cedar_tag_inf_t *tag_inf);

__s32 music_player_scene_delete(H_WIN  music_frame_win);
extern __s32 music_player_delete_digit_layer(H_WIN       music_frame_win);

#endif

