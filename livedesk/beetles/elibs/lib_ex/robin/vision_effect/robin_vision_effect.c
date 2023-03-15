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
#ifndef  __robin_vision_effect_c
#define  __robin_vision_effect_c


#include "robin_i.h"

typedef enum
{
    DISP_VIDEO_NATUAL       = 0,
    DISP_VIDEO_SOFT         = 1,
    DISP_VIDEO_VERYSOFT     = 2,
    DISP_VIDEO_SHARP        = 3,
    DISP_VIDEO_VERYSHARP    = 4
} __disp_video_smooth_t;
/***************************************************************************************************
*Name        : robin_set_vision_effect
*Prototype   : __s32  robin_set_vision_effect( __disp_video_smooth_t  vision_effect )
*Arguments   : vision_effect    input. vision effect mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set vision effect mode, such natual, sharp and so on.
*Other       :
***************************************************************************************************/
__s32  robin_set_vision_effect(__disp_video_smooth_t  vision_effect)
{
    return 0;
}



/***************************************************************************************************
*Name        : robin_get_vision_effect
*Prototype   : __disp_video_smooth_t  robin_get_vision_effect( void )
*Arguments   : void
*Return      : the current vision effect.
*Description : get the current vision effect .
*Other       :
***************************************************************************************************/
__disp_video_smooth_t  robin_get_vision_effect(void)
{
    return (__disp_video_smooth_t) 0;
}



#endif     //  ifndef __robin_vision_effect_c

/* end of robin_vision_effect.c */
