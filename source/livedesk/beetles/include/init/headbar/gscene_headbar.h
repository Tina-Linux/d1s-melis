/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : gscene_headbar.h
* By        : Andy.zhang
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2010-9-25 19:19:34 andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __GSCENE_HEADBAR_H__
#define __GSCENE_HEADBAR_H__

#include <kconfig.h>

typedef enum _HBarState
{
    HBAR_ST_HIDE,
    HBAR_ST_SHOW,
    HBAR_ST_SHOW_WIDTH_VOLUME,//显示的同时显示音量条
} HBarState;

typedef enum tag_HBAR_FORMAT
{
    HBAR_FOARMAT_8BPP,
    HBAR_FOARMAT_32BPP
} __hbar_format_t;

typedef enum
{
    BG_NONE = 0,
    BG_MUSIC,
    BG_AUX,
    BG_PAUSE,

} __bg_audio_t;

/* 命令码 */
#define HBAR_TIME_PAINT         (GUI_MSG_USER_DEF+1)    // 绘制时间
#define HBAR_DATA_PAINT         (GUI_MSG_USER_DEF+2)    // 绘制日期
#define HBAR_VOL_PAINT          (GUI_MSG_USER_DEF+3)    // 绘制电池电量
#define HBAR_BRIGHT_PAINT       (GUI_MSG_USER_DEF+4)    // 绘制亮度
#define HBAR_VOLUME_PAINT       (GUI_MSG_USER_DEF+5)    // 绘制音量
#define HBAR_MUSIC_BG_PAINT     (GUI_MSG_USER_DEF+6)    // 绘制背景音乐
#define HBAR_TITLE_PAINT        (GUI_MSG_USER_DEF+7)    // 绘制标题
#define HBAR_INIT_PAINT         (GUI_MSG_USER_DEF+8)    // 绘制所有
#define HBAR_SIG_STATUS_PAINT   (GUI_MSG_USER_DEF+9)    // 绘制所有

typedef struct
{
    char            *name;      // 场景名称
    __u16           sid;        // 场景id
    H_WIN           parent;     // 父窗口
    H_WIN           layer;      // 图层
    GUI_FONT        *font;      // 字体句柄
    __hbar_format_t format;     //format
} HBarCreatePara;

signed long gscene_hbar_create(H_WIN parent, __hbar_format_t format);
__s32 gscene_hbar_set_state(HBarState state);
__s32 gscene_hbar_get_state(HBarState *p_state);
__s32 gscene_hbar_set_title(char *title, __u32 len);    /* title 为utf8编码格式字符串，len<=32 ,设置hbar title 区域字符串*/



//__s32 gscene_hbar_set_volume(void);
__s32 gscene_hbar_get_screen_rect(RECT *rect);
__s32 gscene_hbar_delete(void);

__s32 gscene_hbar_set_sig_status(__u8 st, __u8 qa, __bool en);
void gscene_hbar_set_music_state(__bg_audio_t state);
__bg_audio_t gscene_hbar_get_music_state(void);

__s32 gscene_hbar_update_text(__s32 id);
__s32 gscene_hbar_set_screen_rect(RECT *rect);

#endif /* __GSCENE_HEADBAR_H__ */

