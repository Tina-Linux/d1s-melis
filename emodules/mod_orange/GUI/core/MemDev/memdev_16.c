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
* File    : memdev_16.c
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
*       Macros
*
**********************************************************************
*/
#ifndef PIXELINDEX   //mcl add  2008.12.23
#define PIXELINDEX                      U16
#define BITSPERPIXEL                     16
#define API_LIST      GUI_MEMDEV__APIList16
#endif     // mcl add  2008.12.23
/*********************************************************************
*
*       _DrawBitLine16BPP_DDB
*/
static void _DrawBitLine16BPP_DDB(GUI_USAGE *pUsage, int x, int y, const U16 *pSrc, int xsize, PIXELINDEX *pDest)
{
    switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR))
    {
        case 0:    /* Write mode */
            memcpy(pDest, pSrc, xsize * 2);
            break;

        case LCD_DRAWMODE_TRANS:
            do
            {
                if (*pSrc)
                {
                    *pDest = *pSrc;

                    if (pUsage)
                    {
                        GUI_USAGE_AddPixel(pUsage, x, y);
                    }
                }

                x++;
                pDest++;
                pSrc++;
            } while (--xsize);

            break;
    }
}

/*********************************************************************
*
*       #include GUIDev code
*
**********************************************************************
*
* The main code is in an other "C" file.
* This has been done to avoid duplication of code.
*/
#include "memdev_8.c"

#else

void GUIDEV16_C(void) {}

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/
