/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : memdev_writealpha.c
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-1-6 8:41:44
**********************************************************************************************************************
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
