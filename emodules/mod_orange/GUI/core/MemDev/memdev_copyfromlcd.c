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
* File    : memdev_copyfromlcd.c
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
*       defines
*
**********************************************************************
*/

#define LCD_LIMIT(Var, Op, Limit) if (Var Op Limit) Var = Limit

/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV_CopyFromLCD
*/
void GUI_MEMDEV_CopyFromLCD(GUI_MEMDEV_Handle hMem)
{
    /* Make sure memory handle is valid */
    if (!hMem)
    {
        hMem = GUI_Context.hDevData;
    }

    if (!hMem)
    {
        return;
    }

    {
        GUI_MEMDEV *pDevData = (GUI_MEMDEV *) GUI_ALLOC_h2p(hMem); /* Convert to pointer */
        LCD_RECT r;
        int y;
        int XMax;
        GUI_USAGE *pUsage = 0;
        GUI_MEMDEV_Handle hMemOld = GUI_Context.hDevData;
        GUI_MEMDEV_Select(hMem);

        if (pDevData->hUsage)
        {
            pUsage = GUI_USAGE_H2P(pDevData->hUsage);
        }

        /* Get bounding rectangle */
        r.y0  = pDevData->y0;
        r.x0  = pDevData->x0;
        r.x1  = pDevData->x0 + pDevData->XSize - 1;
        r.y1  = pDevData->y0 + pDevData->YSize - 1;
        /* Make sure bounds are within LCD area so we can call driver directly */
        LCD_LIMIT(r.x0, <, 0);
        LCD_LIMIT(r.y0, <, 0);
        LCD_LIMIT(r.x1, >, LCD_GET_XSIZE() - 1);
        LCD_LIMIT(r.y1, >, LCD_GET_YSIZE() - 1);
        XMax = r.x1;

        for (y = r.y0; y <= r.y1; y++)
        {
            int x = r.x0;
            LCD_PIXELINDEX *pData = (LCD_PIXELINDEX *)GUI_MEMDEV__XY2PTR(x, y);

            if (pUsage)
            {
                GUI_USAGE_AddHLine(pUsage, x, y, r.x1 - r.x0 + 1);
            }

            for (; x <= XMax; x++)
            {
                *pData++ = LCD_L0_GetPixelIndex(x, y);
            }
        }

        GUI_MEMDEV_Select(hMemOld);
    }
}

#endif /* GUI_MEMDEV_SUPPORT */

/*************************** end of file ****************************/
