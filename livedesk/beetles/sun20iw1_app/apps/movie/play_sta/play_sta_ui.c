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
#include "play_sta_ui.h"

static movie_playsta_uipara_t uipara_480_272 =
{
    1,
    {
        30, 48, 18, 21,
        {0,0},
        {0,0},
    },
    {
        0, 0, 18, 21,
        {0,0},
        {0,0},
    },
    {{
            0, 0, 18, 21,
            {
                ID_MOVIE_PAUSESTA_BMP,
                ID_MOVIE_PLAYSTA_BMP
            },
            {0,0}
        }
    }
};

static movie_playsta_uipara_t uipara_720_576 =
{
    1,
    {
        30, 48, 18, 21,
        {0,0},
        {0,0},
    },
    {
        0, 0, 18, 21,
        {0,0},
        {0,0},
    },
    {{
            0, 0, 18, 21,
            {
                ID_MOVIE_PAUSESTA_BMP,
                ID_MOVIE_PLAYSTA_BMP
            },
            {0,0}
        }
    }
};

static movie_playsta_uipara_t uipara_720_480 =
{
    1,
    {
        30, 48, 18, 21,
        {0,0},
        {0,0},
    },
    {
        0, 0, 18, 21,
        {0,0},
        {0,0},
    },
    {{
            0, 0, 18, 21,
            {
                ID_MOVIE_PAUSESTA_BMP,
                ID_MOVIE_PLAYSTA_BMP
            },
            {0,0}
        }
    }
};

static movie_playsta_uipara_t uipara_800_480 =
{
    1,
    {
        30, 48, 18, 21,
        {0,0},
        {0,0},
    },
    {
        0, 0, 18, 21,
        {0,0},
        {0,0},
    },
    {{
            0, 0, 18, 21,
            {
                ID_MOVIE_PAUSESTA_BMP,
                ID_MOVIE_PLAYSTA_BMP
            },
            {0,0},
        }
    }
};


movie_playsta_uipara_t *movie_playsta_get_uipara(__s32 rotate)
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
