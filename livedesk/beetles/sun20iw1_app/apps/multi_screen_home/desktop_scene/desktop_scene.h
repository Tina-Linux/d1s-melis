/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : main_menu.h
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
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
    int32_t                 item_total;         //×Ü¸öÊý

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
