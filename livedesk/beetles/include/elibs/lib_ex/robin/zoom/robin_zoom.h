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
#ifndef  __robin_zoom_h
#define  __robin_zoom_h
#include <kconfig.h>

typedef enum
{
    ROBIN_ZOOM_FULL_SCREEN_VIDEO_RATIO = 0,    // 全屏，按视频比例
    ROBIN_ZOOM_FULL_SCREEN_SCREEN_RATIO,       // 全屏，按屏幕比例
    ROBIN_ZOOM_ORIGIN_SIZE,                    // 原始尺寸
    ROBIN_ZOOM_4R3,                            // 强制4：3比例，可能会导致变形
    ROBIN_ZOOM_16R9,                           // 强制16：9比例，可能会导致变形
    ROBIN_ZOOM_FIT_VIEW,                       // 适合可见，只应用于TV显示。
    ROBIN_ZOOM_USERDEF,                        //用户自定义,自己确定srcwin, scnwin的参数
    ROBIN_ZOOM_EXIT,
    ROBIN_ZOOM_UNKNOWN = -1
} robin_zoom_e;


/***************************************************************************************************
*Name        : robin_set_zoom
*Prototype   : __s32  robin_set_zoom( robin_zoom_e zoom )
*Arguments   : zoom      input. zoom mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set video zoom mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_zoom(robin_zoom_e zoom);

/***************************************************************************************************
*Name        : robin_get_zoom
*Prototype   : robin_zoom_e  robin_get_zoom( void )
*Arguments   : void
*Return      : the current zoom mode.
*Description : get the current zoom mode.
*Other       :
***************************************************************************************************/
extern robin_zoom_e  robin_get_zoom(void);


#endif     //  ifndef __robin_zoom_h

/* end of robin_zoom.h */
