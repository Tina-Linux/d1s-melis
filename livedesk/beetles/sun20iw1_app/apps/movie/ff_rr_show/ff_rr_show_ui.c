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
