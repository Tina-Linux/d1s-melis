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
#ifndef  __robin_zoom_c
#define  __robin_zoom_c

#include <emodules/mod_display.h>
#include "robin_i.h"
#include <kconfig.h>

typedef struct _zoom_map_t
{
    __cedar_vide_window_ratio_mode_t  cedar_zoom;
    robin_zoom_e                        robin_zoom;
} zoom_map_t;


static robin_zoom_e  cur_zoom = ROBIN_ZOOM_UNKNOWN;

static zoom_map_t  zoom_map_table[ ] =
{
    { CEDAR_VID_WINDOW_BESTSHOW, ROBIN_ZOOM_FULL_SCREEN_VIDEO_RATIO  },
    { CEDAR_VID_WINDOW_FULLSCN, ROBIN_ZOOM_FULL_SCREEN_SCREEN_RATIO },
    { CEDAR_VID_WINDOW_ORIGINAL, ROBIN_ZOOM_ORIGIN_SIZE              },
    { CEDAR_VID_WINDOW_4R3MODE, ROBIN_ZOOM_4R3                      },
    { CEDAR_VID_WINDOW_16R9MODE, ROBIN_ZOOM_16R9                     },
    { CEDAR_VID_WINDOW_BESTSHOW, ROBIN_ZOOM_FIT_VIEW                 },
    { CEDAR_VID_WINDOW_USERDEF, ROBIN_ZOOM_USERDEF                 }
};

#define ZOOM_MAP_TABLE_LEN              ( sizeof(zoom_map_table) / sizeof(zoom_map_table[0]) )


static robin_zoom_e                        map_cedar2crs(__cedar_vide_window_ratio_mode_t  cedar_zoom);
static __cedar_vide_window_ratio_mode_t  map_crs2cedar(robin_zoom_e  robin_zoom);
static __s32  get_screen_size(__u32 *width_p, __u32 *height_p);


/***************************************************************************************************
*Name        : robin_set_zoom
*Prototype   : __s32  robin_set_zoom( robin_zoom_e zoom )
*Arguments   : zoom      input. zoom mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set video zoom mode.
*Other       :
***************************************************************************************************/
__s32  robin_set_zoom(robin_zoom_e zoom)
{
    __cedar_vide_window_ratio_mode_t   cedar_zoom;
    __u32  screen_width;
    __u32  screen_height;
    __s32  x;
    __s32  y;
    __s32  width;
    __s32  height;
    SrcScnWinPara UserDef_SrcScnWinPara;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    cedar_zoom = map_crs2cedar(zoom);
    get_screen_size(&screen_width, &screen_height);
    x      = 0;
    y      = 0;
    width  = screen_width;
    height = screen_height;

    if (zoom == ROBIN_ZOOM_FIT_VIEW)
    {
        enum disp_output_type  output;
        output  = (enum disp_output_type)esMODS_MIoctrl(robin_hdis, MOD_DISP_GET_OUTPUT_TYPE, 0, 0) ;

        if (output == DISP_OUTPUT_TYPE_TV || output == DISP_OUTPUT_TYPE_HDMI)
        {
            if (screen_width == 1280 && screen_height == 720)
            {
                x      = (1280 - 1208) >> 1;
                y      = (720 - 680) >> 1;
                width  = 1208;
                height = 680;
            }
            else if (screen_width == 1920 && screen_height == 1080)
            {
                x      = (1920 - 1812) >> 1;
                y      = (1080 - 1020) >> 1;
                width  = 1812;
                height = 1020;
            }
            else if (screen_width == 720 && screen_height == 480)
            {
                x      = (720 - 660) >> 1;
                y      = (480 - 440) >> 1;
                width  = 660;
                height = 440;
            }
            else if (screen_width == 720 && screen_height == 576)
            {
                x      = (720 - 660) >> 1;
                y      = (576 - 536) >> 1;
                width  = 660;
                height = 536;
            }
        }
    }

    robin_set_video_win(x, y, width, height);
    #if 0
    UserDef_SrcScnWinPara.SrcWin.x      = 400;
    UserDef_SrcScnWinPara.SrcWin.y      = 240;
    UserDef_SrcScnWinPara.SrcWin.width  = 400;
    UserDef_SrcScnWinPara.SrcWin.height = 240;
    UserDef_SrcScnWinPara.ScnWin.x      = 0;
    UserDef_SrcScnWinPara.ScnWin.y      = 0;
    UserDef_SrcScnWinPara.ScnWin.width  = width;
    UserDef_SrcScnWinPara.ScnWin.height = height;
    __log("[>_<]para:%d,%d", width, height);
    if (cedar_zoom == CEDAR_VID_WINDOW_USERDEF)
    {
        esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_VID_SHOW_MODE, cedar_zoom, &UserDef_SrcScnWinPara);
    }
    else
    #endif
    {
        esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_VID_SHOW_MODE, cedar_zoom, NULL);
    }
    cur_zoom = zoom;
    return 0;
}



/***************************************************************************************************
*Name        : robin_get_zoom
*Prototype   : robin_zoom_e  robin_get_zoom( void )
*Arguments   : void
*Return      : the current zoom mode.
*Description : get the current zoom mode.
*Other       :
***************************************************************************************************/
robin_zoom_e  robin_get_zoom(void)
{
    __cedar_vide_window_ratio_mode_t  cedar_zoom;

    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    if (cur_zoom == ROBIN_ZOOM_UNKNOWN)
    {
        cedar_zoom = (__cedar_vide_window_ratio_mode_t)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_VID_SHOW_MODE, 0, NULL);
        return map_cedar2crs(cedar_zoom);
    }
    else
    {
        return cur_zoom;
    }
}



static robin_zoom_e  map_cedar2crs(__cedar_vide_window_ratio_mode_t  cedar_zoom)
{
    __u32  i;

    for (i = 0;  i < ZOOM_MAP_TABLE_LEN;  i++)
    {
        if (zoom_map_table[i].cedar_zoom == cedar_zoom)
        {
            return zoom_map_table[i].robin_zoom;
        }
    }

    return ROBIN_ZOOM_FULL_SCREEN_VIDEO_RATIO;
}



static __cedar_vide_window_ratio_mode_t  map_crs2cedar(robin_zoom_e  robin_zoom)
{
    __u32  i;

    for (i = 0;  i < ZOOM_MAP_TABLE_LEN;  i++)
    {
        if (zoom_map_table[i].robin_zoom == robin_zoom)
        {
            return zoom_map_table[i].cedar_zoom;
        }
    }

    return CEDAR_VID_WINDOW_BESTSHOW;
}



static __s32  get_screen_size(__u32 *width_p, __u32 *height_p)
{
    if (width_p == NULL || height_p == NULL)
    {
        return -1;
    }

    *width_p  = esMODS_MIoctrl(robin_hdis, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    *height_p = esMODS_MIoctrl(robin_hdis, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    return 0;
}


#endif     //  ifndef __robin_zoom_c

/* end of robin_zoom.c */
