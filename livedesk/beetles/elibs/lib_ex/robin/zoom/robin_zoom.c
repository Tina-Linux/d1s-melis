/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_zoom.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.27
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.09.27       1.1.0        build the file
*
************************************************************************************************************************
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
    { CEDAR_VID_WINDOW_BESTSHOW, ROBIN_ZOOM_FIT_VIEW                 }
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
    esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_VID_SHOW_MODE, cedar_zoom, NULL);
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
