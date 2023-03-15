/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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
