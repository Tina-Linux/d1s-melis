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
#include "gui_core_memdev_private.h"


#if GUI_SUPPORT_MEMDEV

/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV_CompareWithLCD
*/
int GUI_MEMDEV_CompareWithLCD(GUI_MEMDEV_Handle hMem, int *px, int *py, int *pExp, int *pAct)
{
    int Ret = 0;

    /* Make sure memory handle is valid */
    if (!hMem)
    {
        hMem = GUI_Context.hDevData;
    }

    if (!hMem)
    {
        return 1;
    }

    GUI_LOCK();
    {
        GUI_MEMDEV *pDevData = (GUI_MEMDEV *) GUI_ALLOC_h2p(hMem);  /* Convert to pointer */
        int y    = pDevData->y0;
        int x0   = pDevData->x0;
        int XMax = pDevData->XSize + x0;
        int YMax = pDevData->YSize + y;
        int Max;
        int MagX = LCD_GetXMag();
        int MagY = LCD_GetYMag();

        /* Calculate limits */
        if (y < 0)
        {
            y = 0;
        }

        if (x0 < 0)
        {
            x0 = 0;
        }

        Max = LCD_GET_YSIZE();

        if (YMax > Max)
        {
            YMax = Max;
        }

        Max = LCD_GET_XSIZE();

        if (XMax > Max)
        {
            XMax = Max;
        }

        for (; y < YMax; y++)
        {
            int x = x0;
            U8 *pData = (U8 *)GUI_MEMDEV__XY2PTR(x, y);

            for (x = x0; x < XMax; x++)
            {
                int Exp = *pData++;
                int Act = LCD_L0_GetPixelIndex(x * MagX, y * MagY);
#if (GUI_NUM_LAYERS > 1)

                if (LCD_GetBitsPerPixelEx(GUI_Context.SelLayer) > 8)
#else
                if (LCD_GetBitsPerPixel() > 8)
#endif
                {
                    Exp |= (*pData++) << 8;
                }

                if (Act != Exp)
                {
                    *px    = x;
                    *py    = y;
                    *pAct  = Act;
                    *pExp  = Exp;
                    Ret = 1;
                    goto Skip;
                }
            }
        }
    }
Skip:
    GUI_UNLOCK();
    return Ret;
}

#else

void GUIDEV_CmpWithLCD(void) {} /* avoid empty object files */

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/
