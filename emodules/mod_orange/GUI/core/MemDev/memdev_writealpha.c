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
*       Static routines
*
**********************************************************************
*/
/*********************************************************************
*
*       _WriteAlphaToActiveAt
*/
static void _WriteAlphaToActiveAt(GUI_MEMDEV_Handle hMem, int Intens, int x, int y)
{
    /* Make sure the memory handle is valid */
    if (hMem)
    {
        GUI_MEMDEV *pDev = GUI_MEMDEV_H2P(hMem);
        GUI_USAGE_h hUsage = pDev->hUsage;
        GUI_USAGE  *pUsage;
        int YSize = pDev->YSize;
        int yi;

        if (hUsage)
        {
            pUsage = GUI_USAGE_H2P(hUsage);

            for (yi = 0; yi < YSize; yi++)
            {
                int xOff = 0;
                int XSize;
                XSize = GUI_USAGE_GetNextDirty(pUsage, &xOff, yi);

                /* Draw the partial line which needs to be drawn */
                for (; XSize;)
                {
                    U8 *pData;
                    pData = (U8 *)GUI_MEMDEV__XY2PTREx(pDev, xOff, yi);

                    do
                    {
                        LCD_COLOR Color, BkColor;
                        int xPos, yPos, Index;

                        if (pDev->BitsPerPixel == 8)
                        {
                            Index = *pData++;
                        }
                        else
                        {
                            Index = *(U16 *)pData;
                            pData += 2;
                        }

                        Color   = LCD_Index2Color(Index);
                        xPos    = xOff + x;
                        yPos    = yi + y;
                        BkColor = LCD_GetPixelColor(xPos, yPos);
                        Color   = LCD_MixColors256(Color, BkColor, Intens);
                        Index   = LCD_Color2Index(Color);
                        LCD_SetPixelIndex(xPos, yPos, Index);
                        xOff++;
                    } while (--XSize);

                    XSize = GUI_USAGE_GetNextDirty(pUsage, &xOff, yi);
                }
            }
        }
    }
}

/*********************************************************************
*
*       Exported routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV_WriteAlphaAt
*/
void GUI_MEMDEV_WriteAlphaAt(GUI_MEMDEV_Handle hMem, int Alpha, int x, int y)
{
    if (hMem)
    {
        GUI_MEMDEV *pDevData;
#if (GUI_WINSUPPORT)
        GUI_RECT r;
#endif
        GUI_LOCK();
        pDevData = (GUI_MEMDEV *) GUI_ALLOC_h2p(hMem); /* Convert to pointer */

        if (x == GUI_POS_AUTO)
        {
            x = pDevData->x0;
            y = pDevData->y0;
        }

#if (GUI_WINSUPPORT)
        r.x1 = (r.x0 = x) + pDevData->XSize - 1;
        r.y1 = (r.y0 = y) + pDevData->YSize - 1;;
        WM_ITERATE_START(&r)
        {
            _WriteAlphaToActiveAt(hMem, Alpha, x, y);
        }
        WM_ITERATE_END();
#else
        _WriteAlphaToActiveAt(hMem, Alpha, x, y);
#endif
        GUI_UNLOCK();
    }
}

/*********************************************************************
*
*       GUI_MEMDEV_WriteAlpha
*/
void GUI_MEMDEV_WriteAlpha(GUI_MEMDEV_Handle hMem, int Alpha)
{
    GUI_MEMDEV_WriteAlphaAt(hMem, Alpha, GUI_POS_AUTO, GUI_POS_AUTO);
}

#else

void GUIDEV_WriteAlpha_C(void) {}

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/
