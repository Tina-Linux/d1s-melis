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
#ifndef _MINI_MUSIC_UIPARA_H_
#define _MINI_MUSIC_UIPARA_H_

typedef struct MINIPLAY_UI_PARA_ITEM
{
    __u32   x;
    __u32   y;
    __u32   width;
    __u32   height;
    __u32   reserved[32];
} __miniplay_ui_para_item_t;

typedef struct LYRIC_UI_PARA_LAYER
{
    __pixel_rgbfmt_t    fb_fmt;             //图层格式
    SIZE    fb_size;                        //framebuffer大小
    RECT    src_rect;                       //scrwin区域
    RECT    scn_rect;                       //srcwin区域
    __u32   reserved[32];                   //预留
} __miniplay_ui_para_layer_t;

typedef struct LYRIC_PARA
{
    __miniplay_ui_para_layer_t  para_lyrwin;        //lyrwin
    __miniplay_ui_para_item_t   para_frmwin;        //framewin
    __miniplay_ui_para_item_t   para_songname;      //歌曲名控件参数
    __miniplay_ui_para_item_t   para_slide;         //进度条控件参数
    __miniplay_ui_para_item_t   para_pre;           //前一首控件参数
    __miniplay_ui_para_item_t   para_next;          //下一首控件参数
    __miniplay_ui_para_item_t   para_play;          //播放控件参数
    __miniplay_ui_para_item_t   para_time;          //播放时间控件参数
    __miniplay_ui_para_item_t   para_exit;          //停止控件，停止音乐播放
    __miniplay_ui_para_item_t   para_quit;          //推出控件，转到全模式播放界面
    __miniplay_ui_para_item_t   para_bg;            //背景
} __miniplay_uipara_t;

__miniplay_uipara_t *miniplay_get_uipara(void);

#endif //_MINIPLAY_UIPARA_H_
