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
#ifndef  GUI_PRIVATE_H
#define  GUI_PRIVATE_H


#include "mod_orange_i.h"

#include "GUI_Protected.h"
#include "LCD_Private.h"   /* Required because of LCD_PIXELINDEX, which depends on LCDConf */

#if GUI_SUPPORT_DEVICES
#define LCDDEV_L0_Color2Index         GUI_Context.pDeviceAPI->pfColor2Index
#define LCDDEV_L0_DrawBitmap          GUI_Context.pDeviceAPI->pfDrawBitmap
#define LCDDEV_L0_DrawHLine           GUI_Context.pDeviceAPI->pfDrawHLine
#define LCDDEV_L0_DrawVLine           GUI_Context.pDeviceAPI->pfDrawVLine
#define LCDDEV_L0_DrawPixel           GUI_Context.pDeviceAPI->pfDrawPixel
#define LCDDEV_L0_FillRect            GUI_Context.pDeviceAPI->pfFillRect
#define LCDDEV_L0_GetPixel            GUI_Context.pDeviceAPI->pfGetPixel
#define LCDDEV_L0_GetRect             GUI_Context.pDeviceAPI->pfGetRect
#define LCDDEV_L0_GetPixelIndex       GUI_Context.pDeviceAPI->pfGetPixelIndex
#define LCDDEV_L0_Index2Color         GUI_Context.pDeviceAPI->pfIndex2Color
#define LCDDEV_L0_SetPixelIndex       GUI_Context.pDeviceAPI->pfSetPixelIndex
#define LCDDEV_L0_XorPixel            GUI_Context.pDeviceAPI->pfXorPixel
#else
#define LCDDEV_L0_Color2Index         LCD_L0_Color2Index
#define LCDDEV_L0_DrawBitmap          LCD_L0_DrawBitmap
#define LCDDEV_L0_DrawHLine           LCD_L0_DrawHLine
#define LCDDEV_L0_DrawVLine           LCD_L0_DrawVLine
#define LCDDEV_L0_DrawPixel           LCD_L0_DrawPixel
#define LCDDEV_L0_FillRect            LCD_L0_FillRect
#define LCDDEV_L0_GetPixel            LCD_L0_GetPixel
#define LCDDEV_L0_GetRect             LCD_L0_GetRect
#define LCDDEV_L0_GetPixelIndex       LCD_L0_GetPixelIndex
#define LCDDEV_L0_Index2Color         LCD_L0_Index2Color
#define LCDDEV_L0_SetPixelIndex       LCD_L0_SetPixelIndex
#define LCDDEV_L0_XorPixel            LCD_L0_XorPixel
#endif

//added by MCL
#define  LCDDEV_L0_DrawBitmap_trans   LCD_L0_DrawBitmap8bpp
void LCD_L0_DrawBitmap(int x0, int y0, int xsize, int ysize,
                       int BitsPerPixel, int BytesPerLine,
                       const U8 GUI_UNI_PTR *pData, int Diff, const LCD_PIXELINDEX *pTrans);
void LCD_L0_DrawBitmap8bpp(int x0, int y0,
                           int xsize, int ysize,
                           int BitsPerPixel,
                           int BytesPerLine,
                           const U8 *pData, int Diff,
                           const LCD_PIXELINDEX *pTrans,
                           U8 index);         //added by MCL
void LCD_L0_1_DrawBitmap(int x0, int y0, int xsize, int ysize,
                         int BitsPerPixel, int BytesPerLine,
                         const U8 GUI_UNI_PTR *pData, int Diff, const LCD_PIXELINDEX *pTrans);
void LCD_L0_2_DrawBitmap(int x0, int y0, int xsize, int ysize,
                         int BitsPerPixel, int BytesPerLine,
                         const U8 GUI_UNI_PTR *pData, int Diff, const LCD_PIXELINDEX *pTrans);
void LCD_L0_3_DrawBitmap(int x0, int y0, int xsize, int ysize,
                         int BitsPerPixel, int BytesPerLine,
                         const U8 GUI_UNI_PTR *pData, int Diff, const LCD_PIXELINDEX *pTrans);
void LCD_L0_4_DrawBitmap(int x0, int y0, int xsize, int ysize,
                         int BitsPerPixel, int BytesPerLine,
                         const U8 GUI_UNI_PTR *pData, int Diff, const LCD_PIXELINDEX *pTrans);
void LCD_L0_MAG_DrawBitmap(int x0, int y0, int xsize, int ysize,
                           int BitsPerPixel, int BytesPerLine,
                           const U8 GUI_UNI_PTR *pData, int Diff, const LCD_PIXELINDEX *pTrans);
void LCD_L0_DELTA_DrawBitmap(int x0, int y0, int xsize, int ysize,
                             int BitsPerPixel, int BytesPerLine,
                             const U8 GUI_UNI_PTR *pData, int Diff, const LCD_PIXELINDEX *pTrans);

LCD_PIXELINDEX  *LCD_GetpPalConvTable(const LCD_LOGPALETTE GUI_UNI_PTR   *pLogPal);
LCD_PIXELINDEX  *LCD_GetpPalConvTableUncached(const LCD_LOGPALETTE GUI_UNI_PTR   *pLogPal);


#define GUI_POS_AUTO -4095   /* Position value for auto-pos */

#if GUI_SUPPORT_MEMDEV
void  *GUI_MEMDEV__XY2PTR(int x, int y);
void  *GUI_MEMDEV__XY2PTREx(GUI_MEMDEV *pDev, int x, int y);
void   GUI_MEMDEV__WriteToActiveAt(GUI_MEMDEV_Handle hMem, int x, int y);
#endif

#endif   /* ifdef GUI_H */

/*************************** End of file ****************************/
