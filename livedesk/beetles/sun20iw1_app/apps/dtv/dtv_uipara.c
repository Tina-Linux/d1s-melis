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
#include <log.h>
#include <emodules/mod_display.h>
#include "dtv_uipara.h"
#include <const.h>
static __movie_uipara_t movie_uipara_800_480 =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {800, 480},             //fb_size
        {0, 0, 800, 480},       //src_rect
        {0, 0, 800, 480},       //scr_rect
        {0},
    },
    {
        //sub
        PIXEL_COLOR_ARGB8888,   //fmt
        {800, 480},             //fb_size
        {0, 0, 800, 480},       //src_rect
        {0, 0, 800, 480},       //scr_rect
        {0}
    },
    {0, 44, 620, 414},          // preview
};

static __movie_uipara_t movie_uipara_480_272 =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {800, 300},             //fb_size
        {0, 0, 800, 300},       //src_rect
        {0, 190, 800, 300},         //scr_rect
        {0}
    },
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {800, 300},             //fb_size
        {0, 0, 800, 300},       //src_rect
        {0, 190, 800, 300},         //scr_rect
        {0}
    },
    {0, 44, 300, 190},
};

static __movie_uipara_t movie_uipara_720p =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {1000, 300},                //fb_size
        {0, 0, 1000, 300},      //src_rect
#if (EPDK_ARCH == EPDK_ARCH_SUNI)
        {0 + 112, 190 + 120, 1000, 300},    //scr_rect
#else
        {0 + 140, 190 + 120 + 50, 1000, 300},
#endif
    {0}
    },
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {1000, 300},                //fb_size
        {0, 0, 1000, 300},      //src_rect
#if (EPDK_ARCH == EPDK_ARCH_SUNI)
        {0 + 112, 190 + 120, 1000, 300},    //scr_rect
#else
        {0 + 140, 190 + 120 + 50, 1000, 300},
#endif
    {0}
    },
    {0, 44, 300, 190},
};

static __movie_uipara_t movie_uipara_1080p =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {1200, 600},            //fb_size
        {0, 0, 1200, 600},      //src_rect
#if (EPDK_ARCH == EPDK_ARCH_SUNI)
        {0 + 112, 190 + 120, 1200, 600},    //scr_rect
#else
        {0 + 360, 190 + 240, 1200, 600},
#endif
    {0}
    },
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {1200, 600},            //fb_size
        {0, 0, 1200, 600},      //src_rect
#if (EPDK_ARCH == EPDK_ARCH_SUNI)
        {0 + 112, 190 + 120, 1200, 600},    //scr_rect
#else
        {0 + 360, 190 + 240, 1200, 600},
#endif
    {0}

    },
    {0, 44, 300, 190},
};

static __movie_uipara_t movie_uipara_720_576 =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {720, 576},             //fb_size
        {0, 0, 720, 300},       //src_rect
        {0, 240, 720, 300},     //scr_rect
        {0}
        
    },
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {720, 576},             //fb_size
        {0, 0, 720, 300},       //src_rect
        {0, 240, 720, 300},     //scr_rect
        {0}
        
    },
    {0, 44, 540, 510},
};

static __movie_uipara_t movie_uipara_1024_768 =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {800, 300},             //fb_size
        {0, 0, 800, 300},       //src_rect
        {0, 190, 800, 300},         //scr_rect
        {0}
    },
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {800, 300},             //fb_size
        {0, 0, 800, 300},       //src_rect
        {0, 190, 800, 300},         //scr_rect
        {0}
    },
    {0, 44, 300, 190},
};

static __movie_uipara_t movie_uipara_720_480 =
{
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {720, 480},             //fb_size
        {0, 0, 720, 300},       //src_rect
        {0, 155, 720, 300},     //scr_rect
        {0}
    },
    {
        PIXEL_COLOR_ARGB8888,   //fmt
        {720, 480},             //fb_size
        {0, 0, 720, 300},       //src_rect
        {0, 155, 720, 300},     //scr_rect
        {0}
    },
    {0, 44, 540, 414},
};




__movie_uipara_t *movie_get_uipara(void)
{
    __mp         *mp_display;
    __s32           screen_width;
    __s32           screen_height;
    /* get lcd size*/
	mp_display = esKSRV_Get_Display_Hld();

    if (mp_display == ((__mp *)0))
    {
        __inf("%s %d : can not open display driver\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    screen_height = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    screen_width  = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_WIDTH, 0, 0);

    if ((screen_width == 800) && (screen_height == 480))
    {
        return &movie_uipara_800_480;
    }
    else if ((screen_width == 1920) && screen_height == 1080)
    {
        return &movie_uipara_1080p;
    }
    else if ((screen_width == 1280) && (screen_height == 720))
    {
        return &movie_uipara_720p;
    }
    else if ((screen_width == 1024) && (screen_height == 768))
    {
        return &movie_uipara_1024_768;    //VGA
    }
    else if ((screen_width == 720) && (screen_height == 576))
    {
        return &movie_uipara_720_576;
    }
    else if ((screen_width == 720) && (screen_height == 480))
    {
        return &movie_uipara_720_480;
    }
    else if ((screen_width == 480) && (screen_height == 272))
    {
        return &movie_uipara_480_272;
    }
    //  else if ((screen_width == 1024)&&(screen_height == 600))
    //      return &movie_uipara_1024_600;
    else
    {
        return &movie_uipara_720_480;
    }
}
