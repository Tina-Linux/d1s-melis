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
