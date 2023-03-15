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
