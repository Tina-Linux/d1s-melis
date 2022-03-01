
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : prog_bar_ui.c
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include "apps.h"
#include "prog_bar_ui.h"

static init_prog_bar_uipara_t uipara_480_272 =
{
    {
        213 - 20, 4, 134 + 20, 15, {0}, {0},
    },
    {
        0, 0, 0, 0, {0}, {0},
    },
    {
        {
            213 - 20, 4, 134 + 20, 15,
            {ID_INIT_PROG_BG_BMP}, {0},
        },
        {
            213 - 20, 4, 7, 15,
            {ID_INIT_PROG_LEFT_BMP}, {0},
        },
        {
            213 - 20, 4, 7, 15,
            {ID_INIT_PROG_MID_BMP}, {0},
        },
        {
            213 - 20, 4, 20, 15,
            {ID_INIT_PROG_CURSOR_BMP}, {0},
        }
    },
	{0},
	{0},
};

static init_prog_bar_uipara_t uipara_800_480 =
{
    {
        79, 65, 680, 26, {0}, {0},
    },
    {
        0, 0, 0, 0, {0}, {0},
    },
    {
        {
            79, 65, 680, 26,
            {ID_INIT_PROG_BG_BMP}, {0},
        },
        {
            79, 65, 12, 26,
            {ID_INIT_PROG_LEFT_BMP}, {0},
        },
        {
            79, 65, 12, 26,
            {ID_INIT_PROG_MID_BMP}, {0},
        },
        {
            79, 65, 26, 26,
            {ID_INIT_PROG_CURSOR_BMP}, {0},
        },
    },
    {
        0, 46, 800, 68,
        {ID_INIT_VOLUME_BG_BMP}, {0},
    },
    {
        0, 46, 800, 68,
        {ID_INIT_BRIGHT_BG_BMP}, {0},
    }


};
static init_prog_bar_uipara_t uipara_720_576 =
{
    {
        222 - 20, 4, 134 + 20, 15, {0}, {0},
    },
    {
        0, 0, 0, 0, {0}, {0},
    },
    {
        {
            222 - 20, 4, 134 + 20, 15,
            {ID_INIT_PROG_BG_BMP}, {0},
        },
        {
            222 - 20, 4, 7, 15,
            {ID_INIT_PROG_LEFT_BMP}, {0},
        },
        {
            222 - 20, 4, 7, 15,
            {ID_INIT_PROG_MID_BMP}, {0},
        },
        {
            222 - 20, 4, 20, 15,
            {ID_INIT_PROG_CURSOR_BMP}, {0},
        }
    },
    {0},
	{0},

};

static init_prog_bar_uipara_t uipara_720_480 =
{
    {
        222 - 20, 4, 134 + 20, 15, {0}, {0},
    },
    {
        0, 0, 0, 0, {0}, {0},
    },
    {
        {
            222 - 20, 4, 134 + 20, 15,
            {ID_INIT_PROG_BG_BMP}, {0},
        },
        {
            222 - 20, 4, 7, 15,
            {ID_INIT_PROG_LEFT_BMP}, {0},
        },
        {
            222 - 20, 4, 7, 15,
            {ID_INIT_PROG_MID_BMP}, {0},
        },
        {
            222 - 20, 4, 20, 15,
            {ID_INIT_PROG_CURSOR_BMP}, {0},
        }
    },
	{0},
	{0},
};

init_prog_bar_uipara_t *init_prog_bar_get_uipara(__s32 rotate)
{
    __s32           screen_width;
    __s32           screen_height;
    /* get lcd size*/
    dsk_display_get_size(&screen_width, &screen_height);

    switch (rotate)
    {
        case GUI_SCNDIR_NORMAL:
        case GUI_SCNDIR_ROTATE180:
        {
            if ((screen_width == 480) && (screen_height == 272))
            {
                return &uipara_480_272;
            }
            else if ((screen_width == 800) && (screen_height == 480))
            {
                return &uipara_800_480;
            }
            else if ((screen_width == 720) && (screen_height == 576))
            {
                return &uipara_720_576;
            }
        }

        case GUI_SCNDIR_ROTATE90:
        case GUI_SCNDIR_ROTATE270:
            break;
    }

    return &uipara_480_272;
}

