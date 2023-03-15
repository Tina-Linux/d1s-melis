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
#ifndef  __robin_palette_c
#define  __robin_palette_c


#include "robin_i.h"
#include <kconfig.h>
#include <emodules/mod_display.h>


/***************************************************************************************************
*Name        : robin_set_palette
*Prototype   : __s32 robin_set_palette( const __u32 *palette_buf )
*Arguments   : palette_buf   input. the palette data buffer, whose size must be
*                            ( ROBIN_PALETTE_COLOR_COUNT * 4 ).
*Return      : ==0       succeed
*              !=0       fail
*Description : set the whole palette.
*Other       :
***************************************************************************************************/
__s32 robin_set_palette(const __u32 *palette_buf)
{
    __err("not support anymore after d100");

    return 0;
}



/***************************************************************************************************
*Name        : robin_get_palette
*Prototype   : __s32 robin_get_palette( __u32 *palette_buf )
*Arguments   : palette_buf   output. the buffer to store palette data, whose size must be
*                            ( ROBIN_PALETTE_COLOR_COUNT * 4 ).
*Return      : ==0       succeed
*              !=0       fail
*Description : get the whole palette.
*Other       :
***************************************************************************************************/
__s32 robin_get_palette(__u32 *palette_buf)
{
    __err("not support anymore after d100");
    return 0;
}



/***************************************************************************************************
*Name        : robin_modify_palette
*Prototype   : __s32 robin_modify_palette( __u32 first_index, const __u32 *color_array, __u32 nr )
*Arguments   : first_index   input. the index of the first color to be modified in the palette, it
*                            rangs 0~~~~(ROBIN_PALETTE_COLOR_COUNT-1).
*              color_array   input. the new color list, it is used to replace partial colors in
*                            palette.
*              nr            input. the color count of the color array.
*Return      : ==0       succeed
*              !=0       fail
*Description : modify the partial colors of the palette.
*Other       :
***************************************************************************************************/
__s32 robin_modify_palette(__u32 first_index, const __u32 *color_array, __u32 nr)
{
    __err("not support anymore after d100");
    return 0;
}

#endif     //  ifndef __robin_palette_c
