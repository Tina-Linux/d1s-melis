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
#ifndef LCDWIN_H_1
#define LCDWIN_H_1

/*#include <windows.h>*/
//#include "LCD.h"
#include "LCD_Private.h"              /* include LCDConf.h */
//#include "LCDSIM.h"
//#include "lcdctl_i.h"
#include "GUI_Private.h"
//#include "lcdctl.h"
/*added  by MCL*/

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

extern void LCD_L0_1_DrawPixel(int x, int y);
/*********************************************************************
*
*       LCD_L0_DrawHLine
*/
void LCD_L0_1_DrawHLine(int x0, int y,  int x1);



/*********************************************************************
*
*       LCD_L0_1_DrawVLine
*/
void LCD_L0_1_DrawVLine(int x, int y0,  int y1);



/*********************************************************************
*
*       LCD_L0_FillRect
*/
void LCD_L0_1_FillRect(int x0, int y0, int x1, int y1) ;



/*********************************************************************
*
*       LCD_L0_DrawBitmap
*/
void LCD_L0_1_DrawBitmap(int x0, int y0,
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
void LCD_L0_1_SetOrg(int x, int y);
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

void LCD_L0_1_SetLUTEntry(U8 Pos, LCD_COLOR color);



/*********************************************************************
*
*       LCD_L0_Init
*/

int  LCD_L0_1_Init(void);





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
void LCD_L0_1_ReInit(void);


unsigned LCD_L0_1_GetPixelIndex(int x, int y);  //mcl


/*********************************************************************
*
*       LCD_L0_XorPixel
*
*  Purpose:
*    Inverts 1 pixel of the display.
*/

void LCD_L0_1_XorPixel(int x, int y) ;


/*********************************************************************
*
*       LCD_L0_SetPixelIndex
*
*  Purpose:
*    Writes 1 pixel into the display.
*/

void LCD_L0_1_SetPixelIndex(int x, int y, int ColorIndex);


#endif
