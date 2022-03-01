/*
************************************************************************************************************************
*                                                      Lemon
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : LCDWin1.c
*
* Author : Miracle
*
* Version : 1.1.0
*
* Date : 2008.08.13
*
* Description :aRGB
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

#ifndef LCD_WIN_C_1
#define LCD_WIN_C_1  1

#include "gui_core_lcddev_private.h"
#include "LCDWin1.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

/*
#if LCD_BITSPERPIXEL <= 8
  #define PIXELINDEX U8
#else
  #define PIXELINDEX WORD
#endif
*/

#ifdef LCD_ASSERT
#undef LCD_ASSERT
#endif
#define LCD_ASSERT(v) ASSERT(v)

#ifndef LCD_DISPLAY_INDEX
#define LCD_DISPLAY_INDEX  0
#endif
#if 0   //mcl
#undef LCD_PIXELINDEX     //  mcl add
#define LCD_PIXELINDEX U32

#undef  LCD_BITSPERPIXEL    //mcl add
#define LCD_BITSPERPIXEL  32

#undef  LCD_NUM_COLORS      //mcl add
#define LCD_NUM_COLORS  1L << 24
#endif
#define PIXEL_STEP    2

#define A_MASK 0xff000000
#define R_MASK 0x00ff0000
#define G_MASK 0x0000ff00
#define B_MASK 0x000000ff

/*********************************************************************
*
*       Macros for internal use
*/
#ifdef _DEBUG
static int _CheckBound(unsigned int c)
{
    unsigned int NumColors = LCD_BITSPERPIXEL > 8 ? 0xffff : (1 << LCD_BITSPERPIXEL) - 1;

    if (c > NumColors)
    {
        GUI_DEBUG_ERROROUT("LCDWin::SETPIXEL: parameters out of bounds");
        return 1;
    }

    return 0;
}
#ifdef SIM_PC_WIN
#define SETPIXEL(x, y, c)  LCD2440_SetPixel(x, y, c);
#else
#define SETPIXEL(x, y, c) \
    if (!_CheckBound(c)) { \
        LCD2440_SetPixel(x, y, c); \
    }
#endif
#else
#define SETPIXEL(x, y, c) LCD2440_SetPixel(x, y, c) //modefied by MCL
#endif
#define XORPIXEL(x, y)    _XorPixel(x,y)
#define GETPIXEL(x, y)    LCD2440_GetPixel(x,y) // modefied by MCL



/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

static LCD_PIXELINDEX LCD2440_GetPixel(int x, int y); //added by MCL
static void  LCD2440_SetPixel(int x, int y, LCD_PIXELINDEX c); //added by MCL
static void _XorPixel(int x, int y);


/*********************************************************************
*
*       _XorPixel   // only for palette
*/

static void _XorPixel(int x, int y)
{
    LCD_PIXELINDEX clolorValue = 0;
    LCD_PIXELINDEX *pPixelAdress = (LCD_PIXELINDEX *)orange.fbaddr[GUI_Context.SelLayer];

    if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_NORMAL)
    {
        pPixelAdress    = pPixelAdress + (y * (Screen_Context.LCDXSize[GUI_Context.SelLayer] << PIXEL_STEP)  + (x << PIXEL_STEP));
        *pPixelAdress   = clolorValue;
    }
    else
    {
        if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE90)
        {
            // anticlockwise  90'
            pPixelAdress    = (U32 *)(pPixelAdress + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x) * (Screen_Context.LCDYSize[GUI_Context.SelLayer]))  + (y));
        }
        else if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE180)
        {
            //180'
            pPixelAdress = (U32 *)(pPixelAdress + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y) * (Screen_Context.LCDXSize[GUI_Context.SelLayer]))  + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x)));
        }
        else  if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE270)
        {
            // clockwise  90'
            pPixelAdress = (U32 *)(pPixelAdress + (x * (Screen_Context.LCDYSize[GUI_Context.SelLayer]))  + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y))) ;
        }

        clolorValue = *pPixelAdress;
    }

    clolorValue = LCD_NUM_COLORS - 1 - clolorValue;
    *pPixelAdress = clolorValue;
}



/*********************************************************************
*
static void LCD2440_SetPixel( int  x, int y, LCD_PIXELINDEX clolorValue)

 {
    U8 * pPixelAdress=(unsigned char *)lemon.fbaddr[GUI_Context.SelLayer];

    if (Screen_Context.LCDDirection[GUI_Context.SelLayer]==0)
    {
      pPixelAdress  = pPixelAdress  + (y *(Screen_Context.LCDXSize[GUI_Context.SelLayer])<<PIXEL_STEP)  + (x<<PIXEL_STEP) ;
      *(U32 *)pPixelAdress  = (U32)clolorValue;
    }
  else
    {
       pPixelAdress =pPixelAdress + ((Screen_Context.LCDXSize[GUI_Context.SelLayer]-1-x) *(Screen_Context.LCDYSize[GUI_Context.SelLayer])<<PIXEL_STEP)  + (y<<PIXEL_STEP) ;
       *(U32 *)pPixelAdress = (U32)clolorValue;
    }
 }

*/
//------------------------------------------------------------------------------

static void LCD2440_SetPixel(int  x, int y, LCD_PIXELINDEX clolorValue)
{
    U32 *pPixelAdress = NULL;
    pPixelAdress = (U32 *)orange.fbaddr[GUI_Context.SelLayer];
//__log("x = %d  y = %d   color  = %x",x,y,clolorValue);
    if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_NORMAL)
    {
        if (!GUI_Context.BlendFlag)
        {
            pPixelAdress  = (U32 *)(pPixelAdress  + y * Screen_Context.LCDXSize[GUI_Context.SelLayer] + x) ;
           // __log("[0x%p]\n",pPixelAdress);
            *pPixelAdress  = (U32)clolorValue;
        }
        else
        {
            U32  top, bottom;
            U32  destin;
            U32  R, G, B;
            U8  Alpha = 0;
            top       =  clolorValue;
            bottom  =  LCD2440_GetPixel(x, y);
            Alpha = ((top & A_MASK) >> 24) & 0xff;
            R = ((((top & R_MASK) >> 16) * (Alpha + 1))   + (((bottom & R_MASK) >> 16) << 8) - ((bottom & R_MASK) >> 16) * (Alpha + 1)) >> 8 ;
            G = ((((top & G_MASK) >> 8) * (Alpha + 1))  + (((bottom & G_MASK) >> 8) << 8) - (((bottom & G_MASK) >> 8) * (Alpha + 1))) >> 8;
            B = ((((top & B_MASK)) * (Alpha + 1))  + ((bottom & B_MASK) << 8) - (bottom & B_MASK) * (Alpha + 1)) >> 8;
            destin = (0xff000000 & A_MASK) | ((R << 16) & R_MASK) | ((G << 8) & G_MASK) | ((B << 0) & B_MASK);
            pPixelAdress  = (U32 *)(pPixelAdress  + y * Screen_Context.LCDXSize[GUI_Context.SelLayer] + x) ;
            *pPixelAdress  = (U32)destin;
        }
    }
    else
    {
        if (!GUI_Context.BlendFlag)
        {
            if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE90)
            {
                // anticlockwise  90'
                pPixelAdress    = (U32 *)(pPixelAdress + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x) * (Screen_Context.LCDYSize[GUI_Context.SelLayer]))  + (y));
            }
            else if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE180)
            {
                //180'
                pPixelAdress = (U32 *)(pPixelAdress + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y) * (Screen_Context.LCDXSize[GUI_Context.SelLayer]))  + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x)));
            }
            else  if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE270)
            {
                // clockwise  90'
                pPixelAdress = (U32 *)(pPixelAdress + (x * (Screen_Context.LCDYSize[GUI_Context.SelLayer]))  + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y))) ;
            }

            *pPixelAdress  = (U32)clolorValue;
        }
        else
        {
            U32  top, bottom;
            U32  destin;
            U32  R, G, B;
            U8  Alpha = 0;
            top       =  clolorValue;
            bottom  =  LCD2440_GetPixel(x, y);
            Alpha = ((top & A_MASK) >> 24) & 0xff;
            R = ((((top & R_MASK) >> 16) * (Alpha + 1))   + (((bottom & R_MASK) >> 16) << 8) - ((bottom & R_MASK) >> 16) * (Alpha + 1)) >> 8 ;
            G = ((((top & G_MASK) >> 8) * (Alpha + 1))  + (((bottom & G_MASK) >> 8) << 8) - (((bottom & G_MASK) >> 8) * (Alpha + 1))) >> 8;
            B = ((((top & B_MASK)) * (Alpha + 1))  + ((bottom & B_MASK) << 8) - (bottom & B_MASK) * (Alpha + 1)) >> 8;
            destin = (0xff000000 & A_MASK) | ((R << 16) & R_MASK) | ((G << 8) & G_MASK) | ((B << 0) & B_MASK);

            if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE90)
            {
                // anticlockwise  90'
                pPixelAdress    = (U32 *)(pPixelAdress + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x) * (Screen_Context.LCDYSize[GUI_Context.SelLayer]))  + (y));
            }
            else if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE180)
            {
                //180'
                pPixelAdress = (U32 *)(pPixelAdress + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y) * (Screen_Context.LCDXSize[GUI_Context.SelLayer]))  + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x)));
            }
            else  if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE270)
            {
                // clockwise  90'
                pPixelAdress = (U32 *)(pPixelAdress + (x * (Screen_Context.LCDYSize[GUI_Context.SelLayer]))  + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y))) ;
            }

            *pPixelAdress  = (U32)destin;
        }
    }
}




/*********************************************************************
*
*       LCD2440_GetPixel
*/
static U32 LCD2440_GetPixel(int x, int y)
{
    U32 *pPixelAdress = NULL;
    U32 clolorValue  = 0;
    pPixelAdress = (U32 *)orange.fbaddr[GUI_Context.SelLayer];

    if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_NORMAL)
    {
        pPixelAdress  = (U32 *)(pPixelAdress  + y * Screen_Context.LCDXSize[GUI_Context.SelLayer] + x) ;
        clolorValue =  *pPixelAdress ;
        return clolorValue;
    }
    else
    {
        if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE90)
        {
            // anticlockwise  90'
            pPixelAdress    = (U32 *)(pPixelAdress + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x) * (Screen_Context.LCDYSize[GUI_Context.SelLayer]))  + (y));
        }
        else if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE180)
        {
            //180'
            pPixelAdress = (U32 *)(pPixelAdress + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y) * (Screen_Context.LCDXSize[GUI_Context.SelLayer]))  + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x)));
        }
        else  if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE270)
        {
            // clockwise  90'
            pPixelAdress = (U32 *)(pPixelAdress + (x * (Screen_Context.LCDYSize[GUI_Context.SelLayer]))  + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y))) ;
        }

        clolorValue =  *pPixelAdress  ;
        return  clolorValue;
    }
}


/*********************************************************************
*
*       _DrawBitLine1BPP
*/
static void _DrawBitLine1BPP(int x, int y, U8 const *p, int Diff, int xsize, const LCD_PIXELINDEX *pTrans)
{
    LCD_PIXELINDEX Index0 = *(pTrans + 0);
    LCD_PIXELINDEX Index1 = *(pTrans + 1);
    x += Diff;

    switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR))
    {
        case 0:    /* Write mode */
            do
            {
                //      SETPIXEL(x++,y, (*p & (0x80>>Diff)) ? Index1 : Index0);
                //scott
                LCD_PIXELINDEX color = (*p & (0x80 >> Diff)) ? Index1 : Index0 ;
                SETPIXEL(x, y, color);
                x++;

                //scott
                if (++Diff == 8)
                {
                    Diff = 0;
                    p++;
                }
            } while (--xsize);

            break;

        case LCD_DRAWMODE_TRANS:
            do
            {
                if (*p & (0x80 >> Diff))
                {
                    SETPIXEL(x, y, Index1);
                }

                x++;

                if (++Diff == 8)
                {
                    Diff = 0;
                    p++;
                }
            } while (--xsize);

            break;

        case LCD_DRAWMODE_XOR:
            ;

            do
            {
                if (*p & (0x80 >> Diff))
                {
                    int Pixel = GETPIXEL(x, y);
                    SETPIXEL(x, y, LCD_NUM_COLORS - 1 - Pixel);
                }

                x++;

                if (++Diff == 8)
                {
                    Diff = 0;
                    p++;
                }
            } while (--xsize);

            break;
    }
}

/*********************************************************************
*
*       _DrawBitLine2BPP
*/
#if (LCD_MAX_LOG_COLORS > 2)
static void _DrawBitLine2BPP(int x, int y, U8 const *p, int Diff, int xsize, const LCD_PIXELINDEX *pTrans)
{
    LCD_PIXELINDEX Pixels = *p;
    int CurrentPixel = Diff;
    x += Diff;

    switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR))
    {
        case 0:
            if (pTrans)
            {
                do
                {
                    int Shift = (3 - CurrentPixel) << 1;
                    int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
                    LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
                    SETPIXEL(x++, y, PixelIndex);

                    if (++CurrentPixel == 4)
                    {
                        CurrentPixel = 0;
                        Pixels = *(++p);
                    }
                } while (--xsize);
            }
            else
            {
                do
                {
                    int Shift = (3 - CurrentPixel) << 1;
                    int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
                    SETPIXEL(x++, y, Index);

                    if (++CurrentPixel == 4)
                    {
                        CurrentPixel = 0;
                        Pixels = *(++p);
                    }
                } while (--xsize);
            }

            break;

        case LCD_DRAWMODE_TRANS:
            if (pTrans)
            {
                do
                {
                    int Shift = (3 - CurrentPixel) << 1;
                    int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;

                    if (Index)
                    {
                        LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
                        SETPIXEL(x, y, PixelIndex);
                    }

                    x++;

                    if (++CurrentPixel == 4)
                    {
                        CurrentPixel = 0;
                        Pixels = *(++p);
                    }
                } while (--xsize);
            }
            else
            {
                do
                {
                    int Shift = (3 - CurrentPixel) << 1;
                    int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;

                    if (Index)
                    {
                        SETPIXEL(x, y, Index);
                    }

                    x++;

                    if (++CurrentPixel == 4)
                    {
                        CurrentPixel = 0;
                        Pixels = *(++p);
                    }
                } while (--xsize);
            }

            break;
    }
}
#endif

/*********************************************************************
*
*       _DrawBitLine4BPP
*/
#if (LCD_MAX_LOG_COLORS > 4)
static void _DrawBitLine4BPP(int x, int y, U8 const *p, int Diff, int xsize, const LCD_PIXELINDEX *pTrans)
{
    LCD_PIXELINDEX Pixels = *p;
    int CurrentPixel = Diff;
    x += Diff;

    switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR))
    {
        case 0:
            if (pTrans)
            {
                do
                {
                    int Shift = (1 - CurrentPixel) << 2;
                    int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
                    LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
                    SETPIXEL(x++, y, PixelIndex);

                    if (++CurrentPixel == 2)
                    {
                        CurrentPixel = 0;
                        Pixels = *(++p);
                    }
                } while (--xsize);
            }
            else
            {
                do
                {
                    int Shift = (1 - CurrentPixel) << 2;
                    int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;
                    SETPIXEL(x++, y, Index);

                    if (++CurrentPixel == 2)
                    {
                        CurrentPixel = 0;
                        Pixels = *(++p);
                    }
                } while (--xsize);
            }

            break;

        case LCD_DRAWMODE_TRANS:
            if (pTrans)
            {
                do
                {
                    int Shift = (1 - CurrentPixel) << 2;
                    int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;

                    if (Index)
                    {
                        LCD_PIXELINDEX PixelIndex = *(pTrans + Index);
                        SETPIXEL(x, y, PixelIndex);
                    }

                    x++;

                    if (++CurrentPixel == 2)
                    {
                        CurrentPixel = 0;
                        Pixels = *(++p);
                    }
                } while (--xsize);
            }
            else
            {
                do
                {
                    int Shift = (1 - CurrentPixel) << 2;
                    int Index = (Pixels & (0xF0 >> (4 - Shift))) >> Shift;

                    if (Index)
                    {
                        SETPIXEL(x, y, Index);
                    }

                    x++;

                    if (++CurrentPixel == 2)
                    {
                        CurrentPixel = 0;
                        Pixels = *(++p);
                    }
                } while (--xsize);
            }

            break;
    }
}
#endif

/*********************************************************************
*
*       _DrawBitLine8BPP

#if (LCD_MAX_LOG_COLORS > 16)
static void _DrawBitLine8BPP(int x, int y, U8 const*p, int xsize, const LCD_PIXELINDEX*pTrans) {
  LCD_PIXELINDEX pixel;
  GUI_Context.DrawMode=LCD_DRAWMODE_TRANS;   //mcl add
  if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS)==0) {
    if (pTrans) {
      for (;xsize > 0; xsize--,x++,p++) {
        pixel = *p;
        SETPIXEL(x, y, *(pTrans+pixel));
      }
    } else {
      for (;xsize > 0; xsize--,x++,p++) {
        SETPIXEL(x, y, *p);
      }
    }
  } else {   // Handle transparent bitmap
    if (pTrans) {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
       if (pixel) {    //mcl mod for ttf
               //if (pixel!=255) {       //   MCL
          SETPIXEL(x+0, y, *(pTrans+pixel));
        }
      }
    } else {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        if (pixel) {
          SETPIXEL(x+0, y, pixel);
        }
      }
    }
  }
}
*/
#if (LCD_MAX_LOG_COLORS > 16)

static void _DrawBitLine8BPP(int x, int y, U8 const *p, int xsize, const LCD_PIXELINDEX *pTrans)
{
    LCD_PIXELINDEX pixel;
    GUI_Context.DrawMode = LCD_DRAWMODE_TRANS;   //mcl add

    if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS) == 0)
    {
        if (pTrans)
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                pixel = *p;
                SETPIXEL(x, y, *(pTrans + pixel));
            }
        }
        else
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                SETPIXEL(x, y, *p);
            }
        }
    }
    else       // Handle transparent bitmap
    {
        if (pTrans)    //for shadow
        {
            if (GUI_Context.FontMode == LCD_FONTMODE_32BPPSHADOW)
            {
                U32 *pDest = (U32 *)((U32 *)orange.fbaddr[GUI_Context.SelLayer] + y * (Screen_Context.LCDXSize[GUI_Context.SelLayer]) + x);
                memcpy(pDest, pTrans, (xsize << 2));
            }
            else
            {
                for (; xsize > 0; xsize--, x++, p++)
                {
                    pixel = *p;

                    if (pixel)      //mcl mod for ttf
                    {
                        //if (pixel!=255) {       //   MCL
                        SETPIXEL(x + 0, y, *(pTrans + pixel));
                    }
                }
            }
        }
        else
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                pixel = *p;

                if (pixel)
                {
                    SETPIXEL(x + 0, y, pixel);
                }
            }
        }
    }
}

#if 0
static void _DrawBitLine8BPP_Trans(int x, int y, U8 const *p, int xsize, const LCD_PIXELINDEX *pTrans, LCD_PIXELINDEX index)
{
    LCD_PIXELINDEX pixel;
    //  LCD_PIXELINDEX colorInd;

    /* Handle transparent bitmap */
    if (pTrans)
    {
        for (; xsize > 0; xsize--, x++, p++)
        {
            pixel = *p;

            if (pixel != index)
            {
                SETPIXEL(x + 0, y, *(pTrans + pixel));
            }
        }
    }
    else
    {
        for (; xsize > 0; xsize--, x++, p++)
        {
            pixel = *p;

            if (pixel)
            {
                SETPIXEL(x + 0, y, pixel);
            }
        }
    }
}
#endif
#endif

/*********************************************************************
*
*       _DrawBitLine16BPP
*/
#if (LCD_BITSPERPIXEL > 8)
static void _DrawBitLine16BPP(int x, int y, U16 const *p, int xsize, const LCD_PIXELINDEX *pTrans)
{
    LCD_PIXELINDEX pixel;

    if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS) == 0)
    {
        if (pTrans)
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                pixel = *p;
                SETPIXEL(x, y, *(pTrans + pixel));
            }
        }
        else
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                SETPIXEL(x, y, *p);
            }
        }
    }
    else       /* Handle transparent bitmap */
    {
        if (pTrans)
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                pixel = *p;

                if (pixel)
                {
                    SETPIXEL(x + 0, y, *(pTrans + pixel));
                }
            }
        }
        else
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                pixel = *p;

                if (pixel)
                {
                    SETPIXEL(x + 0, y, pixel);
                }
            }
        }
    }
}
#endif

static int _GetStep(int *pYSize, int *pY)
{
    if (*pYSize > 0)
    {
        *pY = *pYSize - 1;
        return -1;
    }
    else if (*pYSize < 0)
    {
        *pYSize = -*pYSize;
        *pY = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

static int _DrawBitmapARGB(__u32 *pData, int x0, int y0, int XSize, int YSize,
                           __u32 *pAddr, int dstlayer_w, int dstlayer_h, const __u32 *pTrans)
{
    int         x, y, PixelPerLine;
    unsigned long       l_startaddr;                    /*行起始地址*/
    unsigned long       col_addr;                       /*列写入地址*/
    __u32       dir;
    __u8        *pColor;
    __u32       index;
    __bool      trans = EPDK_FALSE;
    __u32       *addr;
    __u32       line_stride;
    __u8        a, r, g, b;
    int         Step;
    Step = _GetStep(&YSize, &y);
    if (!Step)
    {
        return 1;
    }

    dir  = Screen_Context.LCDDirection[GUI_Context.SelLayer];
    PixelPerLine = (32 * XSize + 31) >> 5;

    if (dir == GUI_SCNDIR_ROTATE90 || dir == GUI_SCNDIR_ROTATE270)
    {
        line_stride  = dstlayer_h << 2;
    }
    else
    {
        line_stride  = dstlayer_w << 2;
    }

    for (y = 0; y < YSize; y++)
    {
        /*方向*/
        if (dir == GUI_SCNDIR_ROTATE90)
        {
            if (y == 0)
            {
                l_startaddr = (unsigned long)(pAddr + (dstlayer_w - 1 - x0) * dstlayer_h + (y0));
            }
            else
            {
                l_startaddr += 4;
            }
        }
        else if (dir == GUI_SCNDIR_ROTATE180)
        {
            if (y == 0)
            {
                l_startaddr = (unsigned long)(pAddr + (dstlayer_h - 1 - y0) * dstlayer_w  + (dstlayer_w - 1 - x0));
            }
            else
            {
                l_startaddr -= line_stride;
            }
        }
        else if (dir == GUI_SCNDIR_ROTATE270)
        {
            if (y == 0)
            {
                l_startaddr = (unsigned long)(pAddr + x0 * dstlayer_h  + (dstlayer_h - 1 - y0));
            }
            else
            {
                l_startaddr -= 4;
            }
        }
        else
        {
            if (y == 0)
            {
                l_startaddr  = (unsigned long)(pAddr  + y0 * dstlayer_w + x0);
            }
            else
            {
                l_startaddr  += line_stride;
            }
        }

        for (x = 0; x < XSize; x++)
        {
            if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS) == 0)
            {
                if (pTrans)
                {
                    index   = *(pData + x);
                    pColor  = (__u8 *)(pTrans + index);
                    a = *(pColor + 3);
                    r = *(pColor + 2);
                    g = *(pColor + 1);
                    b = *(pColor + 0);
                }
                else
                {
                    pColor = (__u8 *)(pData + x);
                    a = *(pColor + 3);
                    r = *(pColor + 2);
                    g = *(pColor + 1);
                    b = *(pColor + 0);
                }

                trans   = EPDK_FALSE;
            }
            else
            {
                if (pTrans)
                {
                    index   = *(pData + x);

                    if (index)
                    {
                        pColor  = (__u8 *)(pTrans + index);
                        a = *(pColor + 3);
                        r = *(pColor + 2);
                        g = *(pColor + 1);
                        b = *(pColor + 0);
                    }
                    else
                    {
                        trans   = EPDK_TRUE;
                    }
                }
                else
                {
                    pColor = (__u8 *)(pData + x);
                    a = *(pColor + 3);
                    r = *(pColor + 2);
                    g = *(pColor + 1);
                    b = *(pColor + 0);
                    trans   = EPDK_FALSE;
                }
            }

            if (x == 0)
            {
                col_addr = l_startaddr;
            }
            else
            {
                if (dir == GUI_SCNDIR_ROTATE90)
                {
                    col_addr = col_addr - line_stride;
                }
                else if (dir == GUI_SCNDIR_ROTATE180)
                {
                    col_addr = col_addr - 4;
                }
                else if (dir == GUI_SCNDIR_ROTATE270)
                {
                    col_addr = col_addr + line_stride;
                }
                else
                {
                    col_addr = col_addr + 4;
                }
            }

            addr  = (__u32 *)col_addr;

            if (trans  == EPDK_FALSE)
            {
                *addr = (((__u32)a << 24) | ((__u32)r << 16) | (g << 8) | b);
            }
        }

        pData += PixelPerLine;
    }

    return 0;
}

/*********************************************************************
*
*       _DrawBitLine32BPP
*/
#if (LCD_BITSPERPIXEL > 16)
static void _DrawBitLine32BPP(int x, int y, U32 const *p, int xsize, const LCD_PIXELINDEX *pTrans)
{
    LCD_PIXELINDEX pixel;
__log("_DrawBitLine32BPP");
    //GUI_Context.DrawMode = LCD_DRAWMODE_TRANS;   // mcl
    if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS) == 0)
    {
        if (pTrans)
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                pixel = *p;
                SETPIXEL(x, y, *(pTrans + pixel));
            }
        }
        else
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                SETPIXEL(x, y, *p);
            }
        }
    }
    else
    {
        /* Handle transparent bitmap */
        if (pTrans)
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                pixel = *p;

                if (pixel)
                {
                    SETPIXEL(x + 0, y, *(pTrans + pixel));
                }
            }
        }
        else
        {
            for (; xsize > 0; xsize--, x++, p++)
            {
                pixel = *p;
                // if (pixel) {
                SETPIXEL(x + 0, y, pixel);
                // }
            }
        }
    }
}
#endif

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
void LCD_L0_1_DrawPixel(int x, int y)
{
    if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR)
    {
        XORPIXEL(x, y);
    }
    else
    {
        SETPIXEL(x, y, LCD_COLORINDEX);
    }
}

/*********************************************************************
*
*       LCD_L0_DrawHLine
*/
void LCD_L0_1_DrawHLine(int x0, int y,  int x1)
{
    if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR)
    {
        for (; x0 <= x1; x0++)
        {
            XORPIXEL(x0, y);
        }
    }
    else
    {

        for (; x0 <= x1; x0++)
        {
            SETPIXEL(x0, y, LCD_COLORINDEX);
        }
    }
}




/*********************************************************************
*
*       LCD_L0_DrawVLine
*/
void LCD_L0_1_DrawVLine(int x, int y0,  int y1)
{
    if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR)
    {
        while (y0 <= y1)
        {
            XORPIXEL(x, y0);
            y0++;
        }
    }
    else
    {
        while (y0 <= y1)
        {
            SETPIXEL(x, y0, LCD_COLORINDEX);
            y0++;
        }
    }
}



/*********************************************************************
*
*       LCD_L0_FillRect   // mcl modify for  accelerating clearing speed
void LCD_L0_1_FillRect(int x0, int y0, int x1, int y1) {
  for (; y0 <= y1; y0++) {
    LCD_L0_1_DrawHLine(x0,y0, x1);
  }
}



*/
#if  0
void LCD_L0_1_FillRect(int x0, int y0, int x1, int y1)
{
    for (; y0 <= y1; y0++)
    {
        LCD_L0_1_DrawHLine(x0, y0, x1);
    }
}
#endif

static void LCD_Normal_FillRect(int x0, int y0, int x1, int y1, int fb_width)
{
    int  XSize = x1 - x0 + 1;
    int  YSize = y1 - y0 + 1;
    int  y = 0;
    int  x = 0;
    U8 *pSrc = NULL;
    U8 *pDest = (unsigned char *)(orange.fbaddr[GUI_Context.SelLayer]) + (y0 * (fb_width) << 2) + (x0 << 2);
	//__log("xywh =  %d  %d  %d  %d",x0,y0,x1,y1);
	//__log("orange.fbaddr[GUI_Context.SelLayer] = %p",orange.fbaddr[GUI_Context.SelLayer]);
	//__log("GUI_Context.SelLayer = %d",GUI_Context.SelLayer);

    pSrc = pDest;
	//memset(orange.fbaddr[GUI_Context.SelLayer],0xff,200*200*4);
//	return;
    for (x = 0; x < XSize; x++)
    {
        /*write first line*/
        *((U32 *)pDest + x) = LCD_COLORINDEX;
    }

    pDest = (U8 *)pDest + (fb_width << 2);

    if (YSize > 0)
    {
        for (y = 1; y < YSize; y++)
        {
            /*start by the second line */
            memcpy(pDest,  pSrc,  XSize << 2);   /*copy from first line*/
            pDest = (U8 *)pDest + (fb_width << 2); /*add address by line size*/
        }
    }
}

#if 1   //accelerate  clearing speed   mcl  08.12.09
void LCD_L0_1_FillRect(int x0, int y0, int x1, int y1)
{
    if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == 0)
    {
        LCD_Normal_FillRect(x0, y0, x1, y1, Screen_Context.LCDXSize[GUI_Context.SelLayer]);
    }
    else
    {
        int         x01;
        int         x11;
        int         y01;
        int         y11;
        int         fb_width;
        __u32       dir;

        dir  = Screen_Context.LCDDirection[GUI_Context.SelLayer];

        if (dir == GUI_SCNDIR_ROTATE90)
        {
            x01 = y0;
            y01 = Screen_Context.LCDXSize[GUI_Context.SelLayer] - x1 - 1;
            x11 = y1;
            y11 = Screen_Context.LCDXSize[GUI_Context.SelLayer] - x0 - 1;
            fb_width = Screen_Context.LCDYSize[GUI_Context.SelLayer];
        }
        else if (dir == GUI_SCNDIR_ROTATE180)
        {
            x01 = Screen_Context.LCDXSize[GUI_Context.SelLayer] - x1 - 1;
            y01 = Screen_Context.LCDYSize[GUI_Context.SelLayer] - y1 - 1;
            x11 = Screen_Context.LCDXSize[GUI_Context.SelLayer] - x0 - 1;
            y11 = Screen_Context.LCDYSize[GUI_Context.SelLayer] - y0 - 1;
            fb_width = Screen_Context.LCDXSize[GUI_Context.SelLayer];
        }
        else
        {
            x01 = Screen_Context.LCDYSize[GUI_Context.SelLayer] - y1 - 1;
            y01 = x0;
            x11 = Screen_Context.LCDYSize[GUI_Context.SelLayer] - y0 - 1;
            y11 = x1;
            fb_width = Screen_Context.LCDYSize[GUI_Context.SelLayer];
        }

        LCD_Normal_FillRect(x01, y01, x11, y11, fb_width);
    }
}
#endif

#if  0

/****************************************************

    LCD_L0_DrawBitmap8bpp    mcl add for BMP trans

*/
void LCD_L0_DrawBitmap8bpp(int x0, int y0,
                           int xsize, int ysize,
                           int BitsPerPixel,
                           int BytesPerLine,
                           const U8 *pData, int Diff,
                           const LCD_PIXELINDEX *pTrans,
                           U8 index)
{
    int i;

    /*
       Use DrawBitLineXBPP
    */
    for (i = 0; i < ysize; i++)
    {
        _DrawBitLine8BPP_Trans(x0, i + y0, pData, xsize, pTrans, index);
    }

    pData += BytesPerLine;
}

#endif


/*********************************************************************
*
*       LCD_L0_DrawBitmap
*
*  Purpose:
*    Sets the original position of the virtual display.
*    Has no function at this point with the PC-driver.
*/
void LCD_L0_1_DrawBitmap(int x0, int y0,
                         int xsize, int ysize,
                         int BitsPerPixel,
                         int BytesPerLine,
                         const U8 *pData, int Diff,
                         const LCD_PIXELINDEX *pTrans)
{
    int i;

    if (BitsPerPixel != 32)
    {
        for (i = 0; i < ysize; i++)
        {
            switch (BitsPerPixel)
            {
                case 1:
                    _DrawBitLine1BPP(x0, i + y0, pData, Diff, xsize, pTrans);
                    break;
#if (LCD_MAX_LOG_COLORS > 2)

                case 2:
                    _DrawBitLine2BPP(x0, i + y0, pData, Diff, xsize, pTrans);
                    break;
#endif
#if (LCD_MAX_LOG_COLORS > 4)

                case 4:
                    _DrawBitLine4BPP(x0, i + y0, pData, Diff, xsize, pTrans);
                    break;
#endif
#if (LCD_MAX_LOG_COLORS > 16)

                case 8:
                    if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_ARGB8888
                        || GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_RGB0888)
                    {
                        _DrawBitLine8BPP(x0, i + y0, pData, xsize, pTrans);

                        if (GUI_Context.FontMode == LCD_FONTMODE_32BPPSHADOW)
                        {
                            pTrans = (U32 *)((U32 *) pTrans + xsize); // for shadow
                        }
                    }
                    else
                    {
                        _DrawBitLine8BPP(x0, i + y0, pData, xsize, pTrans);
                    }

                    break;
#endif
#if (LCD_BITSPERPIXEL > 8)

                case 16:
                    _DrawBitLine16BPP(x0, i + y0, (const U16 *)pData, xsize, pTrans);
                    break;
#endif
            }

            pData += BytesPerLine;
        }
    }
    else
    {
        _DrawBitmapARGB((__u32 *)pData, x0, y0, xsize, ysize, (__u32 *)orange.fbaddr[GUI_Context.SelLayer],
                        Screen_Context.LCDXSize[GUI_Context.SelLayer],
                        Screen_Context.LCDYSize[GUI_Context.SelLayer], pTrans);
    }
}

void LCD_L0_1_SetOrg(int x, int y)
{
    GUI_USE_PARA(x);
    GUI_USE_PARA(y);
}

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
#ifdef LCD_VERIFY
int  LCD_GetErrStat(void)
{
    return 0;
}
void LCD_ClrErrStat(void)
{
}
int  LCD_GetErrCnt(void)
{
    return 0;
}
#endif

/*********************************************************************
*
*       LCD_On
*       LCD_Off
*
*  (Not supported in Simulation)
*/
/*
void LCD_Off  (void)
{
      unsigned long temp = LCDCTL_LCDCON1_ADDR;
      temp&=0xfffffffe;
      LCDCTL_LCDCON1_ADDR = temp;
}
void LCD_On (void)
{
        unsigned long temp = LCDCTL_LCDCON1_ADDR;
        temp|=0x00000001;
        LCDCTL_LCDCON1_ADDR = temp;
}
*/
/*********************************************************************
*
*       LCD_L0_SetLUTEntry
*/
//static void  LCDSIM_SetLUTEntry(U8 Pos, LCD_COLOR color, int LayerIndex)
//{ }


void LCD_L0_1_SetLUTEntry(U8 Pos, LCD_COLOR color)
{
    //LCDSIM_SetLUTEntry(Pos, color, LCD_DISPLAY_INDEX);
}





int  LCD_L0_1_Init(void)
{
    //LCD_INIT_CONTROLLER_0();
    return 0;
}


int  LCD_L0_1_CheckInit(void)
{
    return 0;
}



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
void LCD_L0_1_ReInit(void) {}




unsigned LCD_L0_1_GetPixelIndex(int x, int y)
{
    return GETPIXEL(x, y);
}


/*********************************************************************
*
*       LCD_L0_XorPixel
*
*  Purpose:
*    Inverts 1 pixel of the display.
*/


void LCD_L0_1_XorPixel(int x, int y)
{

    XORPIXEL(x, y);
}


/*********************************************************************
*
*       LCD_L0_SetPixelIndex
*
*  Purpose:
*    Writes 1 pixel into the display.
*/


void LCD_L0_1_SetPixelIndex(int x, int y, int ColorIndex)
{
    SETPIXEL(x, y, ColorIndex);
}





#endif


