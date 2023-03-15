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

#ifndef EBOOK_NEW_SSET_H
#define EBOOK_NEW_SSET_H

#include "app_ebook_i.h"

/*背景音乐开关切换*/
typedef enum     tag_bg_music_switch_e
{
    BG_MUSIC_OPEN = 0,
    BG_MUSIC_CLOSE,
} __ebook_bg_music_switch_e;

/*字体颜色设置*/
typedef enum      tag_font_color_e
{
    FONT_COLOR_BLACK = 0,
    FONT_COLOR_GREEN,
    FONT_COLOR_BLUE,
    //..FONT_COLOR_BLACK,
    FONT_COLOR_EXIT,  //退出
} __ebook_font_color_t;

/* 自动切换时间 */
typedef enum tag_book_auto_e
{
    AUTO_TIME_0 = 0,    // 停止自动播放
    AUTO_TIME_10,       // 10s自动切换
    AUTO_TIME_20,       // 20s自动切换
    AUTO_TIME_30,       // 30s自动切换
    AUTO_TIME_EXIT, //退出
} __ebook_autotime_e;

/*tts 开关*/
typedef enum  tag_book_tts_e
{
    TTS_ON = 0,
    TTS_OFF,
    TTS_EXIT,
} __ebook_tts_e;

/*电子书签*/
typedef enum      tag_open_bmk_e
{
    LABEL1 = 0,
    LABEL2,
    LABEL3,
    LABEL4,
    LABEL5,
    LABEL6

} __ebook_obmk_t;


typedef enum tag_EBOOK_SSET_MSG
{
    EBOOK_SET_EXIT  = 1,        //退出消息码
    EBOOK_SET_MUSIC,            //背景音乐开关消息码
    EBOOK_SET_FCOLOR,           //字体颜色设置
    EBOOK_SET_AUTO,         //自动播放
    EBOOK_SET_TTS,              //tts 开关选项
    EBOOK_SET_GOTO_PAGE,        //页面跳转
    EBOOK_SET_BRIGHT,
    EBOOK_SET_PREV,
    EBOOK_SET_NEXT,
    EBOOK_SET_ZOOMIN,
    EBOOK_SET_ZOOMOUT,
    EBOOK_SET_OBMK,
    EBOOK_SET_SBMK,
    EBOOK_SET_BMK_OPEN,
    EBOOK_SET_BMK_ADD,
    EBOOK_SET_BCOLOR,
    EBOOK_SET_ROTATE,
    EBOOK_SET_TIME,

} __ebook_sset_msg_t;

#define EBOOK_SSET_NC_ID_UNKNOW    0xFF


__sset_create_para_t *Ebook_SSET_Build_CreatePara(H_WIN h_win);
__s32 ebook_sset_init_focus_item_by_reg(__sset_create_para_t *sset_para);


#endif  /*EBOOK_NEW_SSET_H*/
