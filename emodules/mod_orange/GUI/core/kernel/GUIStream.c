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
* File    : GUIStream.c
* Purpose : Support for streamed bitmaps
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DrawStreamedBitmap
*/
void GUI_DrawStreamedBitmap(const GUI_BITMAP_STREAM *pBitmapHeader, int x, int y)
{
    GUI_BITMAP Bitmap = {0};
    GUI_LOGPALETTE Palette;
    Bitmap.BitsPerPixel = pBitmapHeader->BitsPerPixel;
    Bitmap.BytesPerLine = pBitmapHeader->BytesPerLine;
    Bitmap.pData        = ((const U8 *)pBitmapHeader) + 16 + 4 * pBitmapHeader->NumColors;
    Bitmap.pPal         = &Palette;
    Bitmap.XSize        = pBitmapHeader->XSize;
    Bitmap.YSize        = pBitmapHeader->YSize;
    Palette.HasTrans    = pBitmapHeader->HasTrans;
    Palette.NumEntries  = pBitmapHeader->NumColors;
    Palette.pPalEntries = (const LCD_COLOR *)((const U8 *)pBitmapHeader + 16);
    GUI_DrawBitmap(&Bitmap, x, y);
}

/*************************** End of file ****************************/
