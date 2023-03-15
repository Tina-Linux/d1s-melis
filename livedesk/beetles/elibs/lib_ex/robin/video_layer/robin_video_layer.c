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
#ifndef  __robin_video_layer_c
#define  __robin_video_layer_c

#include <emodules/mod_display.h>
#include "robin_i.h"
#include <kconfig.h>

/***************************************************************************************************
*Name        : robin_request_video_layer
*Prototype   : __s32  robin_request_video_layer( const RECT *rect_p, __s32 pipe, __s32 prio )
*Arguments   : rect_p    input. the coordinate and size of the video layer.
*              pipe      input. layer pipe.
*              prio      input. layer priority.
*Return      : the video layer handle. if return NULL, means failing.
*Description : request the video layer.
*Other       :
***************************************************************************************************/
__hdle robin_request_video_layer(const RECT *rect_p, __s32 pipe, __s32 prio)
{
    __disp_layer_info_t     image_layer_info;
    __disp_fb_t             image_fb_para;
    RECT                    image_win;
    __hdle                  hlay = NULL;
    unsigned long arg[3] = {0, };

    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    if (rect_p == NULL)
    {
        return NULL;
    }

    eLIBs_memset(&image_layer_info, 0x00, sizeof(image_layer_info));
    eLIBs_memset(&image_fb_para, 0x00, sizeof(image_fb_para));
    eLIBs_memset(&image_win, 0x00, sizeof(RECT));
    image_fb_para.size.height       = 0;                   // DONT'T CARE
    image_fb_para.size.width        = 0;                   // DONT'T CARE
    image_fb_para.addr[0]           = NULL;
    image_fb_para.addr[1]           = NULL;
    image_fb_para.addr[2]           = NULL;
    image_fb_para.format            = DISP_FORMAT_RGB_565;   // DONT'T CARE
    image_fb_para.seq               = DISP_SEQ_ARGB;        // DONT'T CARE
    image_fb_para.mode              = 0;                    // DONT'T CARE
    image_fb_para.br_swap           = 0;                    // DONT'T CARE
    image_fb_para.cs_mode           = NULL;
    image_layer_info.mode           = MOD_DISP_LAYER_WORK_MODE_SCALER;
    image_layer_info.pipe           = pipe;
    image_layer_info.prio           = prio;
    image_layer_info.alpha_en       = 0;
    image_layer_info.alpha_val      = 255;
    image_layer_info.ck_enable      = 0;
    image_layer_info.src_win.x      = 0;
    image_layer_info.src_win.y      = 0;
    image_layer_info.src_win.width  = rect_p->width ;
    image_layer_info.src_win.height = rect_p->height;
    image_layer_info.scn_win.x      = rect_p->x     ;
    image_layer_info.scn_win.y      = rect_p->y     ;
    image_layer_info.scn_win.width  = rect_p->width ;
    image_layer_info.scn_win.height = rect_p->height;
    image_layer_info.fb             = image_fb_para;
    arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
    arg[1] = 0;
    arg[2] = 0;
    hlay = (__hdle)esMODS_MIoctrl(robin_hdis, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);

    if (hlay == NULL)
    {
        __err("Error in applying for the video layer.");
        goto error;
    }

   // arg[0] = (__u32)hlay;
  //  arg[1] = (__u32)&image_layer_info;
   // arg[2] = 0;
 //   esMODS_MIoctrl(robin_hdis, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
    image_win.x      = rect_p->x;
    image_win.y      = rect_p->y;
    image_win.width  = rect_p->width ;
    image_win.height = rect_p->height;
    __log("pipe = %d, prio = %d, w:%d, h:%d", pipe, prio, rect_p->width, rect_p->height);
            
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_VID_LAYERHDL, 0, (void *)hlay) != EPDK_OK)
    {
        __wrn("Fail in setting video layer handle to cedar!");
        goto error;
    }
    //set video window information to cedar
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_VID_WINDOW, 0, &image_win) != EPDK_OK)
    {
        __wrn("Fail in setting video window information to cedar!");
        goto error;
    }
    return hlay;
error:
    if (hlay != NULL)
    {
        arg[0] = (__u32)hlay;
        arg[1] = 0;
        arg[2] = 0;
        esMODS_MIoctrl(robin_hdis, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
        hlay = NULL;
    }

    return NULL;
}



/***************************************************************************************************
*Name        : robin_release_video_layer
*Prototype   : void   robin_release_video_layer( __hdle hlay_video )
*Arguments   : hlay_video     input. the video layer handle, gotten by call robin_request_video_layer().
*Return      : void
*Description : release the video layer.
*Other       :
***************************************************************************************************/
void   robin_release_video_layer(__hdle hlay_video)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return ;
    }

    if (hlay_video != NULL)
    {
        __u32 arg[3];
        arg[0] = (__u32)hlay_video;
        arg[1] = 0;
        arg[2] = 0;
        esMODS_MIoctrl(robin_hdis, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
        hlay_video = NULL;
    }
}



#endif     //  ifndef __robin_video_layer_c
