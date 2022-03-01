/*
************************************************************************************************************************
*                                                      Lemon
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : LCDWin2.h
*
* Author : Miracle
*
* Version : 1.1.0
*
* Date : 2008.08.13
*
* Description :  8BPP driver head file
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
*  Miracle      2008.08.13       1.1.0        build the file
*
************************************************************************************************************************
*/




#ifndef LCDWIN_H_2
#define LCDWIN_H_2


#include "LCD_Private.h"              /* include LCDConf.h */
#include "GUI_Private.h"
#include "GUI_Protected.h"

/*#include "memory.h"*/

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/






/*********************************************************************
*
*       Exported code
*
**********************************************************************
*/

/*********************************************************************
*
*       LCD_L0_DrawPixel
*
*  Purpose:  Writes 1 pixel into the display.
*/

extern void LCD_L0_2_DrawPixel(int x, int y);
/*********************************************************************
*
*       LCD_L0_DrawHLine
*/
void LCD_L0_2_DrawHLine(int x0, int y,  int x1);



/*********************************************************************
*
*       LCD_L0_1_DrawVLine
*/
void LCD_L0_2_DrawVLine(int x, int y0,  int y1);



/*********************************************************************
*
*       LCD_L0_FillRect
*/
void LCD_L0_2_FillRect(int x0, int y0, int x1, int y1) ;



/*********************************************************************
*
*       LCD_L0_DrawBitmap
*/
void LCD_L0_2_DrawBitmap(int x0, int y0,
                         int xsize, int ysize,
                         int BitsPerPixel,
                         int BytesPerLine,
                         const U8 *pData, int Diff,
                         const LCD_PIXELINDEX *pTrans);

/*********************************************************************
*
*       LCD_L0_DrawBitmap
*
*  Purpose:
*    Sets the original position of the virtual display.
*    Has no function at this point with the PC-driver.
*/
void LCD_L0_2_SetOrg(int x, int y);
/*********************************************************************
*
*       Support for verification
*
*  Purpose:
*    The following routines are implemented, but have no functionility
*    at this point. The reason is that these functions are supposed
*    to supervise the hardware, which for obvious reasons can not be
*    done in a simulation.
*/




/*********************************************************************
*
*       LCD_L0_SetLUTEntry
*/

void LCD_L0_2_SetLUTEntry(U8 Pos, LCD_COLOR color);



/*********************************************************************
*
*       LCD_L0_Init
*/

int  LCD_L0_2_Init(void);





/*********************************************************************
*
*       LCD_L0_ReInit
*
*  Purpose:
*    This routine is supplied for compatibility and interchangability of
*    "C"-sources with embedded versions of the driver. It has no real
*    effect in the PC-version as there is simply no need to re-initialize
*    the LCD since it is just simulated anyhow.
*/
void LCD_L0_2_ReInit(void);


unsigned LCD_L0_2_GetPixelIndex(int x, int y);  //mcl


/*********************************************************************
*
*       LCD_L0_XorPixel
*
*  Purpose:
*    Inverts 1 pixel of the display.
*/

void LCD_L0_2_XorPixel(int x, int y) ;


/*********************************************************************
*
*       LCD_L0_SetPixelIndex
*
*  Purpose:
*    Writes 1 pixel into the display.
*/

void LCD_L0_2_SetPixelIndex(int x, int y, int ColorIndex);


#endif




