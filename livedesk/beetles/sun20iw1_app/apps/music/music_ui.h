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
#ifndef __MUSIC_UI_H__
#define __MUSIC_UI_H__

#define MUSIC_BOTTOM_TEXT_LEN 40


typedef enum music_list_icon_id_s
{
    //for music explorer
    ID_MUSIC_PALETTE = 0,
    ID_MUSIC_BACKGROUND,
    ID_MUSIC_AUDIO_TYPE,
    ID_MUSIC_FILE_UNFOCUSED_ICON,
    ID_MUSIC_FILE_FOCUSED_ICON,
    ID_MUSIC_FOCUS_ITEM_BORDER,
    ID_MUSIC_UNFOCUS_ITEM_BORDER,
    ID_MUSIC_LONG_STR_SCROLL,

    //Headbar
    ID_MUSIC_PLAY_ALL_STATUS_ICON,
    ID_MUSIC_PLAY_ONE_STATUS_ICON,
    ID_MUSIC_PLAY_RANDOM_STATUS_ICON,
    ID_MUSIC_REPEAT_ALL_STATUS_ICON,
    ID_MUSIC_REPEAT_ONE_STATUS_ICON,

    ID_MUSIC_EQ_NORMAL_STATUS_ICON,
    ID_MUSIC_EQ_DBB_STATUS_ICON,
    ID_MUSIC_EQ_POP_STATUS_CION,
    ID_MUSIC_EQ_ROCK_STATUS_CION,
    ID_MUSIC_EQ_CLASSIC_STATUS_CION,
    ID_MUSIC_EQ_JAZZ_STATUS_CION,
    ID_MUSIC_EQ_VOCAL_STATUS_CION,
    ID_MUSIC_EQ_DANCE_STATUS_CION,
    ID_MUSIC_EQ_SOFT_STATUS_CION,

    ID_MUSIC_BL_ON_CION,
    ID_MUSIC_BL_TIME_10S_CION,
    ID_MUSIC_BL_TIME_30S_CION,
    ID_MUSIC_BL_TIME_60S_CION,
    ID_MUSIC_BL_TIME_90S_CION,

    ID_MUSIC_VOL_STATUS_ICON,

    ID_MUSIC_BATTERY_0_ICON,
    ID_MUSIC_BATTERY_1_ICON,
    ID_MUSIC_BATTERY_2_ICON,
    ID_MUSIC_BATTERY_3_ICON,
    ID_MUSIC_BATTERY_4_ICON,
    ID_MUSIC_BATTERY_5_ICON,

    ID_MUSIC_PLAY_ALL_BTN_UNFOCUSED_ICON,
    ID_MUSIC_PLAY_ALL_BTN_FOCUSED_ICON,
    ID_MUSIC_PLAY_ALL_BTN_PRESSED_ICON,

    ID_MUSIC_PLAY_ONE_BTN_UNFOCUSED_ICON,
    ID_MUSIC_PLAY_ONE_BTN_FOCUSED_ICON,
    ID_MUSIC_PLAY_ONE_BTN_PRESSED_ICON,

    ID_MUSIC_PLAY_RANDOM_BTN_UNFOCUSED_ICON,
    ID_MUSIC_PLAY_RANDOM_BTN_FOCUSED_ICON,
    ID_MUSIC_PLAY_RANDOM_BTN_PRESSED_ICON,

    ID_MUSIC_REPEAT_ALL_BTN_UNFOCUSED_ICON,
    ID_MUSIC_REPEAT_ALL_BTN_FOCUSED_ICON,
    ID_MUSIC_REPEAT_ALL_BTN_PRESSED_ICON,

    ID_MUSIC_REPEAT_ONE_BTN_UNFOCUSED_ICON,
    ID_MUSIC_REPEAT_ONE_BTN_FOCUSED_ICON,
    ID_MUSIC_REPEAT_ONE_BTN_PRESSED_ICON,

    ID_MUSIC_EQ_MODE_BTN_UNFOCUSED_ICON,
    ID_MUSIC_EQ_MODE_BTN_FOCUSED_ICON,
    ID_MUSIC_EQ_MODE_BTN_PRESSED_ICON,

    ID_MUSIC_BL_TIME_BTN_UNFOCUSED_CION,
    ID_MUSIC_BL_TIME_BTN_FOCUSED_CION,
    ID_MUSIC_BL_TIME_BTN_PRESSED_CION,

    ID_MUSIC_USB_DISK_BTN_UNFOCUSED_ICON,
    ID_MUSIC_USB_DISK_BTN_FOCUSED_ICON,
    ID_MUSIC_USB_DISK_BTN_PRESSED_ICON,

    ID_MUSIC_SD_CARD_BTN_UNFOCUSED_ICON,
    ID_MUSIC_SD_CARD_BTN_FOCUSED_ICON,
    ID_MUSIC_SD_CARD_BTN_PRESSED_ICON,

    ID_MUSIC_PLAY_PRE_BTN_UNFOCUSED_ICON,
    ID_MUSIC_PLAY_PRE_BTN_FOCUSED_ICON,
    ID_MUSIC_PLAY_PRE_BTN_PRESSED_ICON,

    ID_MUSIC_PLAY_NEXT_BTN_UNFOCUSED_ICON,
    ID_MUSIC_PLAY_NEXT_BTN_FOCUSED_ICON,
    ID_MUSIC_PLAY_NEXT_BTN_PRESSED_ICON,

    ID_MUSIC_PLAY_PAUSE_BTN_UNFOCUSED_ICON,
    ID_MUSIC_PLAY_PAUSE_BTN_FOCUSED_ICON,
    ID_MUSIC_PLAY_PAUSE_BTN_PRESSED_ICON,

    ID_MUSIC_PLAY_BACKWARD_BTN_UNFOCUSED_ICON,
    ID_MUSIC_PLAY_BACKWARD_BTN_FOCUSED_ICON,
    ID_MUSIC_PLAY_BACKWARD_BTN_PRESSED_ICON,

    ID_MUSIC_PLAY_FORWARD_BTN_UNFOCUSED_ICON,
    ID_MUSIC_PLAY_FORWARD_BTN_FOCUSED_ICON,
    ID_MUSIC_PLAY_FORWARD_BTN_PRESSED_ICON,

    ID_MUSIC_PALYING_STATUS_ICON,
    ID_MUSIC_PAUSE_STATUS_ICON,
    ID_MUSIC_PREVIOUS_STATUS_ICON,
    ID_MUSIC_NEXT_STATUS_ICON,

    ID_MUSIC_PROCESS_BAR_HEAD_ICON,
    ID_MUSIC_PROCESS_BAR_BODY_ICON,
    ID_MUSIC_PROCESS_BAR_TAIL_ICON,
    ID_MUSIC_PROCESS_BAR_POINT_ICON,
    ID_MUSIC_PERFORMER_ICON,
    ID_MUSIC_FILE_SIZE_ICON,
    ID_MUSIC_ALBUM_ICON,
    ID_MUSIC_BIT_RATE_ICON,

    //sptrc
    ID_MUSIC_PARTICLE,
    ID_MUSIC_BOUY,

    //scroll bar for explorer
    ID_MUSIC_LIST_SCROLL_BG,
    ID_MUSIC_LIST_SCROLL_HEAD,
    ID_MUSIC_LIST_SCROLL_BODY,
    ID_MUSIC_LIST_SCROLL_TAIL,

    //bg
    ID_MUSIC_BG,
    ID_MUSIC_INFO_BAR,
    ID_MUSIC_PLAYBAR,
    //flag
    ID_MUSIC_PROCESS_BAR_FLAG,

    //bottom
    ID_MUSIC_BACK0,
    ID_MUSIC_EQ_CHANGE0,
    ID_MUSIC_PLAY_MODE0,
    ID_MUSIC_EQ_LYRICS0,
    ID_MUSIC_AB0,//112350  ,
    ID_MUSIC_BACK1,
    ID_MUSIC_EQ_CHANGE1,
    ID_MUSIC_PLAY_MODE1,
    ID_MUSIC_EQ_LYRICS1,
    ID_MUSIC_AB1,//112350  ,
    ID_MUSIC_BOTTOM,
    //play pause
    ID_MUSIC_PLAY0,
    ID_MUSIC_PLAY1,

    //menu_tn   show_mode_btn
    ID_MUSIC_MENU_UNFOCUSED_ICON,
    ID_MUSIC_MENU_FOCUSED_ICON,

    ID_MUSIC_SHOW_MODE_UNFOCUSED_ICON,
    ID_MUSIC_SHOW_MODE_FOCUSED_ICON,

    ID_MUSIC_DEFAULT_ALBUM_ICON,

    ID_MUSIC_HEADBAR_BG,

    MUSIC_ICON_MAX,
} music_list_icon_id_e;                         //e:代表enum,  s:代表struct

typedef enum music_touch_icon
{
    MUSIC_USB_DISK_TOUCH,
    MUSIC_SD_CARD_TOUCH,
    MUSIC_PLAY_TOUCH,
    MUSIC_BACKWARD_TOUCH,
    MUSIC_FORWARD_TOUCH,
    MUSIC_PREV_TOUCH,
    MUSIC_NEXT_TOUCH,

    MUSIC_BACK_TOUCH,
    MUSIC_EQ_CHANGE_TOUCH,
    MUSIC_PLAY_MODE_TOUCH,
    MUSIC_EQ_LYRICS_TOUCH,
    MUSIC_AB_TOUCH,

    MUSIC_PLAYING_BAR_TOUCH,

    MUSIC_TOUCH_ICON_MAX
} music_touch_icon;

typedef struct music_icon_data_s
{
    __u32 res_id;
    HTHEME res_handle;
    void *res_buffer;
} music_icon_data_t;                                //icon data struct

typedef struct music_layer_ui_s
{
    __pixel_rgbfmt_t fb_fmt;        //图层格式
    SIZE fb_size;                   //framebuffer大小
    RECT src_rect;                  //srcwin区域
    RECT scn_rect;                  //scnwin区域
    __u32 reserved[32];             //预留
} music_layer_ui_t;

typedef struct music_menu_layer_ui_s
{
    __pixel_rgbfmt_t fb_fmt;        //图层格式
    SIZE fb_size;                   //framebuffer大小
    RECT src_rect;                  //srcwin区域
    RECT scn_rect;                  //scnwin区域
    __u32 reserved[32];             //预留
} music_menu_layer_ui_t;

typedef struct music_list_ui_s
{
    RECT listbar_rect;                                  //list display area in list mode
    RECT item_rect;                                         //list item  UI 参数

    RECT focus_icon_rect;                       //select border
    RECT file_icon_rect;              //文件图标

    RECT text_rect;
} music_list_ui_t;

typedef struct music_scrollbar_ui_s
{
    RECT BG_rect;                       //背景区域
    RECT scroll_rect;               //滚动条
    __u32 head_height;
    __u32 body_height;
    __s32 tail_height;

} music_scrollbar_ui_t;



//Headbar状态条ui部分
typedef struct music_headbar_ui_s
{
    RECT BG_rect;                           //背景区域
    RECT play_mode_rect;                    //播放模式按钮
    RECT EQ_mode_rect;                      //EQ模式图标
    RECT BLight_rect;                       //背光关闭时间图标
    RECT volume_icon_rect;                  //音量图标
    RECT volume_text_rect;                  //音量值显示区域
    RECT battery_rect;                      //电池图标
    RECT sys_time_rect;                         //时间显示
} music_headbar_ui_t;


//播放器界面部分
typedef struct music_palyer_ui_s
{
    RECT contrl_BG_rect;        //控制按钮部分显示区域，与频谱部分共用一块显示区域
    RECT btn_hint_rect;         //按键提示符区域
    RECT play_mode_btn_rect;    //播放模式按钮
    RECT EQ_mode_btn_rect;      //EQ模式按钮
    RECT BLight_btn_rect;           //背光时间控制按钮

    RECT play_usb_disk_btn_rect;        //
    RECT play_sd_card_btn_rect;     //
    RECT play_pre_btn_rect;     //上一首按钮
    RECT play_next_btn_rect;    //下一首按钮
    RECT play_pause_btn_rect;   //播放暂停按钮
    RECT backward_btn_rect;     //快退按钮
    RECT forward_btn_rect;      //快进按钮

    RECT play_status_rect;      //播放状态图标(暂停或播放)
    RECT play_time;                     //当前播放时间
    RECT total_time;                    //总时间显示区域

    RECT process_bar_head_rect;     //进度条头部
    RECT process_bar_body_rect;     //进度条body
    RECT process_bar_point_rect;    //当前进度点
    RECT process_bar_tail_rect;     //进度条尾部
    RECT process_bar_flag_rect;     //进度条尾部

    RECT performer_icon_rect;   //演唱者图标
    RECT performer_name_rect; //演唱者名称
    RECT file_size_icon_rect;   //文件大小图标
    RECT file_size_text_rect;           //文件大小
    RECT album_icon_rect;       //专辑名称图标
    RECT album_text_rect;       //专辑名称文本
    RECT bit_rate_icon_rect;    //BitRate图标
    RECT bit_rate_text_rect;    //文本显示

    RECT menu_icon_rect;             //菜单
    RECT menu_text_rect;            //菜单文本
    RECT show_mode_icon_rect;       //显示方式
    RECT show_mode_text_rect;        // 显示方式文本
    RECT ab_icon_rect;       //ab播放
    RECT ab_text_rect;        // ab 播放

    RECT tack_text_rect;            // 显示声道
} music_palyer_ui_t;

//音量控制显示部分
typedef struct music_volume_bar_ui_s
{
    RECT BG_rect;                       //整个音量控制区域
    RECT left_speaker_rect; //左边音量图标
    RECT vol_bar_rect;          //音量进度条图标
    RECT right_speaker_rect;//右边音量图标
} music_volume_bar_ui_t;

typedef struct music_spectrum_ui_s
{
    RECT BG_rect;
} music_spectrum_ui_t;


typedef struct music_bg_ui_s
{
    RECT BG_rect;
    //  RECT InfoBar_rect;
    //  RECT PlayBar_rect;
} music_bg_ui_t;

typedef struct music_bottom_ui_s
{
    RECT    back_rect;
    RECT    EQ_Change_rect;
    RECT    play_mode_rect;
    RECT    EQ_lyrics_rect;
    RECT    AB_rect;
    RECT    bottom_rect;
} music_bottom_ui_t;

typedef struct music_album_ui_s
{
    RECT    albumRect;
    void   *albumBuf;
} music_album_ui_t;

typedef struct music_ui_s
{
    music_layer_ui_t            layer;              //整个播放器图层坐标参数
    music_menu_layer_ui_t       menu_layer;

    music_list_ui_t             list_ctrl;      //浏览器list
    music_scrollbar_ui_t    scrollbar;      //浏览器滚动条

    music_headbar_ui_t      headbar;            //顶部状态条
    music_palyer_ui_t     player;               //播放器区域
    music_volume_bar_ui_t volume_bar;       //音量控制条

    music_spectrum_ui_t     spectrum;

    music_bg_ui_t           bg;

    music_bottom_ui_t       bottom;

    music_album_ui_t        album;

    music_icon_data_t       icon_table[MUSIC_ICON_MAX + 1];
} music_ui_t;

//显示缓冲区
#define     C_LAYER_CACHE_AREA_X        310
#define     C_LAYER_CACHE_AREA_Y        40
#define     C_LAYER_CACHE_AREA_W        180
#define     C_LAYER_CACHE_AREA_H        140

extern __s32   tv_out_status;

extern GUI_RECT *music_get_touch_rect(__u32 iconRectID);
extern __bool music_is_touch_icon(__u32 iconRectID, __u16 x, __u16 y);

extern music_ui_t *music_get_ui_param(void);                    //获得UI参数

//获得Explorer 窗口layer size
extern __s32 music_get_screen_rect(RECT *screen_rect);

extern void *music_get_album_buf(void);
extern void *music_get_icon_res(__u32 index);
extern __s32 music_free_icon_res(void);

//获得音乐浏览器UI参数
extern music_ui_t *music_get_ui_param(void);

//获得音乐浏览器UI参数
extern music_list_ui_t *music_get_list_ui_param(void);

//获得顶部HeadbarUI参数
extern music_headbar_ui_t *music_get_headbar_ui_param(void);

//获得顶部playerUI参数
extern music_palyer_ui_t *music_get_player_ui_param(void);

//获得volume barUI参数
extern music_volume_bar_ui_t *music_get_volume_bar_ui_param(void);

//获得listbar每页可以显示条目数
extern __u32 music_count_listbar_pagenum(void);

void music_modify_icon_res(__s32 rat_root_type);


#endif



