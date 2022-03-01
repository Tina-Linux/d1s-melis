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
* File    : GUI_CursorArrowSI.c
* Purpose : Defines the arrow cursor, small inverted
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapArrowSI =
{
    9,                  /* XSize */
    15,                  /* YSize */
    3,                   /* BytesPerLine */
    2,                   /* BitsPerPixel */
    GUI_Pixels_ArrowS,   /* Pointer to picture data (indices) */
    &GUI_CursorPalI,     /* Pointer to palette */
    NULL
};

GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowSI =
{
    &GUI_BitmapArrowSI, 0, 0
};

/*************************** End of file ****************************/
