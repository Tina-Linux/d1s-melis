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
#ifndef __HEADBAR_UIPARA_H__
#define __HEADBAR_UIPARA_H__

#include <kconfig.h>

typedef struct
{
    __s32   fb_w;
    __s32   fb_h;

    __s32   src_x;
    __s32   src_y;
    __s32   src_w;
    __s32   src_h;

    __s32   scn_x;
    __s32   scn_y;
    __s32   scn_w;
    __s32   scn_h;

    __u8    pipe;
} __headbar_ui_para_layer_t;


typedef struct
{
    __u32   x;
    __u32   y;
    __u32   w;
    __u32   h;
    __u32   reserved[32];
} __headbar_ui_para_item_t;

typedef struct
{
    __u32 vol_id0;
    __u32 vol_id1;
    __u32 vol_id2;
    __u32 vol_id3;
    __u32 vol_id4;
    __u32 vol_id5;
    __u32 charge_vol_id0;
    __u32 charge_vol_id1;
    __u32 charge_vol_id2;
    __u32 charge_vol_id3;
    __u32 charge_vol_id4;
    __u32 charge_vol_id5;
    __u32 no_vol_id;
    __u32 usb_connect_vol_id;
    __u32 music_id;
    __u32 aux_id;
    __u32 pause_id;
    __u32 bright_id;
    __u32 volume_id;
    __u32 home_id;
    __u32 home_fcs_id;
    __u32 bg_id;
    __u32 sig_st_id[6];
    __u32 sig_qa_id;
} __headbar_ui_para_res_t;

typedef struct
{
    __headbar_ui_para_layer_t   hbar_lyr;
    /* item         */
    __headbar_ui_para_item_t    hbar_frmwin;

    __headbar_ui_para_item_t    music;
    __headbar_ui_para_item_t    date;
    __headbar_ui_para_item_t    time;
    __headbar_ui_para_item_t    volume_icon;
    __headbar_ui_para_item_t    volume_txt;
    __headbar_ui_para_item_t    bright_icon;
    __headbar_ui_para_item_t    bright_txt;
    __headbar_ui_para_item_t    battery;
    __headbar_ui_para_item_t    home;
    __headbar_ui_para_item_t    title;

    __headbar_ui_para_item_t    singal_icon;//信号强度
    __headbar_ui_para_item_t    singal_qa;  //信号质量

    __headbar_ui_para_res_t     res_id;

    GUI_FONT                    *font;
} __headbar_uipara_t;

extern GUI_FONT    *FONT16;
extern __headbar_uipara_t  headbar_uipara;

void headbar_init_uipara(__hbar_format_t format);

#endif /* __MAIN_STATUS_BAR_H__ */
