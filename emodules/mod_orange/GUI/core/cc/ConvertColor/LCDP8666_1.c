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
*       Public code,
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_8666_1
*/
unsigned LCD_Color2Index_8666_1(LCD_COLOR Color)
{
    int r, g, b, Index;
    r = Color & 255;
    g = (Color >> 8) & 255;
    b = Color >> 16;

    /* Check if image is a gray scale ... */
    if ((r == g) && (g == b))
    {
        return 120 + (r + 8) / 17;  /* Convert into colors from 120 - 135 */
    }

    /* Convert into the 6*6*6 colors ... */
    r = (r * 5 + 127) / 255;
    g = (g * 5 + 127) / 255;
    b = (b * 5 + 127) / 255;
    Index = r + 6 * g + 36 * b;
    return (Index < 108) ? Index + 12 : Index + 28;
}

/*********************************************************************
*
*       LCD_Index2Color_8666_1
*/
LCD_COLOR LCD_Index2Color_8666_1(int Index)
{
    unsigned int r, g;
    U32 b;

    /* 16 Gray scale range ? */
    if ((Index >= 120) && (Index < 136))
    {
        return (U32)0x111111 * (U32)(Index - 120);
    }

    if ((Index < 12) || (Index > 243))
    {
        return 0;  /* Black for illegal indices */
    }

    if (Index >= 120)
    {
        Index -= 28;
    }
    else
    {
        Index -= 12;
    }

    r = (Index % 6) * (255 / 5);
    g = ((Index / 6) % 6) * (255 / 5);
    b = (Index / 36) * (255 / 5);
    return r + (g << 8) + ((U32)b << 16);
}

/*********************************************************************
*
*       LCD_GetIndexMask_8666_1
*/
unsigned LCD_GetIndexMask_8666_1(void)
{
    return 0xff;
}

/*************************** End of file ****************************/
