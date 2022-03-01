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
* File    : GUI_BitString_Draw.c
* Purpose : Implementation of GUI_BitString_Draw... functions
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include <string.h>
#include "gui_core_kernel_private.h"

//#define ABS(x)                          ( (x) >0 ? (x) : -(x) )


/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _DrawBitmap_32bpp_Alpha   // mcl  08.11.27
*/
static int _DrawBitmap_32bpp_Alpha(const U8 *pData, int x0, int y0, int XSize, int YSize)
{
    int  y, BytesPerLine;
    int LCDXSize = 0;
    U8 *pDest = NULL;

    if ((Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE90)
        || (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE270))
    {
        LCDXSize = Screen_Context.LCDYSize[GUI_Context.SelLayer];
    }
    else
    {
        LCDXSize = Screen_Context.LCDXSize[GUI_Context.SelLayer];
    }

    pDest = (unsigned char *)(orange.fbaddr[GUI_Context.SelLayer]) + (y0 * LCDXSize << 2) + (x0 << 2);
    BytesPerLine = ((32 * XSize + 31) >> 5) << 2;

    for (y = 0; y < YSize; y++)
    {
        memcpy(pDest, pData, (XSize << 2));
        pData += BytesPerLine;
        pDest = (U8 *)(pDest + (LCDXSize << 2));
    }

    return 0;
}



/*********************************************************************
*
*       _Draw_FB_ARGB   // mcl
*/
static int _Draw_FB_ARGB(const U8 *pData, int x0, int y0, int XSize, int YSize)
{
    int x, y, BytesPerLine;
    BytesPerLine = ((32 * XSize + 31) >> 5) << 2;

    for (y = 0; y < YSize; y++)
    {
        for (x = 0; x < XSize; x++)
        {
            const U8 *pColor = pData + 4 * x;
#if 0
            U8 a, r, g, b;
            a = *(pColor + 3);
            r = *(pColor + 2);
            g = *(pColor + 1);
            b = *(pColor + 0);
            LCD_SetPixelIndex(x0 + x, y0 + y, LCD_Color2Index(((U32)a << 24) | ((U32)b << 16) | (g << 8) | r));
#else
            U32 index32 = *((U32 *) pColor);
            LCD_SetPixelIndex(x0 + x, y0 + y, index32);
#endif
        }

        pData += BytesPerLine;
    }

    return 0;
}


/*********************************************************************
*
*      _Draw_FB_BGRA  // mcl
*/
static int _Draw_FB_BGRA(const U8 *pData, int x0, int y0, int XSize, int YSize)
{
    int x, y, BytesPerLine;
    BytesPerLine = ((32 * XSize + 31) >> 5) << 2;

    for (y = 0; y < YSize; y++)
    {
        for (x = 0; x < XSize; x++)
        {
            const U8 *pColor = pData + 4 * x;
#if 0
            U8 a, r, g, b;
            a = *(pColor + 0);
            r = *(pColor + 1);
            g = *(pColor + 2);
            b = *(pColor + 3);
            LCD_SetPixelIndex(x0 + x, y0 + y, LCD_Color2Index(((U32)a << 24) | ((U32)b << 16) | (g << 8) | r));
#else
            U32 index32 = *((U32 *) pColor);
            LCD_SetPixelIndex(x0 + x, y0 + y, index32);
#endif
        }

        pData += BytesPerLine;
    }

    return 0;
}



/*********************************************************************
*
*       GUI_BitString_Draw
*/
int GUI_BitString_Draw(FB *fb, int x0, int y0)
{
    int Ret = 0;
    GUI_LOCK();

    if (!fb->fmt.type)
    {
        if (fb->fmt.fmt.rgb.pixseq == 0)
        {
            Ret = _Draw_FB_ARGB((const U8 *)fb->addr[0], x0, y0, fb->size.width, fb->size.height);    // mcl add
        }
        else
        {
            Ret = _Draw_FB_BGRA((const U8 *)fb->addr[0], x0, y0, fb->size.width, fb->size.height);
        }
    }

    GUI_UNLOCK();
    return Ret;
}

int GUI_BitString_DrawEx(FB *fb, int x0, int y0)
{
    U8 *pSrcTemp   = NULL;
    int Height      = fb->size.height;
    int Width       = fb->size.width;
    int Ret         = 0;
    const U8 *pSrc = (const U8 *)fb->addr[0];
    GUI_LOCK();

    if (Screen_Context.LCDDirection[GUI_Context.SelLayer])
    {
        GUI_BitString_Draw(fb, x0, y0);
    }
    else
    {
        if (!fb->fmt.type)
        {
            if (fb->fmt.fmt.rgb.pixseq == 0)
            {
                Ret = _DrawBitmap_32bpp_Alpha(pSrc, x0, y0, Width, Height);    // mcl add
            }
            else
            {
                Ret = _DrawBitmap_32bpp_Alpha(pSrc, x0, y0, Width, Height);
            }
        }
    }

    GUI_UNLOCK();
    return  Ret;
}



/*************************** End of file ****************************/
