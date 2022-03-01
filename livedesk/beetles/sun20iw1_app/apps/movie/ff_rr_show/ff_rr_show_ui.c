
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : ff_rr_show_ui.c
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/

#include "ff_rr_show_ui.h"

static movie_ff_rr_show_uipara_t uipara_480_272 =
{
    1,
    {
        46, 0, 342, 60
    },
    {
        0, 0, 342, 60
    }
};

static movie_ff_rr_show_uipara_t uipara_720_576 =
{
    1,
    {
        0, 0, 720, 60
    },
    {
        0, 0, 720, 60
    }
};

static movie_ff_rr_show_uipara_t uipara_720_480 =
{
    1,
    {
        0, 0, 720, 60
    },
    {
        0, 0, 720, 60
    }
};
static movie_ff_rr_show_uipara_t uipara_800_480 =
{
    1,
    {
        0, 0, 800, 60
    },
    {
        0, 0, 800, 60
    }
};

movie_ff_rr_show_uipara_t *movie_ff_rr_show_get_uipara(__s32 rotate)
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


static movie_ff_rr_show_interval_t interval_uipara_480_272 =
{
    {
        240, 136, 40, 30
    },
    {
        50, 100, 150, 200, 330, 380, 430, 480
    }
};

static movie_ff_rr_show_interval_t interval_uipara_720_576 =
{
    {
        720 / 2, 480 / 2, 40, 30
    },
    {
        50, 140, 230, 320, 410, 500, 590, 680
    }
};

static movie_ff_rr_show_interval_t interval_uipara_720_480 =
{
    {
        720 / 2, 480 / 2, 40, 30
    },
    {
        50, 140, 230, 320, 410, 500, 590, 680
    }
};
static movie_ff_rr_show_interval_t interval_uipara_800_480 =
{
    {
        800 / 2, 480 / 2, 40, 30
    },
    {
        50, 150,  250,  350, 450, 550, 650, 750
    }
};

movie_ff_rr_show_interval_t *movie_ff_rr_show_get_interval_uipara(__s32 rotate)
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
                return &interval_uipara_480_272;
            }
            else if ((screen_width == 720) && (screen_height == 576))
            {
                return &interval_uipara_720_576;
            }
            else if ((screen_width == 720) && (screen_height == 480))
            {
                return &interval_uipara_720_480;
            }
            else if ((screen_width == 800) && (screen_height == 480))
            {
                return &interval_uipara_800_480;
            }
        }

        case GUI_SCNDIR_ROTATE90:
        case GUI_SCNDIR_ROTATE270:
            break;
    }

    return &interval_uipara_800_480;
}


