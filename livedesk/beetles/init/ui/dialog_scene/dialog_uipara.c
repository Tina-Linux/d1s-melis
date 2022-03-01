/*
************************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                 (c) Copyright 2007-2010, Jackie, China
*                                       All Rights Reserved
*
* File          : dialog_uipara.c
* By            : Jackie.chen
* Func      : dialog uipara
* Version   : v1.0
* ======================================================================================================================
* 2010-11-03  Jackie.chen  create this file, implements the fundemental interface;
************************************************************************************************************************
*/
#include <log.h>
#include "mod_init_i.h"
#include "dialog_uipara.h"

__dialog_uipara_t  dialog_uipara;

void init_dialog_uipara(void)
{
    int32_t     width, height;
    int32_t     screen_type;

    dsk_display_get_size(&width, &height);
    __wrn("======get display size:width=%d,height=%d \n", width, height);

    if ((width == 400) && (height == 240))
    {
        screen_type = SCREEN_TYPE_400_240;
    }
    else if ((width == 480) && (height == 272))
    {
        screen_type = SCREEN_TYPE_400_240;
    }
    else if ((width == 480) && (height == 640))
    {
        screen_type = SCREEN_TYPE_480_640;
    }	
    else if ((width == 720) && (height == 480))
    {
        screen_type = SCREEN_TYPE_720_480;
    }
    else if ((width == 720) && (height == 576))
    {
        screen_type = SCREEN_TYPE_720_576;
    }
    else if ((width == 800) && (height == 480))
    {
        screen_type = SCREEN_TYPE_800_480;
    }
    else if ((width == 1024) && (height == 768))
    {
        screen_type = SCREEN_TYPE_1024_768;
    }
    else if ((width == 1280) && (height == 720))
    {
        screen_type = SCREEN_TYPE_1280_720;
    }
    else if ((width == 1920) && (height == 1080))
    {
        screen_type = SCREEN_TYPE_1920_1080;
    }
    else
    {
        screen_type = SCREEN_TYPE_LAST;
    }

    switch (screen_type)
    {
        case SCREEN_TYPE_400_240:
        {
            /* dialog layer */
            //__here__;
            dialog_uipara.dialog_lyr.fb_w  = 256;
            dialog_uipara.dialog_lyr.fb_h  = 128;
            dialog_uipara.dialog_lyr.src_x = 0;
            dialog_uipara.dialog_lyr.src_y = 0;
            dialog_uipara.dialog_lyr.src_w = 256;
            dialog_uipara.dialog_lyr.src_h = 128;
            dialog_uipara.dialog_lyr.scn_x = (width - 256) / 2;
            dialog_uipara.dialog_lyr.scn_y = (height - 128) / 2;
            dialog_uipara.dialog_lyr.scn_w = 256;
            dialog_uipara.dialog_lyr.scn_h = 128;
            dialog_uipara.dialog_lyr.pipe  = 1;
            break;
        }
		
        case SCREEN_TYPE_480_640:
        {
            /* dialog layer */
            //__here__;
            dialog_uipara.dialog_lyr.fb_w  = 256;
            dialog_uipara.dialog_lyr.fb_h  = 128;
            dialog_uipara.dialog_lyr.src_x = 0;
            dialog_uipara.dialog_lyr.src_y = 0;
            dialog_uipara.dialog_lyr.src_w = 256;
            dialog_uipara.dialog_lyr.src_h = 128;
            dialog_uipara.dialog_lyr.scn_x = (width - 256) / 2;
            dialog_uipara.dialog_lyr.scn_y = (height - 128) / 2;
            dialog_uipara.dialog_lyr.scn_w = 256;
            dialog_uipara.dialog_lyr.scn_h = 128;
            dialog_uipara.dialog_lyr.pipe  = 1;
            break;
        }
		
        case SCREEN_TYPE_720_480:
        {
            /* dialog layer */
            dialog_uipara.dialog_lyr.fb_w  = 512;
            dialog_uipara.dialog_lyr.fb_h  = 256;
            dialog_uipara.dialog_lyr.src_x = 0;
            dialog_uipara.dialog_lyr.src_y = 0;
            dialog_uipara.dialog_lyr.src_w = 512;
            dialog_uipara.dialog_lyr.src_h = 256;
            dialog_uipara.dialog_lyr.scn_x = (width - 512) / 2;
            dialog_uipara.dialog_lyr.scn_y = (height - 256) / 2;
            dialog_uipara.dialog_lyr.scn_w = 512;
            dialog_uipara.dialog_lyr.scn_h = 256;
            dialog_uipara.dialog_lyr.pipe  = 1;
            break;
        }

        case SCREEN_TYPE_720_576:
        {
            /* dialog layer */
            dialog_uipara.dialog_lyr.fb_w  = 512;
            dialog_uipara.dialog_lyr.fb_h  = 256;
            dialog_uipara.dialog_lyr.src_x = 0;
            dialog_uipara.dialog_lyr.src_y = 0;
            dialog_uipara.dialog_lyr.src_w = 512;
            dialog_uipara.dialog_lyr.src_h = 256;
            dialog_uipara.dialog_lyr.scn_x = (width - 512) / 2;
            dialog_uipara.dialog_lyr.scn_y = (height - 256) / 2;
            dialog_uipara.dialog_lyr.scn_w = 512;
            dialog_uipara.dialog_lyr.scn_h = 256;
            dialog_uipara.dialog_lyr.pipe  = 1;
            break;
        }

        case SCREEN_TYPE_LAST:              /* 以800*480 为基准, 居中对齐 */
        case SCREEN_TYPE_800_480:
        {
            /* dialog layer */
            __wrn("===========800 X 480 dialog uipara \n");
            dialog_uipara.dialog_lyr.fb_w  = 512;
            dialog_uipara.dialog_lyr.fb_h  = 256;
            dialog_uipara.dialog_lyr.src_x = 0;
            dialog_uipara.dialog_lyr.src_y = 0;
            dialog_uipara.dialog_lyr.src_w = 512;
            dialog_uipara.dialog_lyr.src_h = 256;
            dialog_uipara.dialog_lyr.scn_x = (width - 512) / 2;
            dialog_uipara.dialog_lyr.scn_y = (height - 256) / 2;
            dialog_uipara.dialog_lyr.scn_w = 512;
            dialog_uipara.dialog_lyr.scn_h = 256;
            dialog_uipara.dialog_lyr.pipe  = 1;
            break;
        }

        case SCREEN_TYPE_1024_768:
        {
            /* dialog layer */
            dialog_uipara.dialog_lyr.fb_w  = 512;
            dialog_uipara.dialog_lyr.fb_h  = 256;
            dialog_uipara.dialog_lyr.src_x = 0;
            dialog_uipara.dialog_lyr.src_y = 0;
            dialog_uipara.dialog_lyr.src_w = 512;
            dialog_uipara.dialog_lyr.src_h = 256;
            dialog_uipara.dialog_lyr.scn_x = (width - 512) / 2;
            dialog_uipara.dialog_lyr.scn_y = (height - 256) / 2;
            dialog_uipara.dialog_lyr.scn_w = 512;
            dialog_uipara.dialog_lyr.scn_h = 256;
            dialog_uipara.dialog_lyr.pipe  = 1;
            break;
        }

        case SCREEN_TYPE_1280_720:
        {
            /* dialog layer */
            dialog_uipara.dialog_lyr.fb_w  = 512;
            dialog_uipara.dialog_lyr.fb_h  = 256;
            dialog_uipara.dialog_lyr.src_x = 0;
            dialog_uipara.dialog_lyr.src_y = 0;
            dialog_uipara.dialog_lyr.src_w = 512;
            dialog_uipara.dialog_lyr.src_h = 256;
            dialog_uipara.dialog_lyr.scn_x = (width - 512) / 2;
            dialog_uipara.dialog_lyr.scn_y = (height - 256) / 2;
            dialog_uipara.dialog_lyr.scn_w = 512;
            dialog_uipara.dialog_lyr.scn_h = 256;
            dialog_uipara.dialog_lyr.pipe  = 1;
            break;
        }

        case SCREEN_TYPE_1920_1080:
        {
            /* dialog layer */
            dialog_uipara.dialog_lyr.fb_w  = 512;
            dialog_uipara.dialog_lyr.fb_h  = 256;
            dialog_uipara.dialog_lyr.src_x = 0;
            dialog_uipara.dialog_lyr.src_y = 0;
            dialog_uipara.dialog_lyr.src_w = 512;
            dialog_uipara.dialog_lyr.src_h = 256;
            dialog_uipara.dialog_lyr.scn_x = (width - 512) / 2;
            dialog_uipara.dialog_lyr.scn_y = (height - 256) / 2;
            dialog_uipara.dialog_lyr.scn_w = 512;
            dialog_uipara.dialog_lyr.scn_h = 256;
            dialog_uipara.dialog_lyr.pipe  = 1;
            break;
        }

        default:
            __err(" unknown screen type \n");
            break;
    };

    return;
}
