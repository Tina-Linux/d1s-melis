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
#ifndef LCD_INTERN_H
#define LCD_INTERN_H
#include "mod_orange_i.h"

/*********************************************************************
*
*      Data types
*
**********************************************************************
*/

typedef struct
{
    int              NumEntries;
    const LCD_COLOR *pPalEntries;
} LCD_PHYSPALETTE;

/*********************************************************************
*
*      Data
*
**********************************************************************
*/
extern GUI_CONST_STORAGE U8 LCD_aMirror[256];


/********************************************************
*
*     Color conversion routines (used internally only !
*
*********************************************************
*/
unsigned LCD_Color2Index_User(LCD_COLOR Color);
unsigned LCD_Color2Index_0(LCD_COLOR Color, const LCD_PHYSPALETTE *pPhysPal);
unsigned LCD_Color2Index_1(LCD_COLOR Color);
unsigned LCD_Color2Index_M1(LCD_COLOR Color);
unsigned LCD_Color2Index_2(LCD_COLOR Color);
unsigned LCD_Color2Index_M2(LCD_COLOR Color);
unsigned LCD_Color2Index_4(LCD_COLOR Color);
unsigned LCD_Color2Index_M4(LCD_COLOR Color);
unsigned LCD_Color2Index_111(LCD_COLOR Color);
unsigned LCD_Color2Index_222(LCD_COLOR Color);
unsigned LCD_Color2Index_233(LCD_COLOR Color);
unsigned LCD_Color2Index_323(LCD_COLOR Color);
unsigned LCD_Color2Index_332(LCD_COLOR Color);
unsigned LCD_Color2Index_444_12(LCD_COLOR Color);
unsigned LCD_Color2Index_444_12_1(LCD_COLOR Color);
unsigned LCD_Color2Index_444_16(LCD_COLOR Color);
unsigned LCD_Color2Index_555(LCD_COLOR Color);
unsigned LCD_Color2Index_565(LCD_COLOR Color);
unsigned LCD_Color2Index_556(LCD_COLOR Color);
unsigned LCD_Color2Index_655(LCD_COLOR Color);
unsigned LCD_Color2Index_8666(LCD_COLOR Color);
unsigned LCD_Color2Index_8666_1(LCD_COLOR Color);
unsigned LCD_Color2Index_8888(LCD_COLOR Color);       //mcl add
unsigned LCD_Color2Index_M111(LCD_COLOR Color);
unsigned LCD_Color2Index_M222(LCD_COLOR Color);
unsigned LCD_Color2Index_M233(LCD_COLOR Color);
unsigned LCD_Color2Index_M323(LCD_COLOR Color);
unsigned LCD_Color2Index_M332(LCD_COLOR Color);
unsigned LCD_Color2Index_M444_12(LCD_COLOR Color);
unsigned LCD_Color2Index_M444_16(LCD_COLOR Color);
unsigned LCD_Color2Index_M555(LCD_COLOR Color);
unsigned LCD_Color2Index_M565(LCD_COLOR Color);
unsigned LCD_Color2Index_M556(LCD_COLOR Color);
unsigned LCD_Color2Index_M655(LCD_COLOR Color);
unsigned LCD_Color2Index_M8888(LCD_COLOR Color);      // mcl add



LCD_COLOR LCD_Index2Color_User(int Index);
LCD_COLOR LCD_Index2Color_0(int Index, const LCD_PHYSPALETTE *pPhysPal);
LCD_COLOR LCD_Index2Color_1(int Index);
LCD_COLOR LCD_Index2Color_2(int Index);
LCD_COLOR LCD_Index2Color_4(int Index);
LCD_COLOR LCD_Index2Color_111(int Index);
LCD_COLOR LCD_Index2Color_222(int Index);
LCD_COLOR LCD_Index2Color_233(int Index);
LCD_COLOR LCD_Index2Color_323(int Index);
LCD_COLOR LCD_Index2Color_332(int Index);
LCD_COLOR LCD_Index2Color_444_12(int Index);
LCD_COLOR LCD_Index2Color_444_12_1(int Index);
LCD_COLOR LCD_Index2Color_444_16(int Index);
LCD_COLOR LCD_Index2Color_555(int Index);
LCD_COLOR LCD_Index2Color_565(int Index);
LCD_COLOR LCD_Index2Color_556(int Index);
LCD_COLOR LCD_Index2Color_655(int Index);
LCD_COLOR LCD_Index2Color_8888(int Index);       //mcl add
LCD_COLOR LCD_Index2Color_8666(int Index);
LCD_COLOR LCD_Index2Color_8666_1(int Index);
LCD_COLOR LCD_Index2Color_M111(int Index);
LCD_COLOR LCD_Index2Color_M222(int Index);
LCD_COLOR LCD_Index2Color_M233(int Index);
LCD_COLOR LCD_Index2Color_M323(int Index);
LCD_COLOR LCD_Index2Color_M332(int Index);
LCD_COLOR LCD_Index2Color_M444_12(int Index);
LCD_COLOR LCD_Index2Color_M444_16(int Index);
LCD_COLOR LCD_Index2Color_M555(int Index);
LCD_COLOR LCD_Index2Color_M565(int Index);
LCD_COLOR LCD_Index2Color_M556(int Index);
LCD_COLOR LCD_Index2Color_M655(int Index);
LCD_COLOR LCD_Index2Color_M8888(int Index);      //mcl add

unsigned LCD_GetIndexMask_User(void);
unsigned LCD_GetIndexMask_0(void);
unsigned LCD_GetIndexMask_1(void);
unsigned LCD_GetIndexMask_M1(void);
unsigned LCD_GetIndexMask_2(void);
unsigned LCD_GetIndexMask_M2(void);
unsigned LCD_GetIndexMask_4(void);
unsigned LCD_GetIndexMask_M4(void);
unsigned LCD_GetIndexMask_111(void);
unsigned LCD_GetIndexMask_222(void);
unsigned LCD_GetIndexMask_233(void);
unsigned LCD_GetIndexMask_323(void);
unsigned LCD_GetIndexMask_332(void);
unsigned LCD_GetIndexMask_444_12(void);
unsigned LCD_GetIndexMask_444_12_1(void);
unsigned LCD_GetIndexMask_444_16(void);
unsigned LCD_GetIndexMask_555(void);
unsigned LCD_GetIndexMask_565(void);
unsigned LCD_GetIndexMask_556(void);
unsigned LCD_GetIndexMask_655(void);
unsigned LCD_GetIndexMask_8666(void);
unsigned LCD_GetIndexMask_8666_1(void);
unsigned LCD_GetIndexMask_8888(void);      // mcl add
unsigned LCD_GetIndexMask_M111(void);
unsigned LCD_GetIndexMask_M222(void);
unsigned LCD_GetIndexMask_M233(void);
unsigned LCD_GetIndexMask_M323(void);
unsigned LCD_GetIndexMask_M332(void);
unsigned LCD_GetIndexMask_M444_12(void);
unsigned LCD_GetIndexMask_M444_16(void);
unsigned LCD_GetIndexMask_M555(void);
unsigned LCD_GetIndexMask_M565(void);
unsigned LCD_GetIndexMask_M556(void);
unsigned LCD_GetIndexMask_M655(void);
unsigned LCD_GetIndexMask_M8888(void);       //mcl add

void LCD_SelectLCD(void);

/*********************************************************************
*
*              Declarations for LCD_L0_
*
**********************************************************************

  Prefixes are as follows:
  LCD_L0_xxx       ... Driver for first  LCD
  LCD_L0_1_xxx     ... Driver for second LCD/layer
  LCD_L0_2_xxx     ... Driver for third  LCD/layer
  LCD_L0_MAG_xxx   ... Driver for first  LCD, magnification level (calling LCD_L0_xxx after magnification)
  LCD_L0_DELTA_xxx ... Driver for first  LCD, delta mode

*/

unsigned int LCD_L0_Color2Index(LCD_COLOR Color);
unsigned int LCD_L0_1_Color2Index(LCD_COLOR Color);
unsigned int LCD_L0_2_Color2Index(LCD_COLOR Color);
unsigned int LCD_L0_3_Color2Index(LCD_COLOR Color);
unsigned int LCD_L0_4_Color2Index(LCD_COLOR Color);
void         LCD_L0_DrawHLine(int x0, int y0,  int x1);
void         LCD_L0_1_DrawHLine(int x0, int y0,  int x1);
void         LCD_L0_2_DrawHLine(int x0, int y0,  int x1);
void         LCD_L0_3_DrawHLine(int x0, int y0,  int x1);
void         LCD_L0_4_DrawHLine(int x0, int y0,  int x1);
void         LCD_L0_MAG_DrawHLine(int x0, int y0,  int x1);
void         LCD_L0_DELTA_DrawHLine(int x0, int y0,  int x1);
void         LCD_L0_DrawVLine(int x,  int y0,  int y1);
void         LCD_L0_1_DrawVLine(int x,  int y0,  int y1);
void         LCD_L0_2_DrawVLine(int x,  int y0,  int y1);
void         LCD_L0_3_DrawVLine(int x,  int y0,  int y1);
void         LCD_L0_4_DrawVLine(int x,  int y0,  int y1);
void         LCD_L0_MAG_DrawVLine(int x,  int y0,  int y1);
void         LCD_L0_DELTA_DrawVLine(int x,  int y0,  int y1);
void         LCD_L0_FillRect(int x0, int y0, int x1, int y1);
void         LCD_L0_1_FillRect(int x0, int y0, int x1, int y1);
void         LCD_L0_2_FillRect(int x0, int y0, int x1, int y1);
void         LCD_L0_3_FillRect(int x0, int y0, int x1, int y1);
void         LCD_L0_4_FillRect(int x0, int y0, int x1, int y1);
void         LCD_L0_MAG_FillRect(int x0, int y0, int x1, int y1);
void         LCD_L0_DELTA_FillRect(int x0, int y0, int x1, int y1);
unsigned int LCD_L0_GetPixelIndex(int x,  int y);
unsigned int LCD_L0_1_GetPixelIndex(int x,  int y);
unsigned int LCD_L0_2_GetPixelIndex(int x,  int y);
unsigned int LCD_L0_3_GetPixelIndex(int x,  int y);
unsigned int LCD_L0_4_GetPixelIndex(int x,  int y);
unsigned int LCD_L0_MAG_GetPixelIndex(int x,  int y);
unsigned int LCD_L0_DELTA_GetPixelIndex(int x,  int y);
void         LCD_L0_GetRect(LCD_RECT *pRect);
void         LCD_L0_1_GetRect(LCD_RECT *pRect);
void         LCD_L0_2_GetRect(LCD_RECT *pRect);
void         LCD_L0_3_GetRect(LCD_RECT *pRect);
void         LCD_L0_4_GetRect(LCD_RECT *pRect);
void         LCD_L0_SetPixelIndex(int x,  int y, int ColorIndex);
void         LCD_L0_1_SetPixelIndex(int x,  int y, int ColorIndex);
void         LCD_L0_2_SetPixelIndex(int x,  int y, int ColorIndex);
void         LCD_L0_3_SetPixelIndex(int x,  int y, int ColorIndex);
void         LCD_L0_4_SetPixelIndex(int x,  int y, int ColorIndex);
void         LCD_L0_MAG_SetPixelIndex(int x,  int y, int ColorIndex);
void         LCD_L0_DELTA_SetPixelIndex(int x,  int y, int ColorIndex);
void         LCD_L0_XorPixel(int x,  int y);
void         LCD_L0_1_XorPixel(int x,  int y);
void         LCD_L0_2_XorPixel(int x,  int y);
void         LCD_L0_3_XorPixel(int x,  int y);
void         LCD_L0_4_XorPixel(int x,  int y);
void         LCD_L0_MAG_XorPixel(int x,  int y);
void         LCD_L0_DELTA_XorPixel(int x,  int y);
int          LCD_L0_Init(void);
int          LCD_L0_1_Init(void);
int          LCD_L0_2_Init(void);
int          LCD_L0_3_Init(void);
int          LCD_L0_4_Init(void);
int          LCD_L0_MAG_Init(void);
int          LCD_L0_DELTA_Init(void);
void         LCD_L0_On(void);
void         LCD_L0_1_On(void);
void         LCD_L0_2_On(void);
void         LCD_L0_3_On(void);
void         LCD_L0_4_On(void);
void         LCD_L0_Off(void);
void         LCD_L0_1_Off(void);
void         LCD_L0_2_Off(void);
void         LCD_L0_3_Off(void);
void         LCD_L0_4_Off(void);
void         LCD_L0_SetLUTEntry(U8 Pos, LCD_COLOR color);
void         LCD_L0_1_SetLUTEntry(U8 Pos, LCD_COLOR color);
void         LCD_L0_2_SetLUTEntry(U8 Pos, LCD_COLOR color);
void         LCD_L0_3_SetLUTEntry(U8 Pos, LCD_COLOR color);
void         LCD_L0_4_SetLUTEntry(U8 Pos, LCD_COLOR color);

void LCD_L0_ReInit(void);        /* Reinitialize LCD using config-paramters */
void LCD_L0_Refresh(void);       /* Refresh entire LCD */
/* Refresh LCD partially. Could be one line, depends on what
   the LCD controller can handle efficiently */
void LCD_L0_SetOrg(int x, int y);


#endif /* LCD_INTERN_H */



/*************************** End of file ****************************/
