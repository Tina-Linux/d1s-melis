/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : photo_uipara.c
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/07 15:40  create this file, implements the fundamental interface;
**************************************************************************************************************
*/

#include "photo_uipara.h"

//计算单个长度 = (总长 - (头 + 尾) - 总间隔) / 个数; 其中:(头 + 尾) = (_margin) * 2, 总间隔 = 单个间隔 * (个数 - 1)
#define CALC_UNIT_LEN_(_total, _sp, _count, _margin)  (((_total) - (_sp) * ((_count) - 1) - (_margin) * 2) / (_count))
//
#define CALC_UNIT_OFFSET_(_total, _sp, _count, _margin, _fs) (((_total) + (_sp) - ((_sp) + (_fs)) * (_count) - (_margin) * 2) / ((_count) * 2))
//////////////////////////////////////////////////////////////////////////
//400x240 LCD
#define PHOTO_LCD_W                    640//..
#define PHOTO_LCD_H                    480//..

#define MENU_BG_H                      59
#define MENU_BG_W                      PHOTO_LCD_W
#define MENU_BG_X                      0
#define MENU_BG_Y                      (PHOTO_LCD_H - MENU_BG_H)

#define MENU_BTN_BMP_W                 43
#define MENU_BTN_BMP_H                 41
#define MENU_BTN_SP_W                  2
#define MENU_BTN_SP_H                  MENU_BTN_BMP_H//41
#define MENU_BTN_W                     CALC_UNIT_LEN_(MENU_BG_W, MENU_BTN_SP_W, PHOTO_MMENU_PAGE_NUM, 0)
#define MENU_BTN_H                     MENU_BG_H
#define MENU_BTN_BMP_X_OFFSET          ((MENU_BTN_W - MENU_BTN_BMP_W) / 2)
#define MENU_BTN_BMP_Y_OFFSET          2
#define MENU_BTN_TXT_Y_OFFSET          (MENU_BTN_BMP_Y_OFFSET + MENU_BTN_BMP_H)

#define SUBMENU_LIST_Y_END_OFFSET      4

#define INFO_ICON_W                    20
#define INFO_ICON_H                    20

#define VOLUME_BG_W                    356
#define VOLUME_BG_H                    24
#define VOLUME_SLIDE_BG_W              255
#define VOLUME_SLIDE_BG_H              4
#define VOLUME_ICON_W                  15
#define VOLUME_ICON_H                  13
#define SLIDE_BLOCK_LEN                5
#define VOLUME_TXT_W                   36
#define VOLUME_BG_X                    ((PHOTO_LCD_W - VOLUME_BG_W) / 2)
#define VOLUME_BG_Y                    12
#define VOLUME_SLIDE_BG_X              36
#define VOLUME_SLIDE_BG_Y              ((VOLUME_BG_H - VOLUME_SLIDE_BG_H) / 2)
#define VOLUME_ICON_L_X                18
#define VOLUME_SP_ICON_SLIDE           (VOLUME_SLIDE_BG_X - VOLUME_ICON_L_X - VOLUME_ICON_W)
#define VOLUME_ICON_R_X                (VOLUME_SLIDE_BG_X + VOLUME_SLIDE_BG_W + VOLUME_SP_ICON_SLIDE)
#define VOLUME_ICON_Y                  ((VOLUME_BG_H - VOLUME_ICON_H) / 2)

static PHOTO_UI_PARA_T photo_ui_480x272 =
{
    {
        //rt
        100, 60, PHOTO_LCD_W, PHOTO_LCD_H
    },
    {
        //PHOTO_UI_COLOUR_T
        APP_COLOR_WHITE, APP_COLOR_YELLOW                   // 文字正常及焦点时的颜色
    },
    {
        //PHOTO_BG_UI_T
        {
            //PHOTO_INFO_UI_T
            {
                //rt
                40, 40, (PHOTO_LCD_W - 40 * 2), (PHOTO_LCD_H - 40)
            },
            {0, 0},                                         // icon 位置(相对于INFO rect)
            {12, 4},                                        // text 开始的位置(相对于INFO rect)
            44,                                             // text file name space
            20,                                             // text line space
            284,                                            // text line width
        },
        {
            //PHOTO_STATE_UI_T
            {
                //rt
                229, 18, 81, 36
            }
        },
    }
};

static PHOTO_UI_PARA_T photo_ui_800x480 =
{
    {
        //layer rt
        (PHOTO_LCD_W - 397) / 2, (PHOTO_LCD_H - 224) / 2, 397, 224
    },
    {
        //PHOTO_UI_COLOUR_T
        APP_COLOR_WHITE, APP_COLOR_YELLOW                   // 文字正常及焦点时的颜色
    },
    {
        //PHOTO_BG_UI_T
        {
            //PHOTO_INFO_UI_T
            {
                //rt
                0, 0, 397, 224
            },
            {0, 0},                                         // icon 位置(相对于INFO rect)
            {12, 4},                                        // text 开始的位置(相对于INFO rect)
            44,                                             // text file name space
            20,                                             // text line space
            284,                                            // text line width
        },
        {
            //PHOTO_STATE_UI_T
            {
                //rt
                229, 18, 81, 36
            }
        },
    }
};


PHOTO_UI_PARA_T *get_photo_uipara(void)
{
    __u32 w, h;
    dsk_display_get_size(&w, &h);
__log("get_photo_uipara");
    if (800 == w && h == 480)
    {
        return &photo_ui_800x480;
    }
    
    if (480 == w && h == 640)
    {
        return &photo_ui_800x480;
    }
    
    if (480 == w && h == 272)
    {
        return &photo_ui_480x272;
    }

    return &photo_ui_480x272;
}


