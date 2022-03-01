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
* File    : LCD_Private.h
* Purpose : Internals of the LCD level
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-1-6 11:55:42
**********************************************************************************************************************
*/


#ifndef LCD_Private_H
#define LCD_Private_H
#include "mod_orange_i.h"

#include "LCD_ConfDefaults.h"            /* Configuration header file */
#include "LCD_Protected.h"

typedef struct
{
    LCD_COLOR *paColor;
    I16         NumEntries;
} LCD_LUT_INFO;

#if LCD_PHYSCOLORS_IN_RAM
#if (LCD_FIXEDPALETTE == 0)
#if (LCD_FIXEDPALETTE == 0)
extern LCD_COLOR LCD_PhysColors[1 << LCD_BITSPERPIXEL];
#endif
#endif
#if (GUI_NUM_LAYERS > 1)
#if (LCD_FIXEDPALETTE_1 == 0)
extern LCD_COLOR LCD_PhysColors_1[1 << LCD_BITSPERPIXEL_1];
#endif
#endif
#if (GUI_NUM_LAYERS > 2)
#if (LCD_FIXEDPALETTE_2 == 0)
extern LCD_COLOR LCD_PhysColors_2[1 << LCD_BITSPERPIXEL_2];
#endif
#endif
#if (GUI_NUM_LAYERS > 3)
#if (LCD_FIXEDPALETTE_3 == 0)
extern LCD_COLOR LCD_PhysColors_3[1 << LCD_BITSPERPIXEL_3];
#endif
#endif
#if (GUI_NUM_LAYERS > 4)
#if (LCD_FIXEDPALETTE_4 == 0)
extern LCD_COLOR LCD_PhysColors_4[1 << LCD_BITSPERPIXEL_4];
#endif
#endif
#endif

#if GUI_COMPILER_SUPPORTS_FP

extern const struct tLCDDEV_APIList_struct * /* const */ LCD_aAPI[GUI_NUM_LAYERS];

const tLCDDEV_APIList *LCD_SetAPI(const tLCDDEV_APIList *pAPI, int Index);

#endif

#if (LCD_BITSPERPIXEL <= 8) && (GUI_NUM_LAYERS < 2)
#define LCD_BKCOLORINDEX GUI_Context.LCD.aColorIndex8[0]
#define LCD_COLORINDEX   GUI_Context.LCD.aColorIndex8[1]
#define LCD_ACOLORINDEX  GUI_Context.LCD.aColorIndex8
#elif (LCD_BITSPERPIXEL==16)
#define LCD_BKCOLORINDEX GUI_Context.LCD.aColorIndex16[0]
#define LCD_COLORINDEX   GUI_Context.LCD.aColorIndex16[1]
#define LCD_ACOLORINDEX  GUI_Context.LCD.aColorIndex16
#elif  (LCD_BITSPERPIXEL==32)  // mc add for aRGB
#define LCD_BKCOLORINDEX GUI_Context.LCD.aColorIndex32[0]
#define LCD_COLORINDEX   GUI_Context.LCD.aColorIndex32[1]
#define LCD_ACOLORINDEX  GUI_Context.LCD.aColorIndex32
#else
#error Unsupported color mode
#endif

extern LCD_PIXELINDEX LCD__aConvTable[LCD_MAX_LOG_COLORS];
void LCD_DrawBitmap(int x0, int y0,
                    int xsize, int ysize,
                    int xMul, int yMul,
                    int BitsPerPixel,
                    int BytesPerLine,
                    const U8 GUI_UNI_PTR *pPixel,
                    const LCD_PIXELINDEX *pTrans);
void LCD_DrawBitmap_Trans(int x0, int y0, int xsize, int ysize,
                          int BitsPerPixel, int BytesPerLine,
                          const U8 GUI_UNI_PTR *pPixel, const LCD_PIXELINDEX *pTrans, U8 index);  //added by MCL
void LCD_UpdateColorIndices(void);

/*********************************************************************
*
*          Support for Segment/COMLUTs
*
**********************************************************************
*/

#define LCD_TYPE_SEGTRANS U16
#define LCD_TYPE_COMTRANS U16

#ifdef LCD_LUT_COM
#ifdef LCDCOLOR_C
LCD_TYPE_COMTRANS LCD__aLine2Com0[Screen_Context.LCDYSize] = { LCD_LUT_COM };
#else
extern LCD_TYPE_COMTRANS LCD__aLine2Com0[Screen_Context.LCDYSize];
#endif
#endif

#ifdef LCD_LUT_SEG
#ifdef LCDCOLOR_C
LCD_TYPE_SEGTRANS LCD__aCol2Seg0[Screen_Context.LCDXSize] = { LCD_LUT_SEG };
#else
extern LCD_TYPE_COMTRANS LCD__aCol2Seg0[Screen_Context.LCDXSize];
#endif
#endif

#endif /* Avoid multiple inclusion */


/*************************** End of file ****************************/


