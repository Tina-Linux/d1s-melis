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
#ifndef __MUSIC_LYRICS_H__
#define __MUSIC_LYRICS_H__

#include "beetles_app.h"

#define     C_LYRICS_LINE_MAX       6           //此值大小应根据UI参数设计,

typedef struct music_lyrics_ui_s
{
    H_LYR       layer;                  //要显示的图层
    RECT        area;                   //显示区域
    GUI_FONT    *font;                  //字体
    __u32       view_mode;
    GUI_COLOR   font_color;             //字体颜色
    GUI_COLOR   hight_light_color;      //高亮字体颜色

} music_lyrics_ui_t;

typedef music_lyrics_ui_t  music_lyrics_para_t;


typedef struct lyrics_line_s
{
    GUI_COLOR  font_color;
    GUI_RECT   area;
    char       text[128];               //每行最大显示60个字符
} lyrics_line_t;

//歌词以条目为单位，进行访问
typedef struct music_lyrics_s
{
    music_lyrics_ui_t  UI;                      //UI参数


    __u32 cur_item;                             //当前高亮显示的条目号
    __u32 total_item;                           //歌词总条目数
    __cedar_subtitle_item_t *items;             //歌词条目结构指针

    __cedar_subtitle_item_t *last_item;         //上一次检测到的条目号,用于判断是否得到了新的条目
    __cedar_subtitle_item_t *last_focus_item;   //第二个非亮点的条目
    __cedar_subtitle_item_t *last_normal_item;  //第二个非亮点的条目
    __u32 line_count;                           //根据UI参数计算出最大可以显示多少行
    __u32 hl_count;                             //high light count 交替显示的行数
    lyrics_line_t  line[C_LYRICS_LINE_MAX];     //

    __epdk_charset_enm_e    char_code;  //字体编码
} music_lyrics_t;
typedef  music_lyrics_t *H_LYRIC;


extern H_LYRIC new_music_lyrics(music_lyrics_para_t *para);
extern __s32 delete_music_lyrics(H_LYRIC hLyric);
extern __s32 clear_music_lyrics_area(H_LYRIC hLyric);
extern __s32 update_music_lyrics(H_LYRIC hLyric);
extern __bool music_lyrics_exist(void);
#endif
