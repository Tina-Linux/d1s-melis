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
