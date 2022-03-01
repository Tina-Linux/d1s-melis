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
* File    : GUI_CursorArrowMI.c
* Purpose : Defines the arrow cursor, medium inverted
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapArrowMI =
{
    12,                  /* XSize */
    20,                  /* YSize */
    3,                   /* BytesPerLine */
    2,                   /* BitsPerPixel */
    GUI_Pixels_ArrowM,   /* Pointer to picture data (indices) */
    &GUI_CursorPalI,      /* Pointer to palette */
    NULL
};

GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowMI =
{
    &GUI_BitmapArrowMI, 0, 0
};

/*************************** End of file ****************************/
