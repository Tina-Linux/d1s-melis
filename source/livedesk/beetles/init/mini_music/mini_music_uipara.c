/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : miniplay_uipara.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/
#include "mod_init_i.h"
#include "mini_music_uipara.h"

static __miniplay_uipara_t miniplay_layout_lcd =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {256, 128},             //fb_size
        {0, 0, 256, 128},       //src_rect
        {0, 0, 256, 128},       //scr_rect中x,y的范围由其他应用程序控制
        {0},
    },
    {
        3, 8, 250, 112, {0},      //frmwin
    },
    {
        //songname
        50, 13, 150, 32, {0},
    },
    {
        //slide
        20, 64, 266, 4, {0},
    },
    {
        //pre
        29, 61, 25, 20, {0},
    },
    {
        //next
        192, 61, 25, 20, {0},
    },
    {
        //playpause
        110, 56, 30, 30, {0},
    },
    {
        //time
        60, 20, 190, 40, {0},
    },
    {
        //exit
        213, 11, 27, 27, {0},
    },
    {
        //music
        10, 11, 26, 26, {0},
    },
    {
        //bg
        3, 8, 250, 112, {0},
    }
};

static __miniplay_uipara_t miniplay_layout_480i =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {256, 128},             //fb_size
        {0, 0, 256, 128},       //src_rect
        {0, 0, 256, 128},       //scr_rect中x,y的范围由其他应用程序控制
        {0},
    },
    {
        3, 8, 250, 112, {0},      //frmwin
    },
    {
        //songname
        50, 13, 150, 32, {0},
    },
    {
        //slide
        20, 64, 266, 4, {0},
    },
    {
        //pre
        29, 61, 25, 20, {0},
    },
    {
        //next
        192, 61, 25, 20, {0},
    },
    {
        //playpause
        110, 56, 30, 30, {0},
    },
    {
        //time
        60, 20, 190, 40, {0},
    },
    {
        //exit
        213, 11, 27, 27, {0},
    },
    {
        //music
        10, 11, 26, 26, {0},
    },
    {
        //bg
        3, 8, 250, 112, {0},
    }
};

static __miniplay_uipara_t miniplay_layout_480p =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {256, 128},             //fb_size
        {0, 0, 256, 128},       //src_rect
        {0, 0, 256, 128},       //scr_rect中x,y的范围由其他应用程序控制
        {0},
    },
    {
        3, 8, 250, 112, {0},      //frmwin
    },
    {
        //songname
        50, 13, 150, 32, {0},
    },
    {
        //slide
        20, 64, 266, 4, {0},
    },
    {
        //pre
        29, 61, 25, 20, {0},
    },
    {
        //next
        192, 61, 25, 20, {0},
    },
    {
        //playpause
        110, 56, 30, 30, {0},
    },
    {
        //time
        60, 20, 190, 40, {0},
    },
    {
        //exit
        213, 11, 27, 27, {0},
    },
    {
        //music
        10, 11, 26, 26, {0},
    },
    {
        //bg
        3, 8, 250, 112, {0},
    }

};

static __miniplay_uipara_t miniplay_layout_720p =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {256, 128},             //fb_size
        {0, 0, 256, 128},       //src_rect
        {0, 0, 256, 128},       //scr_rect中x,y的范围由其他应用程序控制
        {0},
    },
    {
        3, 8, 250, 112, {0},      //frmwin
    },
    {
        //songname
        50, 13, 150, 32, {0},
    },
    {
        //slide
        20, 64, 266, 4, {0},
    },
    {
        //pre
        29, 61, 25, 20, {0},
    },
    {
        //next
        192, 61, 25, 20, {0},
    },
    {
        //playpause
        110, 56, 30, 30, {0},
    },
    {
        //time
        60, 20, 190, 40, {0},
    },
    {
        //exit
        213, 11, 27, 27, {0},
    },
    {
        //music
        10, 11, 26, 26, {0},
    },
    {
        //bg
        3, 8, 250, 112, {0},
    }
};

static __miniplay_uipara_t miniplay_layout_VGA =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {256, 128},             //fb_size
        {0, 0, 256, 128},       //src_rect
        {0, 0, 256, 128},       //scr_rect中x,y的范围由其他应用程序控制
        {0},
    },
    {
        3, 8, 250, 112, {0},      //frmwin
    },
    {
        //songname
        50, 13, 150, 32, {0},
    },
    {
        //slide
        20, 64, 266, 4, {0},
    },
    {
        //pre
        29, 61, 25, 20, {0},
    },
    {
        //next
        192, 61, 25, 20, {0},
    },
    {
        //playpause
        110, 56, 30, 30, {0},
    },
    {
        //time
        60, 20, 190, 40, {0},
    },
    {
        //exit
        213, 11, 27, 27, {0},
    },
    {
        //music
        10, 11, 26, 26, {0},
    },
    {
        //bg
        3, 8, 250, 112, {0},
    }

};

static __miniplay_uipara_t miniplay_layout_1080p =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {256, 128},             //fb_size
        {0, 0, 256, 128},       //src_rect
        {0, 0, 256, 128},       //scr_rect中x,y的范围由其他应用程序控制
        {0},
    },
    {
        3, 8, 250, 112, {0},      //frmwin
    },
    {
        //songname
        50, 13, 150, 32, {0},
    },
    {
        //slide
        20, 64, 266, 4, {0},
    },
    {
        //pre
        29, 61, 25, 20, {0},
    },
    {
        //next
        192, 61, 25, 20, {0},
    },
    {
        //playpause
        110, 56, 30, 30, {0},
    },
    {
        //time
        60, 20, 190, 40, {0},
    },
    {
        //exit
        213, 11, 27, 27, {0},
    },
    {
        //music
        10, 11, 26, 26, {0},
    },
    {
        //bg
        3, 8, 250, 112, {0},
    }
};

__miniplay_uipara_t *miniplay_get_uipara(void)
{
    //  ES_FILE         *disp;
    __s32           screen_width;
    __s32           screen_height;
    /* get lcd size*/
    dsk_display_get_size(&screen_width, &screen_height);

    if ((screen_width == 800) && (screen_height == 480))
    {
        return &miniplay_layout_lcd;
    }

    if ((screen_width == 720) && (screen_height == 576))
    {
        return &miniplay_layout_480i;
    }

    if ((screen_width == 1920) && screen_height == 1080)
    {
        return &miniplay_layout_1080p;
    }
    else if ((screen_width == 1280) && (screen_height == 720))
    {
        return &miniplay_layout_720p;
    }
    else if ((screen_width == 720) && (screen_height == 480))
    {
        return &miniplay_layout_480p;
    }
    else if ((screen_width == 1024) && (screen_height == 768))
    {
        return &miniplay_layout_VGA;
    }

    return NULL;
}
