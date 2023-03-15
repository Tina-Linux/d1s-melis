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
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       defines
*
**********************************************************************
*/

#if 1  /* Normaly calculate square values */

#define  SQUARE(Dist) ((U16)Dist) * ((U16)Dist)

#else

#define S(x) ((x)*(x))
#define SQUARES(Base)  S(Base+0),  S(Base+1),  S(Base+2),  S(Base+3), S(Base+4),  S(Base+5),  \
    S(Base+6),  S(Base+7),  S(Base+8),  S(Base+9), S(Base+10), S(Base+11), \
    S(Base+12), S(Base+13), S(Base+14), S(Base+15)

static const U16 aSquare[] =
{
    SQUARES(0 * 16)
    , SQUARES(1 * 16), SQUARES(2 * 16), SQUARES(3 * 16)
    , SQUARES(4 * 16), SQUARES(5 * 16), SQUARES(6 * 16), SQUARES(7 * 16)
    , SQUARES(8 * 16), SQUARES(9 * 16), SQUARES(10 * 16), SQUARES(11 * 16)
    , SQUARES(12 * 16), SQUARES(13 * 16), SQUARES(14 * 16), SQUARES(15 * 16)
};

#define  SQUARE(Dist) aSquare[Dist]

#endif

/*********************************************************************
*
*       public code, color conversions for palette based displays
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_CalcColorDist
*/
U32 GUI_CalcColorDist(LCD_COLOR PalColor, LCD_COLOR  Color)
{
    /* This routine does not use abs() because we are optimizing for speed ! */
    I16 Dist;
    U32 Sum;
    Dist  = (PalColor & 0xff) - (Color & 0xff);

    if (Dist < 0)
    {
        Dist = -Dist;
    }

    Sum = SQUARE(Dist);
    Dist  = ((PalColor >> 8) & 0xff) - ((Color >> 8) & 0xff);

    if (Dist < 0)
    {
        Dist = -Dist;
    }

    Sum += SQUARE(Dist);
    Dist  = (PalColor >> 16) - (Color >> 16);

    if (Dist < 0)
    {
        Dist = -Dist;
    }

    return Sum + SQUARE(Dist);
}

/*************************** End of file ****************************/
