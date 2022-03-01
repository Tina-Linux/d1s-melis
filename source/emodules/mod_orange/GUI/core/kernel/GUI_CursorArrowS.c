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
* File    : GUI_CursorArrowS.c
* Purpose : Defines the arrow cursor, small
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapArrowS =
{
    9,                  /* XSize */
    15,                  /* YSize */
    3,                   /* BytesPerLine */
    2,                   /* BitsPerPixel */
    GUI_Pixels_ArrowS,   /* Pointer to picture data (indices) */
    &GUI_CursorPal,      /* Pointer to palette */
    NULL
};

GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowS =
{
    &GUI_BitmapArrowS, 0, 0
};

/*************************** End of file ****************************/
