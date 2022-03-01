/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009,  Miracle.MCL.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUI_CursorArrowLI.c
* Purpose : Defines the arrow cursor, large inverted
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapArrowLI =
{
    18,                  /* XSize */
    30,                  /* YSize */
    5,                   /* BytesPerLine */
    2,                   /* BitsPerPixel */
    GUI_Pixels_ArrowL,   /* Pointer to picture data (indices) */
    &GUI_CursorPalI,     /* Pointer to palette */
    NULL
};

GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowLI =
{
    &GUI_BitmapArrowLI, 0, 0
};

/*************************** End of file ****************************/
