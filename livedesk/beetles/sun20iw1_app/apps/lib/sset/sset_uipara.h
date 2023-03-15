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
