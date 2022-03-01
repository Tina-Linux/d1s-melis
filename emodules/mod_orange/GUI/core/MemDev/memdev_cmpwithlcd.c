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
* File    : memdev_cmpwithlcd.c
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
