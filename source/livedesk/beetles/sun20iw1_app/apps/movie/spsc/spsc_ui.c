/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : spsc_ui.c
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include "spsc_ui.h"

static movie_spsc_uipara_t uipara_480_272 =
{
    1,

    {
        0, 28, 480, 70
    }
};

static movie_spsc_uipara_t uipara_720_576 =
{
    1,

    {
        0, 46, 720, 56
    }
};

static movie_spsc_uipara_t uipara_720_480 =
{
    1,

    {
        0, 46, 720, 56
    }
};

static movie_spsc_uipara_t uipara_800_480 =
{
    1,

    {
        0, 46, 800, 56
    }
};

movie_spsc_uipara_t *movie_spsc_get_uipara(__s32 rotate)
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
            else if ((screen_width == 720) && (screen_height == 576))
            {
                return &uipara_720_576;
            }
            else if ((screen_width == 720) && (screen_height == 480))
            {
                return &uipara_720_480;
            }
            else if ((screen_width == 800) && (screen_height == 480))
            {
                return &uipara_800_480;
            }
        }

        case GUI_SCNDIR_ROTATE90:
        case GUI_SCNDIR_ROTATE270:
            break;
    }

    return &uipara_480_272;
}

