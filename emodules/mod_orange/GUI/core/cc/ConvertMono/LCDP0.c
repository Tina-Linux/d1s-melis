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
*       Public code, 1/2/4/8 bpp color, LUT
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_0
*/
unsigned LCD_Color2Index_0(LCD_COLOR Color, const LCD_PHYSPALETTE *pPhysPal)
{
    int i;
    int NumEntries = pPhysPal->NumEntries;
    int BestIndex = 0;
    U32 BestDiff = 0xffffff; /* Initialize to worst match */
    const LCD_COLOR *pPalEntry;
    /* Try to find perfect match */
    i = 0;
    pPalEntry = &pPhysPal->pPalEntries[0];

    do
    {
        if (Color == *(pPalEntry + i))
        {
            return i;
        }
    } while (++i < NumEntries);

    /* Find best match */
    i = 0;
    pPalEntry = &pPhysPal->pPalEntries[0];

    do
    {
        U32 Diff = GUI_CalcColorDist(Color, *(pPalEntry + i));

        if (Diff < BestDiff)
        {
            BestDiff  = Diff;
            BestIndex = i;
        }
    } while (++i < NumEntries);

    return BestIndex;
}

/*********************************************************************
*
*       LCD_Index2Color_0
*/
LCD_COLOR LCD_Index2Color_0(int Index, const LCD_PHYSPALETTE *pPhysPal)
{
    if ((unsigned)Index >= (unsigned) pPhysPal->NumEntries)
    {
        return 0;     /* Illegal index */
    }

    return *(pPhysPal->pPalEntries + Index);
}

/*********************************************************************
*
*       LCD_GetIndexMask_0
*/
unsigned LCD_GetIndexMask_0(void)
{
    // return ((1 << LCD_BITSPERPIXEL) - 1);
    return ((1 << 24) - 1);    //mcl
}

/*************************** End of file ****************************/
