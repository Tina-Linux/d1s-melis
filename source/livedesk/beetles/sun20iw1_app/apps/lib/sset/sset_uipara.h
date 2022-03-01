/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              calendar app sample
*
*                                   (c) Copyright 2006-2009, ANDY, China
*                                            All Rights Reserved
*
* File    : sset_menu.h
* By      :lyn
* Version : V1.00
*********************************************************************************************************
*/
#ifndef __SSET_MENU_LAYOUT_H__
#define __SSET_MENU_LAYOUT_H__

#define APP_MAX_LIST_ITEM_COUNT      32


typedef struct tag_SSET_UIPAEA_LCD
{
    __u32       w;
    __u32       h;
} __sset_uipara_lcd_t;

typedef struct tag_SSET_UIPARA_LAYER
{
    __u32       scnx;
    __u32       scny;
    __u32       scnw;
    __u32       scnh;

    __u32       srcx;
    __u32       srcy;
    __u32       srcw;
    __u32       srch;

    __u32       fbw;
    __u32       fbh;

    __u8        fmt;
    __u8        pipe;

} __sset_uipara_layer_t;

typedef struct tag_SSET_UIPARA_FRAME
{
    __u32       x;
    __u32       y;
    __u32       w;
    __u32       h;

} __sset_uipara_frame_t;

typedef struct tag_SSET_UIPARA_CONTROL
{
    __u32       x;
    __u32       y;
    __u32       w;
    __u32       h;

} __sset_uipara_control_t;

typedef struct tag_SSET_UIPARA_ITEM
{
    __u32       x;
    __u32       y;
    __u32       w;
    __u32       h;

} __sset_uipara_item_t;


typedef struct tag_SSET_UIPARA
{
    /* lcd        */
    __sset_uipara_lcd_t         sset_lcd;

    /* layer        */
    __sset_uipara_layer_t       tbar_lyr;
    __sset_uipara_layer_t       lstm_lyr;

    /* frame        */
    __sset_uipara_frame_t       tbar_frm;
    __sset_uipara_frame_t       lstm_frm;

    /* control        */
    __sset_uipara_control_t     lstm_ctl;
    __sset_uipara_control_t     spin_ctl;
    __sset_uipara_control_t     slid_ctl;
    __sset_uipara_control_t     stch_ctl;

    /* item         */
    __sset_uipara_item_t        menu_tbar_item;
    __sset_uipara_item_t        menu_tbar_bmp;
    __sset_uipara_item_t        menu_tbar_txt;

    __sset_uipara_item_t        menu_lstm_bmp;
    __sset_uipara_item_t        menu_lstm_txt;
    __sset_uipara_item_t        menu_lstm_fcs;
    __sset_uipara_item_t        menu_opt_bmp;

    __sset_uipara_item_t        menu_spin_inc_bmp;
    __sset_uipara_item_t        menu_spin_dec_bmp;
    __sset_uipara_item_t        menu_spin_txt;

    __sset_uipara_item_t        menu_slid_head_bmp;
    __sset_uipara_item_t        menu_slid_tail_bmp;
    __sset_uipara_item_t        menu_slid_curr_bmp;

    __sset_uipara_item_t        menu_stch_bmp;

} __sset_uipara_t;

enum
{
    SCREEN_SIZE_480_272,
    SCREEN_SIZE_720_480,
    SCREEN_SIZE_720_576,
    SCREEN_SIZE_800_480,
    SCREEN_SIZE_1280_720,
    SCREEN_SIZE_1024_768,
    SCREEN_SIZE_1920_1080,
    SCREEN_SIZE_LAST,
};

extern  __s32   set_scn_size;

__sset_uipara_t *SSET_Get_Uipara(void);


#endif  /* __SSET_MENU_LAYOUT_H__ */

