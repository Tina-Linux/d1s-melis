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
#ifndef __MAIN_MENU_H_
#define __MAIN_MENU_H_

#include "beetles_app.h"
#include "multi_screen_ui.h"

typedef struct tag_desktop_scene_attr
{
    H_LYR                   layer;

    GUI_FONT               *pfont;
    pmulti_screen_ui_t      pui;

    int32_t                 last_focus_item;
    int32_t                 cur_focus_item;
    int32_t                 item_total;         //总个数

    int32_t                 focus_txt_color;
    int32_t                 unfocus_txt_color;
#if 0 /*shqil change For D100, does need't this*/
    GUI_POINT               touchdown_scPt;
    GUI_POINT               touchup_scPt;
    __bool                  touchmove;
    __bool                  touchdown;
#endif
} desktop_scene_attr_t, *pdesktop_scene_attr_t;

#define     SIZEOFFX    30
#define     SIZEOFFY    30

static GUI_RECT uf_rect_800_480[MULTI_SCREEN_STRING_MAX] =
{
#if 0
    {25,  150, 20  + 125, 150 + 100},
    {190, 150, 190 + 125, 150 + 100},
    {350, 150, 350 + 125, 150 + 100},
    {500, 150, 500 + 125, 150 + 100},

    {25,  280, 20  + 125, 280 + 100},
    {190, 280, 190 + 125, 280 + 100},
    {350, 280, 350 + 125, 280 + 100},
    {500, 280, 500 + 125, 280 + 100},
#endif
    {16,                                    80,     16 + 180,                                   80 + 142},
    {16 + 180 + 16,                         80,     16 + 180 + 16 + 180,                        80 + 142},
    {16 + 180 + 16 + 180 + 16,              80,     16 + 180 + 16 + 180 + 16 + 180,             80 + 142},
    {16 + 180 + 16 + 180 + 16 + 180 + 16,   80,     16 + 180 + 16 + 180 + 16 + 180 + 16 + 180,  80 + 142},

    {16,                                    80 + 50 + 142,  16 + 180,                                   80 + 142 + 50 + 142},
    {16 + 180 + 16,                         80 + 50 + 142,  16 + 180 + 16 + 180,                        80 + 142 + 50 + 142},
    {16 + 180 + 16 + 180 + 16,              80 + 50 + 142,  16 + 180 + 16 + 180 + 16 + 180,             80 + 142 + 50 + 142},
    {16 + 180 + 16 + 180 + 16 + 180 + 16,   80 + 50 + 142,  16 + 180 + 16 + 180 + 16 + 180 + 16 + 180,  80 + 142 + 50 + 142},

};

static GUI_RECT  string_rect_800_480[MULTI_SCREEN_STRING_MAX] =
{
#if 0
    {65,  185, 100, 275},
    {230, 185, 280, 275},
    {395, 185, 440, 275},
    {535, 185, 600, 275},

    {65,  310, 115, 410},
    {230, 310, 280, 410},
    {395, 310, 440, 410},
    {535, 310, 600, 410},
#endif
    {16 + 45,                                   80 + 100,   16 + 180 - 45,                                      80 + 142},
    {16 + 180 + 16 + 45,                        80 + 100,   16 + 180 + 16 + 180 - 45,                           80 + 142},
    {16 + 180 + 16 + 180 + 16 + 45,             80 + 100,   16 + 180 + 16 + 180 + 16 + 180 - 45,                80 + 142},
    {16 + 180 + 16 + 180 + 16 + 180 + 16 + 45,  80 + 100,   16 + 180 + 16 + 180 + 16 + 180 + 16 + 180 - 45,     80 + 142},

    {16 + 45,                                   80 + 50 + 100 + 142,    16 + 180 - 45,                                      80 + 142 + 50 + 142},
    {16 + 180 + 16 + 45,                        80 + 50 + 100 + 142,    16 + 180 + 16 + 180 - 45,                           80 + 142 + 50 + 142},
    {16 + 180 + 16 + 180 + 16 + 45,             80 + 50 + 100 + 142,    16 + 180 + 16 + 180 + 16 + 180 - 45,                80 + 142 + 50 + 142},
    {16 + 180 + 16 + 180 + 16 + 180 + 16 + 45,  80 + 50 + 100 + 142,    16 + 180 + 16 + 180 + 16 + 180 + 16 + 180 - 45,     80 + 142 + 50 + 142},

};
extern H_WIN    desktop_scene_create(H_WIN h_parent);
extern int32_t  desktop_scene_destory(H_WIN mm_win);
extern void     dsktop_scene_set_cur_app(uint8_t index);
extern uint8_t  dsktop_scene_get_cur_app(void);



#endif
