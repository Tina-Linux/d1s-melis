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
#include "gui_core_cc_private.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define B_BITS 8
#define G_BITS 8
#define R_BITS 8
#define A_BITS 8

#define R_MASK ((1 << R_BITS) -1)
#define G_MASK ((1 << G_BITS) -1)
#define B_MASK ((1 << B_BITS) -1)
#define A_MASK ((1 << A_BITS) -1)
/*********************************************************************
*
*       Public code,
*
*       LCD_FIXEDPALETTE == 888, 16777216 colors, RRRRRRRRGGGGGGGGBBBBBBBB
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_M888
*/
unsigned LCD_Color2Index_M8888(LCD_COLOR Color)
{
    int a, r, g, b;
    r = (Color >> (8  - R_BITS)) & R_MASK;
    g = (Color >> (16 - G_BITS)) & G_MASK;
    b = (Color >> (24 - B_BITS)) & B_MASK;
    a = (Color >> (32 - A_BITS)) & A_MASK;
    return b + (g << B_BITS) + (r << (G_BITS + B_BITS)) + (a << (G_BITS + B_BITS + R_BITS));
}


/*********************************************************************
*
*       LCD_Index2Color_M888
*/
LCD_COLOR LCD_Index2Color_M8888(int Index)
{
    unsigned int a, r, g, b;
    /* Seperate the color masks */
    b = Index                                            & B_MASK;
    g = (Index >> B_BITS)                                & G_MASK;
    r = ((unsigned)Index >> (B_BITS + G_BITS))           & R_MASK;
    a = ((unsigned)Index >> (B_BITS + G_BITS + R_BITS))  & A_MASK;
    /* Convert the color masks */
    r = r * 255 / R_MASK;
    g = g * 255 / G_MASK;
    b = b * 255 / B_MASK;
    a = a * 255 / A_MASK;
    return r + (g << 8) + (((U32)b) << 16) + (((U32)a) << 24);
}
/*********************************************************************
*
*       LCD_GetIndexMask_M565
*/
unsigned LCD_GetIndexMask_M8888(void)
{
    return 0xffffffff;
}

/*************************** End of file ****************************/
