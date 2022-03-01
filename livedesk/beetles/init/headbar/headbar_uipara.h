/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : headbar_com.h
* By        : Andy.zhang
* Func      : headbar manage
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
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
