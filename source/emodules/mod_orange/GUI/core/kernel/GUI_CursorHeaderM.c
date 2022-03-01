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
* File    : GUI_CursorHeaderM.c
* Purpose : Defines the header cursor, medium size
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       static data, cursor bitmap
*
**********************************************************************
*/

static GUI_CONST_STORAGE GUI_BITMAP _BitmapHeaderM =
{
    17,                  /* XSize */
    17,                  /* YSize */
    5,                   /* BytesPerLine */
    2,                   /* BitsPerPixel */
    GUI_PixelsHeaderM,      /* Pointer to picture data (indices) */
    &GUI_CursorPal,       /* Pointer to palette */
    NULL
};


/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
GUI_CONST_STORAGE GUI_CURSOR GUI_CursorHeaderM =
{
    &_BitmapHeaderM, 8, 8
};

/*************************** End of file ****************************/
