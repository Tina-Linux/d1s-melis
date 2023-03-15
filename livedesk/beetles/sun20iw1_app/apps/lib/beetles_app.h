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
#ifndef __BEETLES_APP_H_
#define __BEETLES_APP_H_

#include <kapi.h>
#include "apps.h"
#include "common/palette.h"
#include "common/misc.h"
#include "common/mem_dev.h"
#include "sset/toolbar/GlideCurve.h"
#include "sset/toolbar/tbar_com.h"
#include "sset/sset_e.h"
#include "sset/sset_uipara.h"

#include "app_dialog/app_dialog.h"
#include "app_loading/app_loading.h"
#include "fifo_db/fifo_db.h"
#include "app_alarm/app_alarm.h"
#include "music/MusicPlayer/music_list.h"

extern GUI_FONT                     *SWFFont;

#define APP_ROOT                    "beetles"
#define APP_HOME                    "app_home"
#define APP_EXPLORER                "app_explorer"
#define APP_DTV                     "app_dtv"
#define APP_MOVIE                   "app_movie"
#define APP_MUSIC                   "app_music"
#define APP_PHOTO                   "app_photo"
#define APP_FM                      "app_fm"
#define APP_EBOOK                   "app_ebook"
#define APP_RECORD                  "app_record"
#define APP_SETTING                 "app_setting"
#define APP_RECORD_VEDIO            "app_record_video"
#define APP_RECORD_VEDIO_ID         0x9111

#define APP_HOME_ID                 0x1001
#define APP_EXPLORER_ID             0x2001
#define APP_MOVIE_ID                0x3001
#define APP_MUSIC_ID                0x4001
#define APP_PHOTO_ID                0x5001
#define APP_FM_ID                   0x6001
#define APP_SETTING_ID              0x7001
#define APP_EBOOK_ID                0x8001
#define APP_DTV_ID                  0x9001
#define APP_RECORD_ID               0x9002
#define APP_LOADING_ID              0x9100
#define APP_ALARM_ID                0x9300
#define APP_ROOT_ID                 0x9400
#define VOL_CHILE_WIN               0x9a00

#define AITALK_STR_GRAMMER_PATH     "f:\\name.xml"
#define FS_DISK_NAME                "f:\\"
#define FS_REQUIRE_FREE_SPACE       (1024*1024*50)
#define HOME_APP_NUM_CHANGE         1   /* 0: 仅仅保留karaoke  music  movie  aux  setting  5个app     1: 保留全部14  个app*/
#define HOME_DICT                   0   /*DICT*/

#define KARAOKE_FUNCTION_EX         1 /*是否有karaoke功能,    0: 没有karaoke    1:有karaoke */
#define EBOOK_FUNCTION_EX           0 /*是否有ebook功能,    0: 没有ebook    1:有ebook */
#define PHOTO_FUNCTION_EX           0 /*是否有photo功能,    0: 没有photo    1:有photo */

#define AudioCloseAll               0x0000
#define AudioOpenMp5                0x0001
#define AudioOpenFm                 0x0002
#define AudioOpenLineIn             0x0003

#define MUSIC_VIEW_CHANGE_20160811
#define RECORD_CHANGE_0815          (1)
#define APP_ADD_RECORD_VEDIO
typedef enum
{
    ENCRYPTION_PARTION_NULL         = 0,
    ENCRYPTION_PARTION_EXIST        ,
    ENCRYPTION_PARTION_UNKOWN       ,
} encryption_partion_e;

typedef enum
{
    //ID_HOME_DTV                     = 0,
    
	ID_HOME_EXPLORER				=0,
    ID_HOME_MOVIE                   ,
    ID_HOME_MUSIC                   ,
    ID_HOME_PICTURE                 ,
    ID_HOME_EBOOK                   ,
    ID_HOME_FM                      ,
    ID_HOME_RECORD                  ,
    ID_HOME_SETTING                 ,
		
    ID_HOME_OTHERS                  ,
    ID_MUSIC_SHORTCUT               ,
    ID_MOVIE_SHORTCUT               ,
    ID_EBOOK_SHORTCUT               ,
    ID_PHOTO_SHORTCUT               ,
    ID_FM_SHORTCUT                  ,
    ID_DTV_SHORTCUT                 ,
    ID_LINEIN_SHORTCUT              ,
    ID_HOME_DTV,
} root_home_id_t;


typedef enum
{
    SWITCH_TO_OTHER_APP             = 0,
    SWITCH_TO_MMENU                 ,
    SWITCH_TO_EXPLORER              ,
    OPEN_BG_MUSIC                   ,
    CLOSE_BG_MUSIC                  ,
    SWITCH_DISK                     ,
    SWITCH_TO_LINE_IN               ,
    CLOSE_AUDIO_APP                 ,
    SWITCH_TO_DICT_PANEL            ,
    SWITCH_TO_DICT_APP              ,
    SWITCH_TO_HANDLE_ADJUST         ,
    ROOT_AITALK_TO_APP              , //chengf  add

    ROOT_SWITCH_TO_KARAOKE          , //chengf add
} app2root_cmd_t;

#define BMP_LINE_ROUND(x)   (((uint32_t)x+3)/4*4)

#define BEETLES_TBL_SIZE(a) (sizeof(a)/sizeof(a[0]))

typedef enum
{
    ID_FM_FMPLAY    = 0,
    ID_FM_FMSRH     ,
} root_fm_id_t;

typedef enum
{
    ID_EXPLORER_ALL             = 0,
    ID_EXPLORER_MOVIE           ,
    ID_EXPLORER_PHOTO           ,
    ID_EXPLORER_MUSIC           ,
    ID_EXPLORER_EBOOK           ,
    ID_EXPLORER_GAME            ,
    ID_EXPLORER_RESTORE         ,
} root_explorer_id_t;

typedef enum
{
    EXPLR_SW_TO_MAIN            = 0,
    EXPLR_SW_TO_MOVIE           ,
    EXPLR_SW_TO_PHOTO           ,
    EXPLR_SW_TO_MUSIC           ,
    EXPLR_SW_TO_EBOOK           ,
    EXPLR_SW_TO_OTHER_DISK      ,
} explorer_sw_to_t;

typedef enum
{
    ID_OTHERS_EXPLORER          = 0,
    ID_OTHERS_EBOOK             ,
    ID_OTHERS_CALENDAR          ,
} root_others_id_t;

typedef enum
{
    ID_SETTING_COMMON           = 0,
    ID_SETTING_POWER            ,
    ID_SETTING_TIME             ,
    ID_SETTING_MULTIMEDIA       ,
} root_setting_id_t;

enum
{
    MENU_PAINT_ALL              ,//draw all
    MENU_PAINT_ITEMSW           ,//item switch
    MENU_PAINT_VALUESW          ,//value change
};

typedef enum
{
    SETTING_SW_TO_MAIN              = 0,
    SETTING_SW_TO_PROMPT_UPGRADE    ,
    SETTING_SW_TO_PROMPT_DEFAULT    ,
    EXPLR_SW_TO_UPGRADE             ,
    SETTING_SW_TO_MULTIMEDIA        ,
} setting_sw_to_t;

typedef enum
{
    FM_SW_TO_RECORD     = 0,
} fm_sw_to_t;

typedef enum
{
    RECORD_SW_TO_FM     = 0,
} record_sw_to_t;


//
//MUSICPLAYER_NORMAL:       正常进入音乐播放器
//MUSICPLAYER_BACKGROUND:   音乐播放器作为后台播放
typedef enum
{
    MUSICPLAYER_NORMAL                  = 0,
    MUSICPLAYER_BACKGROUND              = 1,
    MUSICPLAYER_BACKGROUND_HOME         = 2,

    MUSICPLAYER_MUSIC_PAUSE             ,
    MUSICPLAYER_MUSIC_PLAY              ,
    MUSICPLAYER_MUSIC_MENU_CREATE       ,
    MUSICPLAYER_MUSIC_MENU_DESTROY      ,
    MUSICPLAYER_MUSIC_PRE               ,
    MUSICPLAYER_MUSIC_NEXT              ,
    MUSICPLAYER_MUSIC_MENU_EQ           ,
    MUSICPLAYER_MUSIC_MENU_PLAYMODE     ,
    MUSICPLAYER_MUSIC_MENU_SWITCH_DISK  ,
    MUSICPLAYER_MUSIC_SPCTRM_CREATE     ,
    MUSICPLAYER_MUSIC_ARTIST_INFO
} musicplayer_mode_e;

typedef enum
{
    LINEIN_MODE_NORMAL      = 0,
    LINEIN_MODE_BACKGROUND  ,
    LINEIN_MODE_INVALID
} linein_mode_e;

typedef enum
{
    MOVIE_MODE_NORMAL       = 0,//
    MOVIE_MODE_TV           ,
    MOVIE_MODE_DESKTOP      ,
} __movie_create_mode_e;

typedef enum
{
    MOVIE_MODE_AB           = 0,//
    MOVIE_MODE_A            ,
    MOVIE_MODE_B            ,
} __movie_ab_mode_e;


typedef enum
{
    num0                    = 0, //------1
    num1                    ,   //------2
    num2                    ,   //------3
    num3                    ,   //------4
    num4                    ,   //------5
    num5                    ,   //------6
    num6                    ,   //------7
    num7                    ,   //------8
    num8                    ,   //------9
    num9                    ,   //------10
    numa                    ,   //------11
    numb                    ,   //------12
    numc                    ,   //------13
    numd                    ,   //------14
    nume                    ,   //------15
    numf                    ,   //------16
    numg                    ,   //------17
    numh                    ,   //------18
    numi                    ,   //------19
    numj                    ,   //------20
    numk                    ,   //------21
    numl                    ,   //------22
    numm                    ,   //------23
    numn                    ,   //------24
    numo                    ,   //------25
    nump                    ,   //------26
    numq                    ,   //------27
    numr                    ,   //------28
    nums                    ,   //------29
    numt                    ,   //------30
    numu                    ,   //------31
    numv                    ,   //------31
    numw                    ,   //------33
    numx                    ,   //------34
    numy                    ,   //------35
    numz                    ,   //------36
    numdec                  , //" -" ---37

    //==第五个数码管==
    num_play                ,//" > " --------38
    num_pause               ,//" ||" -------39
    num_usb                 ,//"USB" --------40
    num_sd                  ,//"SD" ----------41
    num_2_dot               ,//" : " --------42
    num_aux                 ,//"AUX" --------43
    num_fm                  ,//" . " " FM" ------44

    num_u_pl                ,//">" "USB"------43
    num_sd_pl               ,//">" "SD"------44
    num_u_pa                ,//"||" "USB"-----45
    num_sd_pa               ,//"||" "SD"-----46

    num_u_pl_dot            ,//">" "USB" ":"---47
    num_sd_pl_dot           ,//">" "SD" ":"---48
    num_u_pa_dot            ,//"||" "USB" ":"--49
    num_sd_pa_dot           ,//"||" "SD" ":"--50

    num_aux_pl              ,//">" "AUX"-----51
    num_aux_pa              ,//"||" "AUX"-----52
    //==================

    numnull,

} lcd_show_num;



typedef struct tag_root_para
{
    GUI_FONT        *font;
    H_WIN           h_parent;
    H_WIN           src_win;
    unsigned long        root_type;
    rat_type_t      explr_root;
    uint32_t        setting_opt;//为0代表通用设置，为1代表电源管理
    uint32_t        record_opt;//为0代表正常进入录音，为1代表fm录音

    int32_t         data;

#if 0
    uint32_t        karaoke_root_type; //karaoke  系统分区    //chengf  add
#endif
    int32_t         explorer_play_music; //从文件管理里面播放音乐标志, 1 表示从文件管理中播放
    char            music_file_name[RAT_MAX_FULL_PATH_LEN];
    __bool          return_to_explorer_file_list ;       // 1 表示回退到浏览器中先前播放的媒体文件列表，0 表示回退到sd 卡/ usb 选择窗口中
    __movie_create_mode_e   tv_mode;//视频模式

    __bool          power_on_tv_out; //开机是否进入TV-OUT  模式, EPDK_FALSE: 开机不进入,拉杆LCD  显示, EPDK_TRUE:开机进入,板卡TV-OUT  显示

    __bool          multimedia_to_setting;
    int32_t             media_type;

} root_para_t;


#define H_THEME                 HTHEME
#define get_menu_text           dsk_langres_get_menu_text
#define get_menu_len            dsk_langres_get_menu_len
#define theme_open              dsk_theme_open
#define theme_hdl2buf           dsk_theme_hdl2buf
#define theme_hdl2size          dsk_theme_hdl2size
#define theme_close             dsk_theme_close

#define theme_open_ex           dsk_theme_open_ex
#define theme_hdl2buf_ex        dsk_theme_hdl2buf_ex
#define theme_hdl2size_ex       dsk_theme_hdl2size_ex
#define theme_close_ex          dsk_theme_close_ex

#define TVOUT_LCD               0
#define TVOUT_NTSC              1
#define TF_ENCRYPTION_EX        0   /*karaoke  数据是否使用加密T  卡形式, 0: 不使用加密T  卡 ,1:使用加密T   卡*/
#define HT_1628_LED_EX          0   /* 0  //是否添加1628  LED 屏*/
#define PLUGOUT_USB_SD_TO_PLAY  1   /*0:直接会主界面  1:播放还存在的TF或USB*/
#define OUTAGE_SAVE_CUR_FILE    0   /*防止断电无法记忆当前曲目*/
#define JX_APP_MUSIC_MOVIE_FM_VOLUME    1   /*捷芯要求music,movie and fm声音最大比line in的小一级*/

extern uint32_t     SetFoucsWindowEx(uint32_t Window);
extern uint32_t     GetFoucsWindowEx(void);
extern uint32_t     SetCurrentDevTypeEx(uint32_t curdev);
extern uint32_t     GetCurrentDevTypeEx(void);
extern uint32_t     SetCurrWinPara(uint32_t Window, uint32_t data);
extern uint32_t     GetCurrWinPara(uint32_t Window);
extern uint32_t     SetSDLastWindowEx(uint32_t index);
extern uint32_t     SetUsbLastWindowEx(uint32_t index);
extern uint32_t     GetSDLastWindowEx(void);
extern uint32_t     GetUsbLastWindowEx(void);
extern uint32_t     GetMusicNum(uint32_t Window);
extern uint32_t     SetMusicNum(uint32_t Window, uint32_t data);
extern uint32_t     GetMovieNum(uint32_t Window);
extern uint32_t     SetMovieNum(uint32_t Window, uint32_t data);
extern int32_t      root_audio_mute(void);
extern int32_t      root_audio_nomute(void);
extern uint32_t     root_check_disk(char disk_name[RAT_MAX_PARTITION][4], uint32_t root_type[2]);

#if HT_1628_LED_EX
extern int32_t      lcd_show_state(uint8_t dat1, uint8_t dat2, uint8_t dat3, uint8_t dat4, uint8_t tip);
extern void         Lcd_Show_Time_State(uint32_t Windos);
#endif
#endif// __BEETLES_APP_H_
