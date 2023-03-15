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
#ifndef __RECORD_UI_H__
#define __RECORD_UI_H__

#include "beetles_app.h"
#include "apps.h"

#define RECORD_ROOT_PATH                "\\RECORD\\"


#define RECORD_NAME_MAX                 40

typedef enum recod_view_touch_icon
{
    RECORD_VIEW_BACK_TOUCH,
    RECORD_VIEW_QUALITY_TOUCH,
    RECORD_VIEW_RECORD_TOUCH,
    RECORD_VIEW_SAVE_TOUCH,
    RECORD_VIEW_MUSIC_LIST_TOUCH,
    //..RECORD_VIEW_FORMAT_TOUCH,

    RECORD_VIEW_POOR_QUALITY_TOUCH,
    RECORD_VIEW_NORMAL_QUALITY_TOUCH,
    RECORD_VIEW_GOOD_QUALITY_TOUCH,

    RECORD_VIEW_WMA_TOUCH,
    RECORD_VIEW_WAV_TOUCH,
    RECORD_VIEW_MP3_TOUCH,

    RECORD_LIST_VIEW_BACK_TOUCH,
    RECORD_LIST_VIEW_PREV_TOUCH,
    RECORD_LIST_VIEW_PLAY_PAUSE_TOUCH,
    RECORD_LIST_VIEW_PLAY_NEXT_TOUCH,
    RECORD_LIST_VIEW_DELE_TOUCH,

    RECORD_LIST_VIEW_PLAYINGBAR_TOUCH,

    RECORD_DISK_VIEW_USB_TOUCH,
    RECORD_DISK_VIEW_SD_TOUCH,

    RECORD_VIEW_TOUCH_ICON_MAX
} recod_view_touch_icon;

enum
{
    //数字图片
    RECORD_0_BMP, RECORD_1_BMP, RECORD_2_BMP, RECORD_3_BMP, RECORD_4_BMP,
    RECORD_5_BMP, RECORD_6_BMP, RECORD_7_BMP, RECORD_8_BMP, RECORD_9_BMP, RECORD_10_BMP,
    //音乐播放进度
    RECORD_66_BMP, RECORD_66_LEFT_BMP, RECORD_66_RIGHT_BMP, RECORD_66_MID_BMP, RECORD_66_FLAG_BMP,
    //录音动画
    RECORD_ANI_BG_BMP, RECORD_ANI_0_BMP, RECORD_ANI_1_BMP, RECORD_ANI_2_BMP, RECORD_ANI_3_BMP, RECORD_ANI_4_BMP,
    RECORD_ANI_5_BMP, RECORD_ANI_6_BMP, RECORD_ANI_7_BMP,
    //底部和顶部bar
    RECORD_BAR_BOTTOM_BMP, RECORD_BAR_TOP_BMP,
    //电池状态
    RECORD_BATTERY_0_BMP, RECORD_BATTERY_1_BMP, RECORD_BATTERY_2_BMP, RECORD_BATTERY_3_BMP, RECORD_BATTERY_4_BMP, RECORD_BATTERY_5_BMP,
    RECORD_BATTERY_ADAPTER_BMP, RECORD_BATTERY_ADD_BMP, RECORD_BATTERY_NO_BMP,
    //音量标记
    RECORD_VOL_OFF_BMP, RECORD_VOL_ON_BMP,
    //删除对话框
    RECORD_DEL_BOX_BMP, RECORD_DEL_SEL0_0_BMP, RECORD_DEL_SEL0_1_BMP,
    //质量对话框
    RECORD_QUALITY_BK_BMP, RECORD_QUALITY_FLAG_BMP, RECORD_QUALITY_BK1_BMP, //..
    //录音格式
    RECORD_FORMAT_BK_BMP,
    //音乐列表卷轴
    RECORD_MUSIC_PROGRESS_BG_BMP, RECORD_MUSIC_PROGRESS_DOWN_BMP,
    RECORD_MUSIC_PROGRESS_FLAG_BMP, RECORD_MUSIC_PROGRESS_UP_BMP, RECORD_MUSIC_PROGRESS_FOCUS_BMP,
    //乐谱
    RECORD_MUSIC_PARTICLE, RECORD_MUSIC_BOUY,
    //录音停止, 录音开始, 录音暂停
    RECORD_MENU_RECORD_STOP, RECORD_MENU_RECORD_START, RECORD_MENU_RECORD_PAUSE,
    //音量调节
    RECORD_VOL_BAR_BG, RECORD_VOL_BAR_BODY, RECORD_VOL_LEFT,
    RECORD_VOL_MID, RECORD_VOL_RIGHT, RECORD_VOL_SPEAKER_RIGHT,
    //录音播放背景
    RECORD_MUSICLIST_BG, RECORD_MUSICPLAY_BG,
    //录音列表背景
    RECORD_LIST_WIN_BG, RECORD_LIST_BG,
    //录音背景
    RECORD_BG, RECORD_BG0,
    //边界
    RECORD_RESOUCE_MAX
};
enum
{
    //质量
    STRING_POOR_QUALITY,
    STRING_NORMAL_QUALITY,
    STRING_GOOD_QUALITY,
    //录音机,播放机
    STRING_RECORDER,
    STRING_RECORD_PLAY,
    //可录音时间
    STRING_RECORD_TIME,
    //暂停,播放中..
    STRING_PAUSE,
    STRING_PLAYING,
    //停止
    STRING_STOP,
    //删除对话框
    STRING_DELETE,
    STRING_MSG4,
    STRING_YES,
    STRING_NO,
    //录音停止, 录音开始, 录音暂停
    STRING_MENU_STOP, STRING_MENU_RECORD, STRING_MENU_PAUSE,
    //无录音文件
    STRING_MSG3,
    //边界
    STRING_RECORD_MAX
};

typedef enum RECORD_MENU
{
#if !RECORD_CHANGE_0815
    MENU_BACK       = 0,
#endif
    MENU_MUSIC_LIST,
    MENU_RECORD,
    MENU_SAVE,
#if !RECORD_CHANGE_0815
    MENU_QUALITY,
#endif
    //..MENU_FORMAT,
    MENU_MAX

} RECORD_MENU;

typedef enum MUSIC_LIST_MENU
{
    MUSIC_MENU_BACK =   0,
    MUSIC_MENU_BACKWARD,
    MUSIC_MENU_PLAY,
    MUSIC_MENU_PAUSE,
    MUSIC_MENU_FORWARD,
    MUSIC_MENU_DEL,
    MUSIC_MENU_MAX
} MUSIC_LIST_MENU;

typedef enum FOCUSE_STATE
{
    FOCUSE_NORMAL,
    FOCUSE_OVER,
    FOCUSE_PRESS,
    FOCUSE_UP,

    FOCUSE_STATE_MAX
} FOCUSE_STATE;

typedef enum QUALITY_STATE
{
    QUALITY_NORMAL,
    QUALITY_GOOD,
    QUALITY_VERY_GOOD,
    QUALITY_STATE_MAX
} QUALITY_STATE;

#define MAX_RECORD_MAPPING_ITEM     RECORD_MAPPING_MAX
#define MAX_RECORD_BMP_ITEM         RECORD_RESOUCE_MAX
#define MAX_RECORD_LANG_ITEM        STRING_RECORD_MAX


typedef struct tag_res_mapping_info
{
    __s32   lang;
    __s32   unfocus;
    __s32   focus;
} res_mapping_info_t, *pres_mapping_info_t;

typedef struct tag_res_mapping_bmp_info
{
    HTHEME  unfocus;
    HTHEME  focus;
} res_mapping_bmp_info_t, *pres_mapping_bmp_info_t;

typedef struct tag_record_ui
{
    res_mapping_bmp_info_t  mapping_bmp[MENU_MAX];
    char                    mapping_lang[MENU_MAX][GUI_NAME_MAX];
    res_mapping_bmp_info_t  music_mapping_bmp[MUSIC_MENU_MAX];
    char                    music_mapping_lang[MUSIC_MENU_MAX][GUI_NAME_MAX];

    char    lang[MAX_RECORD_LANG_ITEM][RECORD_NAME_MAX];
    HTHEME  bmp[MAX_RECORD_BMP_ITEM];
} record_ui_t, *precord_ui_t;

extern GUI_RECT *record_get_touch_rect(__u32 iconRectID);
extern __bool record_is_touch_icon(__u32 iconRectID, __u16 x, __u16 y);

extern __s32 init_record_res(precord_ui_t pui);
extern __s32 uninit_record_res(precord_ui_t pui);

extern void draw_record_mapping(precord_ui_t pui, __s32 mappingIndex, FOCUSE_STATE focus, __s16 mappOffset);
extern void clean_record_mapping(precord_ui_t pui, __s32 mappingIndex);

extern void draw_record_music_mapping(precord_ui_t pui, __s32 mappingIndex, FOCUSE_STATE focus);

extern void *get_record_bmp_data(precord_ui_t pui, __s32 index);

extern void draw_record_bmp(precord_ui_t pui, __s32 index);
extern void draw_record_bmp_ext(precord_ui_t pui, __s32 index, __s16 offsetX, __s16 offsetY);
extern void draw_record_bmp_coor(precord_ui_t pui, __s32 index, __s16 X, __s16 Y);
extern void draw_record_bmp_cut(precord_ui_t pui, __s32 index, __s16 xCoor, __s16 yCoor, __s16 height);
extern void draw_record_lang(precord_ui_t pui, __s32 index, __s32 fnt_color);
extern void draw_record_lang_ext(precord_ui_t pui, __s32 index, __s32 fnt_color, GUI_RECT *rect);
extern void draw_record_str(precord_ui_t pui, char *str, __s32 fnt_color, GUI_RECT *rect);
extern void clean_record_bmp(precord_ui_t pui, __s32 index);
extern void clean_record_lang(precord_ui_t pui, __s32 index);
extern void clean_record_str(precord_ui_t pui, const GUI_RECT *rect);

extern __s32 flush_record_res(precord_ui_t pui);

#endif//__RECORD_UI_H__
