/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Miracle.MCL.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUI_BMP.c
* Purpose : Implementation of GUI_BMP... functions
*
* By      : Miracle.MCL
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include <stdlib.h>
#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetStep
*/
static int _GetStep(int *pYSize, int *pY)
{
    if (*pYSize > 0)
    {
        *pY = *pYSize - 1;
        return -1;
    }
    else if (*pYSize < 0)
    {
        *pYSize = -*pYSize;
        *pY = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

/*********************************************************************
*
*       _DrawBitmap_Pal
*/
static int _DrawBitmap_Pal(const U8 *pData, int x0, int y0, int XSize, int YSize, int Bpp, int NumColors)
{
    int i, y, Step;
    int BytesPerLine = 0;
    Step = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    for (i = 0; i < NumColors; i++)
    {
        U8 r, g, b;
        b = *(pData);
        g = *(pData + 1);
        r = *(pData + 2);
        pData += 4;
        LCD__aConvTable[i] = LCD_Color2Index(((U32)b << 16) | (g << 8) | r);
    }

    switch (Bpp)
    {
        case 1:
            BytesPerLine = ((XSize + 31) >> 5) << 2;
            break;

        case 4:
            BytesPerLine = (((XSize << 2) + 31) >> 5) << 2;
            break;

        case 8:
            BytesPerLine = ((XSize +  3) >> 2) << 2;
            break;
    }

    for (; (y < YSize) && (y >= 0); y += Step)
    {
        LCD_DrawBitmap(x0, y0 + y, XSize, 1, 1, 1, Bpp, XSize, pData, LCD__aConvTable);
        pData += BytesPerLine;
    }

    return 0;
}
/*mcl add for  trans*/
static int _DrawBitmap_Pal_Trans(const U8 *pData, int x0, int y0, int XSize, int YSize, int Bpp, int NumColors, LCD_PIXELINDEX  index)
{
    int i, y, Step;
    int BytesPerLine = 0;
    Step = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    for (i = 0; i < NumColors; i++)
    {
        U8 r, g, b;
        b = *(pData);
        g = *(pData + 1);
        r = *(pData + 2);
        pData += 4;
        LCD__aConvTable[i] = LCD_Color2Index(((U32)b << 16) | (g << 8) | r);
    }

    switch (Bpp)
    {
        case 1:
            BytesPerLine = ((XSize + 31) >> 5) << 2;
            break;

        case 4:
            BytesPerLine = (((XSize << 2) + 31) >> 5) << 2;
            break;

        case 8:
            BytesPerLine = ((XSize +  3) >> 2) << 2;
            break;
    }

    for (; (y < YSize) && (y >= 0); y += Step)
    {
        LCD_DrawBitmap_Trans(x0, y0 + y, XSize, 1,  Bpp, XSize, pData, LCD__aConvTable, index);
        pData += BytesPerLine;
    }

    return 0;
}

/*********************************************************************
*
*       _DrawBitmap_16bpp   // mcl  08.11.27
*/
static int _DrawBitmap_16bpp(const U8 *pData, int x0, int y0, int XSize, int YSize)
{
    int  y, BytesPerLine, Step;
    U16 *pDest = (U16 *)((U16 *)orange.fbaddr[GUI_Context.SelLayer] + y0 * Screen_Context.LCDXSize[GUI_Context.SelLayer] + x0);
    Step = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    if (Step == -1)
    {
        pDest = (U16 *)(pDest + (YSize - 1) * Screen_Context.LCDXSize[GUI_Context.SelLayer]);
    }

    BytesPerLine = ((16 * XSize + 31) >> 5) << 2;

    //BytesPerLine = spcl_size_align(XSize*2, 4 );
    //BytesPerLine = XSize *2;
    for (; (y < YSize) && (y >= 0); y += Step)
    {
        orange_memcpy(pDest, pData, (XSize << 1));
        pData += BytesPerLine;
        pDest = (U16 *)(pDest + (Step) * Screen_Context.LCDXSize[GUI_Context.SelLayer]);
    }

    return 0;
}

/*********************************************************************
*
*       _DrawBitmap_24bpp
*/
static int _DrawBitmap_24bpp(const U8 *pData, int x0, int y0, int XSize, int YSize)
{
    int x, y, BytesPerLine, Step;
    Step = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    BytesPerLine = ((24 * XSize + 31) >> 5) << 2;

    for (; (y < YSize) && (y >= 0); y += Step)
    {
        for (x = 0; x < XSize; x++)
        {
            const U8 *pColor = pData + 3 * x;
            //if ( ((*(U32 *)pColor)&0X00FFFFFF)!=0Xfdfdfd)
            // {
            U8 r, g, b;
            b = *(pColor);
            g = *(pColor + 1);
            r = *(pColor + 2);

            if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_ARGB8888
                || GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_RGB0888)
            {
                // memcpy();
                LCD_SetPixelIndex(x0 + x, y0 + y, LCD_Color2Index(((U32)0xff << 24) | ((U32)b << 16) | (g << 8) | r));
            }
            else
            {
                LCD_SetPixelIndex(x0 + x, y0 + y, LCD_Color2Index(((U32)b << 16) | (g << 8) | r));
            }

            //  }
        }

        pData += BytesPerLine;
    }

    return 0;
}

/*********************************************************************
*
*       _DrawBitmap_32bpp   // mcl
*/
static int _DrawBitmap_32bpp(const U8 *pData, int x0, int y0, int XSize, int YSize)
{
    int x, y, BytesPerLine, Step;
    Step = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    BytesPerLine = ((32 * XSize + 31) >> 5) << 2;

    for (; (y < YSize) && (y >= 0); y += Step)
    {
        for (x = 0; x < XSize; x++)
        {
            const U8 *pColor = pData + 4 * x;
#if 0
            //if ( ((*(U32 *)pColor)&0X00FFFFFF)!=0Xfdfdfd)
            // {
            U8  r, g, b;
            //a = *(pColor + 3);
            b = *(pColor + 0);
            g = *(pColor + 1);
            r = *(pColor + 2);
            LCD_SetPixelIndex(x0 + x, y0 + y, LCD_Color2Index(((U32)0xff << 24) | ((U32)b << 16) | (g << 8) | r));
            //  }
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
*       _DrawBitmap_32bpp_Alpha   // mcl  08.11.27
*/
static int _DrawBitmap_32bpp_Alpha(const U8 *pData, int x0, int y0, int XSize, int YSize)
{
    int x, y, BytesPerLine, Step;
    Step = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    BytesPerLine = ((32 * XSize + 31) >> 5) << 2;

    for (; (y < YSize) && (y >= 0); y += Step)
    {
        for (x = 0; x < XSize; x++)
        {
            const U8 *pColor = pData + 4 * x;
#if 1
            //if ( ((*(U32 *)pColor)&0X00FFFFFF)!=0Xfdfdfd)
            // {
            U8  a, r, g, b;
            a = *(pColor + 3);
            b = *(pColor + 0);
            g = *(pColor + 1);
            r = *(pColor + 2);
            LCD_SetPixelIndex(x0 + x, y0 + y, LCD_Color2Index(((U32)a << 24) | ((U32)b << 16) | (g << 8) | r));
            //  }
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
*       _DrawBitmap_24bpp_Trans               added by MCL
*/
static int _DrawBitmap_24bpp_Trans(const U8 *pData, int x0, int y0, int XSize, int YSize, GUI_COLOR  color)
{
    int x, y, BytesPerLine, Step;
    Step = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    BytesPerLine = ((24 * XSize + 31) >> 5) << 2;

    for (; (y < YSize) && (y >= 0); y += Step)
    {
        for (x = 0; x < XSize; x++)
        {
            const U8 *pColor = pData + 3 * x;

            if (((*(U32 *)pColor) & 0X00FFFFFF) != color)
            {
                U8 r, g, b;
                b = *(pColor);
                g = *(pColor + 1);
                r = *(pColor + 2);
                LCD_SetPixelIndex(x0 + x, y0 + y, LCD_Color2Index(((U32)b << 16) | (g << 8) | r));
            }
        }

        pData += BytesPerLine;
    }

    return 0;
}


/*
**********************************************************************************************************************
*                                               _DrawBitmap_32bpp_Trans
*
* author:
*
* date:             2009-9-21
*
* Description:       drawbitmap 32bpp trans
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static int _DrawBitmap_32bpp_Trans(const U8 *pData, int x0, int y0, int XSize, int YSize, GUI_COLOR  color)
{
    int x, y, BytesPerLine, Step;
    Step = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    BytesPerLine = (XSize << 2);

    for (; (y < YSize) && (y >= 0); y += Step)
    {
        for (x = 0; x < XSize; x++)
        {
            const U8 *pColor = pData + 4 * x;

            if (((*(U32 *)pColor) & 0X00FFFFFF) != color)
            {
#if 0
                U8 a, r, g, b;
                b = *(pColor);
                g = *(pColor + 1);
                r = *(pColor + 2);
                a = *(pColor + 3);
                LCD_SetPixelIndex(x0 + x, y0 + y, LCD_Color2Index(((U32)a << 24) | ((U32)b << 16) | (g << 8) | r));
#else
                U32 index32 = *((U32 *) pColor);
                LCD_SetPixelIndex(x0 + x, y0 + y, index32);
#endif
            }
        }

        pData += BytesPerLine;
    }

    return 0;
}




/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_BMP_GetXSize
*/
int GUI_BMP_GetXSize(const void *pBMP)
{
    const U8 *pSrc = (const U8 *)pBMP;

    if (!pBMP)
    {
        return 0;
    }

    pSrc += 18;                   /* skip rest of BITMAPFILEHEADER */
    return GUI__Read32(&pSrc);
}

/*********************************************************************
*
*       GUI_BMP_GetYSize
*/
int GUI_BMP_GetYSize(const void *pBMP)
{
    const U8 *pSrc = (const U8 *)pBMP;

    if (!pBMP)
    {
        return 0;
    }

    pSrc += 22;
    return labs((int)GUI__Read32(&pSrc));
}

static int BMP_DrawARGB(unsigned long *pData, int x0, int y0, int XSize, int YSize,
                        unsigned long *pAddr, int dstlayer_w, int dstlayer_h)
{
    int         x, y, PixelPerLine;
    unsigned long       l_startaddr = 0;                    /*行起始地址*/
    unsigned long       col_addr = 0;                           /*列写入地址*/
    __u32       dir;
    __u8        *pColor;
    unsigned long       *addr;
    __u32       line_stride = 0;
    __u8        a, r, g, b;
    int         Step;
    Step = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    dir  = GUI_GetScnDir();
    PixelPerLine = (32 * XSize + 31) >> 5;

    if (dir == GUI_SCNDIR_ROTATE90 || dir == GUI_SCNDIR_ROTATE270)
    {
        line_stride  = dstlayer_h << 2;
    }
    else
    {
        line_stride  = dstlayer_w << 2;
    }

    for (y = 0; y < YSize; y++)
    {
        /*方向*/
        if (dir == GUI_SCNDIR_ROTATE90)
        {
            if (y == 0)
            {
                l_startaddr = (unsigned long)(pAddr + (dstlayer_w - 1 - x0) * dstlayer_h + (y0));
            }
            else
            {
                l_startaddr += 4;
            }
        }
        else if (dir == GUI_SCNDIR_ROTATE180)
        {
            if (y == 0)
            {
                l_startaddr = (unsigned long)(pAddr + (dstlayer_h - 1 - y0) * dstlayer_w  + (dstlayer_w - 1 - x0));
            }
            else
            {
                l_startaddr -= line_stride;
            }
        }
        else if (dir == GUI_SCNDIR_ROTATE270)
        {
            if (y == 0)
            {
                l_startaddr = (unsigned long)(pAddr + x0 * dstlayer_h  + (dstlayer_h - 1 - y0));
            }
            else
            {
                l_startaddr -= 4;
            }
        }
        else
        {
            if (y == 0)
            {
                l_startaddr  = (unsigned long)(pAddr  + y0 * dstlayer_w + x0);
            }
            else
            {
                l_startaddr  += line_stride;
            }
        }

        for (x = 0; x < XSize; x++)
        {
#if 0
            pColor = (__u8 *)(pData + x);
            a = *(pColor + 3);
            r = *(pColor + 2);
            g = *(pColor + 1);
            b = *(pColor + 0);
#endif

            if (x == 0)
            {
                col_addr = l_startaddr;
            }
            else
            {
                if (dir == GUI_SCNDIR_ROTATE90)
                {
                    col_addr = col_addr - line_stride;
                }
                else if (dir == GUI_SCNDIR_ROTATE180)
                {
                    col_addr = col_addr - 4;
                }
                else if (dir == GUI_SCNDIR_ROTATE270)
                {
                    col_addr = col_addr + line_stride;
                }
                else
                {
                    col_addr = col_addr + 4;
                }
            }

            addr  = (unsigned long *)col_addr;
#if 0
            *addr = (((__u32)a << 24) | ((__u32)r << 16) | (g << 8) | b);
#else
            *addr = *((unsigned int *)(pData + x * 4));
#endif
        }

        pData += PixelPerLine;
    }

    return 0;
}

int GUI_BMP_Draw_Rotate(const U8 *pData, int x0, int y0, int XSize, int YSize)
{
    int Ret;
    YSize = -YSize;
    Ret = _DrawBitmap_32bpp(pData, x0, y0, XSize, YSize);
    return Ret;
}

/*********************************************************************
*
*       GUI_BMP_Draw
*/
int GUI_BMP_Draw(const void *pBMP, int x0, int y0)
{
    int Ret = 0;
    I32 Width, Height;
    U16 BitCount, Type;
    U32 ClrUsed, Compression;
    int NumColors;
    U32 alphaEn;
    const U8 *pSrc = (const U8 *)pBMP;

    if (pSrc == NULL)
    {
        return 1;
    }

    /*__log("rtn0 = %p.", __builtin_return_address(0));*/
    /*__log("rtn1 = %p.", __builtin_return_address(1));*/
    /*__log("rtn2 = %p.", __builtin_return_address(2));*/
    /*__log("rtn3 = %p.", __builtin_return_address(3));*/
    /*__log("rtn4 = %p.", __builtin_return_address(4));*/
    /*__log("rtn5 = %p.", __builtin_return_address(5));*/
    /*__log("ptr = %p.", pBMP);*/
    Type        = GUI__Read16(&pSrc); /* get type from BITMAPFILEHEADER */
    pSrc        += 4;
    alphaEn     = GUI__Read32(&pSrc);
    pSrc        += 4;                   /* skip rest of BITMAPFILEHEADER */
    /* get values from BITMAPINFOHEADER */
    pSrc        += 4;
    Width       = GUI__Read32(&pSrc);
    Height      = GUI__Read32(&pSrc);
    pSrc        += 2;
    BitCount    = GUI__Read16(&pSrc);
    Compression = GUI__Read32(&pSrc);
    pSrc        += 12;
    ClrUsed     = GUI__Read32(&pSrc);
    pSrc        += 4;

    /* calculate number of colors */
    switch (BitCount)
    {
        case 0:
            return 1; /* biBitCount = 0 (JPEG format) not supported. Please convert image ! */

        case 1:
            NumColors = 2;
            break;

        case 4:
            NumColors = 16;
            break;

        case 8:
            NumColors = 256;
            break;

        case 16:
            NumColors = 0;
            break;

        case 24:
            NumColors = 0;
            break;

        case 32:
            NumColors = 0;
            break;

        default:
            return 1; /* biBitCount should be 1, 4, 8 or 24 */
    }

    if (NumColors && ClrUsed)
    {
        NumColors = ClrUsed;
    }

    /* check validity of bmp */
    if ((NumColors > LCD_MAX_LOG_COLORS) ||
        (Type != 0x4d42)                 || /* 'BM' */
        (Compression)                    || /* only uncompressed bitmaps */
        (Width  > 1920)                  ||
        (ABS(Height) > 1080))
    {
        return 1;
    }

    /* start output */
    GUI_LOCK();

    /* Show bitmap */
    switch (BitCount)
    {
        case 1:
        case 4:
        case 8:
            Ret = _DrawBitmap_Pal(pSrc, x0, y0, Width, Height, BitCount, NumColors);
            break;

        case 16:
            pSrc += 16;

            if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_RGB565)
            {
                _DrawBitmap_16bpp(pSrc, x0, y0, Width, Height);
            }
            else
            {
                Ret = 1;
                __msg("ERROR: 16bpp BMP can only be drawn in RGB565 color mode layer");
            }

            break;

        case 24:
            Ret = _DrawBitmap_24bpp(pSrc, x0, y0, Width, Height);  //modified by MCL
            break;

        case 32:
            if (alphaEn == BMP_ALPHA_ENABLE)
            {
                Ret = _DrawBitmap_32bpp_Alpha(pSrc, x0, y0, Width, Height);    // mcl add
            }
            else
            {            
                Ret = _DrawBitmap_32bpp(pSrc, x0, y0, Width, Height);    // mcl add
            }

            break;
    }

    GUI_UNLOCK();
    return Ret;
}

int GUI_BMP_DrawEx(const void *pBMP, int x0, int y0)
{
    int Ret = 0;
    I32 Width, Height;
    U16 BitCount, Type;
    U32 ClrUsed, Compression;
    int NumColors;
    U32 alphaEn;
    const U8 *pSrc = (const U8 *)pBMP;

    if (pSrc == NULL)
    {
        return 1;
    }

    Type        = GUI__Read16(&pSrc); /* get type from BITMAPFILEHEADER */
    pSrc        += 4;
    alphaEn     = GUI__Read32(&pSrc);
    pSrc        += 4;                   /* skip rest of BITMAPFILEHEADER */
    /* get values from BITMAPINFOHEADER */
    pSrc        += 4;
    Width       = GUI__Read32(&pSrc);
    Height      = GUI__Read32(&pSrc);
    pSrc        += 2;
    BitCount    = GUI__Read16(&pSrc);
    Compression = GUI__Read32(&pSrc);
    pSrc        += 12;
    ClrUsed     = GUI__Read32(&pSrc);
    pSrc        += 4;

    /* calculate number of colors */
    switch (BitCount)
    {
        case 0:
            return 1; /* biBitCount = 0 (JPEG format) not supported. Please convert image ! */

        case 1:
            NumColors = 2;
            break;

        case 4:
            NumColors = 16;
            break;

        case 8:
            NumColors = 256;
            break;

        case 16:
            NumColors = 0;
            break;

        case 24:
            NumColors = 0;
            break;

        case 32:
            NumColors = 0;
            break;

        default:
            return 1; /* biBitCount should be 1, 4, 8 or 24 */
    }

    if (NumColors && ClrUsed)
    {
        NumColors = ClrUsed;
    }

    /* check validity of bmp */
    if ((NumColors > LCD_MAX_LOG_COLORS) ||
        (Type != 0x4d42)                 || /* 'BM' */
        (Compression)                    || /* only uncompressed bitmaps */
        (Width  > 1920)                  ||
        (ABS(Height) > 1024))
    {
        return 1;
    }

    /* start output */
    GUI_LOCK();

    /* Show bitmap */
    switch (BitCount)
    {
        case 1:
        case 4:
        case 8:
            Ret = _DrawBitmap_Pal(pSrc, x0, y0, Width, Height, BitCount, NumColors);
            break;

        case 16:
            pSrc += 16;

            if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_RGB565)
            {
                _DrawBitmap_16bpp(pSrc, x0, y0, Width, Height);
            }
            else
            {
                Ret = 1;
                __msg("ERROR: 16bpp BMP can only be drawn in RGB565 color mode layer");
            }

            break;

        case 24:
            Ret = _DrawBitmap_24bpp(pSrc, x0, y0, Width, Height);  //modified by MCL
            break;

        case 32:
            if (alphaEn == BMP_ALPHA_ENABLE)
            {
                Ret = BMP_DrawARGB((unsigned long *)pSrc, x0, y0, Width, Height,
                                   (unsigned long *)orange.fbaddr[GUI_Context.SelLayer],
                                   Screen_Context.LCDXSize[GUI_Context.SelLayer],
                                   Screen_Context.LCDYSize[GUI_Context.SelLayer]);
                //Ret = _DrawBitmap_32bpp_Alpha(pSrc, x0, y0, Width, Height);    // mcl add
            }
            else
            {
                Ret = _DrawBitmap_32bpp(pSrc, x0, y0, Width, Height);    // mcl add
            }

            break;
    }

    GUI_UNLOCK();
    return Ret;
}

/*********************************************************************
*
*       GUI_BMP_Draw_Trans         added by MCL for Trans
*/
int GUI_BMP_Draw_Trans(const void *pBMP, int x0, int y0, GUI_COLOR  color, U8 index)
{
    int Ret = 0;
    I32 Width, Height;
    U16 BitCount, Type;
    U32 ClrUsed, Compression;
    int NumColors;
    const U8 *pSrc = (const U8 *)pBMP;
    Type            = GUI__Read16(&pSrc); /* get type from BITMAPFILEHEADER */
    pSrc            += 12;                   /* skip rest of BITMAPFILEHEADER */
    /* get values from BITMAPINFOHEADER */
    pSrc            += 4;
    Width           = GUI__Read32(&pSrc);
    Height          = GUI__Read32(&pSrc);
    pSrc            += 2;
    BitCount        = GUI__Read16(&pSrc);
    Compression     = GUI__Read32(&pSrc);
    pSrc            += 12;
    ClrUsed         = GUI__Read32(&pSrc);
    pSrc            += 4;

    /* calculate number of colors */
    switch (BitCount)
    {
        case 0:
            return 1; /* biBitCount = 0 (JPEG format) not supported. Please convert image ! */

        case 1:
            NumColors = 2;
            break;

        case 4:
            NumColors = 16;
            break;

        case 8:
            NumColors = 256;
            break;

        case 24:
            NumColors = 0;
            break;

        case 32:
            NumColors = 0;
            break;

        default:
            return 1; /* biBitCount should be 1, 4, 8 or 24 */
    }

    if (NumColors && ClrUsed)
    {
        NumColors = ClrUsed;
    }

    /* check validity of bmp */
    if ((NumColors > LCD_MAX_LOG_COLORS) ||
        (Type != 0x4d42)                 || /* 'BM' */
        (Compression)                    || /* only uncompressed bitmaps */
        (Width  > 1024)                  ||
        (ABS(Height) > 1024))
    {
        return 1;
    }

    /* start output */
    GUI_LOCK();

    /* Show bitmap */
    switch (BitCount)
    {
        case 1:
        case 4:
        case 8:
            Ret = _DrawBitmap_Pal_Trans(pSrc, x0, y0, Width, Height, BitCount, NumColors, index); //modified by MCL
            break;

        case 24:
            Ret = _DrawBitmap_24bpp_Trans(pSrc, x0, y0, Width, Height, color); //modified by MCL
            break;

        case 32:
            Ret = _DrawBitmap_32bpp_Trans(pSrc, x0, y0, Width, Height, color); //modified by MCL
            break;
    }

    GUI_UNLOCK();
    return Ret;
}

/*************************** End of file ****************************/
