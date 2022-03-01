
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : sub_ui.c
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#include "sub_show_ui.h"

static movie_sub_show_uipara_t uipara_480_272 =
{
    1,
    {
        {
            0, 272 - 40, 480, 40
        },
        {
            0, 116, 480, 40
        },
        {
            0, 0, 480, 40
        },
    },
    {
        0, 0, 480, 40
    }
};

static movie_sub_show_uipara_t uipara_720_576 =
{
    1,
    {
        {
            0, 576 - 60, 720, 60
        },
        {
            0, (576 - 60) / 2, 720, 60
        },
        {
            0, 0, 720, 60
        },
    },
    {
        0, 0, 720, 60
    }
};

static movie_sub_show_uipara_t uipara_720_480 =
{
    1,
    {
        {
            0, 480 - 60, 720, 60
        },
        {
            0, (480 - 60) / 2, 720, 60
        },
        {
            0, 0, 720, 60
        },
    },
    {
        0, 0, 720, 60
    }
};


static movie_sub_show_uipara_t uipara_800_480 =
{
    1,
    {
        {
            0, 480 - 60, 800, 60
        },
        {
            0, (480 - 60) / 2, 800, 60
        },
        {
            0, 0, 800, 60
        },
    },
    {
        0, 0, 800, 60
    }
};



movie_sub_show_uipara_t *movie_sub_show_get_uipara(__s32 rotate)
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

    return &uipara_800_480;
}

