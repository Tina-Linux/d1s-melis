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
* File    : LCD_API.c
* Purpose : Selection of LCD display
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
**********************************************************************************************************************
*/


#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"
#include <kconfig.h>

#if GUI_COMPILER_SUPPORTS_FP

/*********************************************************************
*
*             LCD Device
*       (if memory devices are supported)
*
**********************************************************************
*/

/*RGB 565*/
const tLCDDEV_APIList LCD_L0_APIList =
{
#if LCD_YMAG==1
#if LCD_DELTA_MODE
xx
    LCD_L0_Color2Index,
    LCD_L0_Index2Color,
    LCD_L0_GetIndexMask,
    LCD_L0_DELTA_DrawBitmap,
    LCD_L0_DELTA_DrawHLine,
    LCD_L0_DELTA_DrawVLine,
    LCD_L0_DELTA_FillRect,
    LCD_L0_DELTA_GetPixelIndex,
    LCD_L0_GetRect,          /* Original routine is o.k. here, since we use the logical
                                   coordinates for clipping, which are not magnified */
    LCD_L0_DELTA_SetPixelIndex,
    LCD_L0_DELTA_XorPixel,
    LCD_L0_SetLUTEntry,
#else
    LCD_L0_Color2Index,
    LCD_L0_Index2Color,
    LCD_L0_GetIndexMask,
    (tLCDDEV_DrawBitmap *)LCD_L0_DrawBitmap,
    LCD_L0_DrawHLine,
    LCD_L0_DrawVLine,
    LCD_L0_FillRect,
    LCD_L0_GetPixelIndex,
    LCD_L0_GetRect,
    LCD_L0_SetPixelIndex,
    LCD_L0_XorPixel,
    LCD_L0_SetLUTEntry,
#endif
#else
    LCD_L0_Color2Index,
    LCD_L0_Index2Color,
    LCD_L0_GetIndexMask,
    LCD_L0_MAG_DrawBitmap,
    LCD_L0_MAG_DrawHLine,
    LCD_L0_MAG_DrawVLine,
    LCD_L0_MAG_FillRect,
    LCD_L0_MAG_GetPixelIndex,
    LCD_L0_GetRect,          /* Original routine is o.k. here, since we use the logical
                                 coordinates for clipping, which are not magnified */
    LCD_L0_MAG_SetPixelIndex,
    LCD_L0_MAG_XorPixel,
    LCD_L0_SetLUTEntry,
#endif
#if GUI_SUPPORT_MEMDEV
    NULL,                       /* pfFillPolygon */
    NULL,                       /* pfFillPolygonAA */
#if LCD_BITSPERPIXEL <= 8
sxxx
    &GUI_MEMDEV__APIList16,
    0
#else
    NULL, 0
#endif
#endif
};

/*
      *************************************************
      *                                               *
      *             LCD Device                        *
	  *       (if memory devices are supported)       *
      *                                               *
      *************************************************
*/

/*aBGB*/
const tLCDDEV_APIList LCD_L0_1_APIList =
{
#if LCD_YMAG_1 == 1
    LCD_L0_1_Color2Index,
    LCD_L0_1_Index2Color,
    LCD_L0_1_GetIndexMask,
    (void (*)(int, int, int, int, int, int, const U8 *, int, const void *))LCD_L0_1_DrawBitmap,
    LCD_L0_1_DrawHLine,
    LCD_L0_1_DrawVLine,
    LCD_L0_1_FillRect,
    LCD_L0_1_GetPixelIndex,
    LCD_L0_1_GetRect,
    LCD_L0_1_SetPixelIndex,
    LCD_L0_1_XorPixel,
    LCD_L0_1_SetLUTEntry,
#else
    LCD_L0_1_Color2Index,
    LCD_L0_1_Index2Color,
    LCD_L0_1_GetIndexMask,
    LCD_L0_1_MAG_DrawBitmap,
    LCD_L0_1_MAG_DrawHLine,
    LCD_L0_1_MAG_DrawVLine,
    LCD_L0_1_MAG_FillRect,
    LCD_L0_1_MAG_GetPixelIndex,
    LCD_L0_1_GetRect,          /* Original routine is o.k. here, since we use the logical
                                 coordinates for clipping, which are not magnified */
    LCD_L0_1_MAG_SetPixelIndex,
    LCD_L0_1_MAG_XorPixel,
    LCD_L0_1_SetLUTEntry,
#endif
#if GUI_SUPPORT_MEMDEV
    NULL,                       /* pfFillPolygon */
    NULL,                       /* pfFillPolygonAA */
#ifdef CONFIG_GUI_CORE_MEMDEV_EN
    &GUI_MEMDEV__APIList32, 0
#else
    NULL, 0
#endif
#endif
};

/*8BPP*/
const tLCDDEV_APIList LCD_L0_2_APIList =
{
#if LCD_YMAG_2 == 1
    LCD_L0_2_Color2Index,
    LCD_L0_2_Index2Color,
    LCD_L0_2_GetIndexMask,
    (void (*)(int, int, int, int, int, int, const U8 *, int, const void *))LCD_L0_2_DrawBitmap,
    LCD_L0_2_DrawHLine,
    LCD_L0_2_DrawVLine,
    LCD_L0_2_FillRect,
    LCD_L0_2_GetPixelIndex,
    LCD_L0_2_GetRect,
    LCD_L0_2_SetPixelIndex,
    LCD_L0_2_XorPixel,
    LCD_L0_2_SetLUTEntry,
#else
    LCD_L0_2_Color2Index,
    LCD_L0_2_Index2Color,
    LCD_L0_2_GetIndexMask,
    LCD_L0_2_MAG_DrawBitmap,
    LCD_L0_2_MAG_DrawHLine,
    LCD_L0_2_MAG_DrawVLine,
    LCD_L0_2_MAG_FillRect,
    LCD_L0_2_MAG_GetPixelIndex,
    LCD_L0_2_GetRect,          /* Original routine is o.k. here, since we use the logical
                                 coordinates for clipping, which are not magnified */
    LCD_L0_2_MAG_SetPixelIndex,
    LCD_L0_2_MAG_XorPixel,
    LCD_L0_2_SetLUTEntry,
#endif
#if GUI_SUPPORT_MEMDEV
    NULL,                       /* pfFillPolygon */
    NULL,                       /* pfFillPolygonAA */
#ifdef CONFIG_GUI_CORE_MEMDEV_EN
    &GUI_MEMDEV__APIList8, 0
#else
    NULL, 0
#endif
#endif
};


#if 0
/*2 bpp*/
const tLCDDEV_APIList LCD_L0_3_APIList =
{
#if LCD_YMAG_3 == 1
    LCD_L0_3_Color2Index,
    LCD_L0_3_Index2Color,
    LCD_L0_3_GetIndexMask,
    LCD_L0_3_DrawBitmap,
    LCD_L0_3_DrawHLine,
    LCD_L0_3_DrawVLine,
    LCD_L0_3_FillRect,
    LCD_L0_3_GetPixelIndex,
    LCD_L0_3_GetRect,
    LCD_L0_3_SetPixelIndex,
    LCD_L0_3_XorPixel,
    LCD_L0_3_SetLUTEntry,
#else
    LCD_L0_3_Color2Index,
    LCD_L0_3_Index2Color,
    LCD_L0_3_GetIndexMask,
    LCD_L0_3_MAG_DrawBitmap,
    LCD_L0_3_MAG_DrawHLine,
    LCD_L0_3_MAG_DrawVLine,
    LCD_L0_3_MAG_FillRect,
    LCD_L0_3_MAG_GetPixelIndex,
    LCD_L0_3_GetRect,          /* Original routine is o.k. here, since we use the logical
                                 coordinates for clipping, which are not magnified */
    LCD_L0_3_MAG_SetPixelIndex,
    LCD_L0_3_MAG_XorPixel,
    LCD_L0_3_SetLUTEntry,
#endif
#if GUI_SUPPORT_MEMDEV
    NULL,                       /* pfFillPolygon */
    NULL,                       /* pfFillPolygonAA */

    &GUI_MEMDEV__APIList8

#endif
};

#endif

#if 0
#if GUI_NUM_LAYERS > 4
const tLCDDEV_APIList LCD_L0_4_APIList =
{
#if LCD_YMAG_4 == 1
    LCD_L0_4_Color2Index,
    LCD_L0_4_Index2Color,
    LCD_L0_4_GetIndexMask,
    LCD_L0_4_DrawBitmap,
    LCD_L0_4_DrawHLine,
    LCD_L0_4_DrawVLine,
    LCD_L0_4_FillRect,
    LCD_L0_4_GetPixelIndex,
    LCD_L0_4_GetRect,
    LCD_L0_4_SetPixelIndex,
    LCD_L0_4_XorPixel,
    LCD_L0_4_SetLUTEntry,
#else
    LCD_L0_4_Color2Index,
    LCD_L0_4_Index2Color,
    LCD_L0_4_GetIndexMask,
    LCD_L0_4_MAG_DrawBitmap,
    LCD_L0_4_MAG_DrawHLine,
    LCD_L0_4_MAG_DrawVLine,
    LCD_L0_4_MAG_FillRect,
    LCD_L0_4_MAG_GetPixelIndex,
    LCD_L0_4_GetRect,          /* Original routine is o.k. here, since we use the logical
                                 coordinates for clipping, which are not magnified */
    LCD_L0_4_MAG_SetPixelIndex,
    LCD_L0_4_MAG_XorPixel,
    LCD_L0_4_SetLUTEntry,
#endif
#if GUI_SUPPORT_MEMDEV
    NULL,                       /* pfFillPolygon */
    NULL,                       /* pfFillPolygonAA */
#if LCD_BITSPERPIXEL_4 <= 8
    &GUI_MEMDEV__APIList8
#else
    &GUI_MEMDEV__APIList16
#endif
#endif
};
#endif
#endif

/***********************************************************
*
*                 LCD_aAPI
*
* Purpose:
*  This table lists the available displays/layers by a single pointer.
*  It is important that these are non-constants, because some high level
*  software (such as the VNC server, but maybe also the mouse "Cursor" mdoule)
*  may need to override these pointers in order to link itself into the chain
*  of drawing routines.
*  However, the API tables may of course be constants.
*
*/

const tLCDDEV_APIList * /*const*/ LCD_aAPI[] =
{
    &LCD_L0_APIList

#if GUI_NUM_LAYERS > 1
    , &LCD_L0_1_APIList
    , &LCD_L0_2_APIList
    , NULL               //&LCD_L0_3_APIList
#endif

#if  0   // mcl  del for user layer
#if GUI_NUM_LAYERS > 3
    , &LCD_L0_3_APIList
#endif
#if GUI_NUM_LAYERS > 4
    , &LCD_L0_4_APIList
#endif
#endif
};

#endif

/*************************** End of file ****************************/

