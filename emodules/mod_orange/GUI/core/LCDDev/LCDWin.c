/*
************************************************************************************************************************
*                                                      Lemon
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : LCDWin.c
*
* Author : Miracle
*
* Version : 1.1.0
*
* Date : 2008.08.13
*
* Description :RGB565
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

#ifndef LCD_WIN_C
#define LCD_WIN_C  1


#include "gui_core_lcddev_private.h"
#include "LCDWin.h"


/*********************************************************************
*
*       Defines
*
**********************************************************************
*/


#if LCD_BITSPERPIXEL <= 8
#define PIXELINDEX U8
#else
#define PIXELINDEX WORD
#endif


#ifdef LCD_ASSERT
#undef LCD_ASSERT
#endif
#define LCD_ASSERT(v) ASSERT(v)

#ifndef LCD_DISPLAY_INDEX
#define LCD_DISPLAY_INDEX  0
#endif
#if  0
#undef LCD_PIXELINDEX     //  mcl add
#define LCD_PIXELINDEX U16

#undef  LCD_BITSPERPIXEL    //mcl add 
#define LCD_BITSPERPIXEL  16

#undef  LCD_NUM_COLORS      //mcl add 
#define LCD_NUM_COLORS  1L << 16

#endif
#define PIXEL_STEP    1



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
#define SETPIXEL(x, y, c) \
    if (!_CheckBound(c)) { \
        LCD2440_SetPixel(x, y, c); \
    }
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
    unsigned short clolorValue = 0;
    unsigned short *pPixelAdress = (unsigned short *)orange.fbaddr[GUI_Context.SelLayer];

    if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == 0)
    {
        pPixelAdress  = pPixelAdress + (y * (Screen_Context.LCDXSize[GUI_Context.SelLayer] << PIXEL_STEP)  + (x << PIXEL_STEP));
        *pPixelAdress    = clolorValue;
    }
    else
    {
        pPixelAdress    = pPixelAdress + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x) * (Screen_Context.LCDYSize[GUI_Context.SelLayer] << PIXEL_STEP)  + (y << PIXEL_STEP));
        clolorValue = *pPixelAdress;
    }

    clolorValue = LCD_NUM_COLORS - 1 - clolorValue;
    *pPixelAdress = clolorValue;
}



/*********************************************************************
*
*       LCD2440_SetPixel

*     static void LCD2440_SetPixel( int  x, int y, unsigned short clolorValue)

{

   unsigned short * pPixelAdress;
    int x1,y1;
    int xSize;
    int ySize;
    if (Screen_Context.LCDDirection==0)
    {
      x1=x;
      y1=y;
      xSize=Screen_Context.LCDXSize;
      ySize=Screen_Context.LCDYSize;
      pPixelAdress  = (unsigned short *)((__u32)lemon.fbaddr + (y1 *xSize<<1)  + (x1<<1) );
      *pPixelAdress = clolorValue;

      }
   else
    {
        x1=y;
        y1=Screen_Context.LCDXSize-1-x;
       xSize=Screen_Context.LCDYSize;
       ySize=Screen_Context.LCDXSize;

    }

    if(( x1 < xSize) && (y1 < ySize))
   {
         pPixelAdress   = (unsigned short *)((__u32)lemon.fbaddr + (y1 *xSize<<1)  + (x1<<1) );
        *pPixelAdress   = clolorValue;
    }
    else
    {
        __inf("LCD oversize!");
    }
 }

*/

//------------------------------------------------------------------------------
/*
#define  LCD2440_SetPixel(x, y, clolorValue)    {  \
                                                    unsigned short * pPixelAdress=(unsigned short *)lemon.fbaddr; \
                                                   if (Screen_Context.LCDDirection==0) \
                                                    { \
                                                        pPixelAdress    += (unsigned short *)(( y) *(Screen_Context.LCDXSize<<1)  + (x)<<1 ); \
                                                        *pPixelAdress   = (clolorValue); \
                                                    } \
                                                    else  \
                                                    { \
                                                        pPixelAdress    += (unsigned short *)((Screen_Context.LCDXSize-1-(x)) *( Screen_Context.LCDYSize<<1)  + (y)<<1 ); \
                                                        *pPixelAdress   = (clolorValue); \
                                                    } \
                                                }
*/
//------------------------------------------------------------------------------

static void LCD2440_SetPixel(int  x, int y, LCD_PIXELINDEX clolorValue)   //mcl

{
    unsigned char *pPixelAdress = (unsigned char *)orange.fbaddr[GUI_Context.SelLayer];
__log("LCD2440_SetPixelvvvvvvvvvvvvvvvvv");
    if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == 0)
    {
        pPixelAdress  = pPixelAdress  + (y * (Screen_Context.LCDXSize[GUI_Context.SelLayer]) << PIXEL_STEP) + (x << PIXEL_STEP) ;
        *(unsigned short *)pPixelAdress   = (unsigned short)clolorValue;
    }
    else
    {
        if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == 1) // anticlockwise  90'
        {
            pPixelAdress   = pPixelAdress + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x) * (Screen_Context.LCDYSize[GUI_Context.SelLayer]) << PIXEL_STEP)  + (y << PIXEL_STEP) ;
            *(unsigned short *)pPixelAdress = (unsigned short)clolorValue;
        }
        else  if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == 3) // clockwise  90'
        {
            pPixelAdress  = pPixelAdress + (x * (Screen_Context.LCDYSize[GUI_Context.SelLayer]) << PIXEL_STEP)  + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y) << PIXEL_STEP) ;
            *(unsigned short *)pPixelAdress  = (unsigned short)clolorValue;
        }
        else if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == 2)
        {
            pPixelAdress    = pPixelAdress + ((Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - y) * (Screen_Context.LCDYSize[GUI_Context.SelLayer]) << PIXEL_STEP)  + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x) << PIXEL_STEP) ;
            *(unsigned short *)pPixelAdress  = (unsigned short)clolorValue;
        }
    }
}



/*********************************************************************
*
*       LCD2440_GetPixel
*/
static LCD_PIXELINDEX LCD2440_GetPixel(int x, int y)
{
    unsigned short clolorValue = 0;
    unsigned short *pPixelAdress = NULL;

    if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == 0)
    {
        pPixelAdress  = (unsigned short *)orange.fbaddr[GUI_Context.SelLayer] + (y * (Screen_Context.LCDXSize[GUI_Context.SelLayer] << PIXEL_STEP)  + (x << PIXEL_STEP));
        *pPixelAdress    = clolorValue;
    }
    else
    {
        pPixelAdress    = (unsigned short *)orange.fbaddr[GUI_Context.SelLayer] + ((Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - x) * (Screen_Context.LCDYSize[GUI_Context.SelLayer] << PIXEL_STEP)  + (y << PIXEL_STEP));
        clolorValue = *pPixelAdress;
    }

    return  clolorValue;
}



/*********************************************************************
*
*       _DrawBitLine1BPP
*/
static void _DrawBitLine1BPP(int x, int y, U8 const *p, int Diff, int xsize, const LCD_PIXELINDEX *pTrans)
{
    unsigned short Index0 = *(pTrans + 0);
    unsigned short Index1 = *(pTrans + 1);
    x += Diff;

    switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR))
    {
        case 0:    /* Write mode */
            do
            {
                //      SETPIXEL(x++,y, (*p & (0x80>>Diff)) ? Index1 : Index0);
                //scott
                unsigned short color = (*p & (0x80 >> Diff)) ? Index1 : Index0 ;
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
    unsigned short Pixels = *p;
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
                    unsigned short PixelIndex = *(pTrans + Index);
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
                        unsigned short PixelIndex = *(pTrans + Index);
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
    unsigned short Pixels = *p;
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
                    unsigned short PixelIndex = *(pTrans + Index);
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
                        unsigned short PixelIndex = *(pTrans + Index);
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
*/
#if (LCD_MAX_LOG_COLORS > 16)
static void _DrawBitLine8BPP(int x, int y, U8 const *p, int xsize, const LCD_PIXELINDEX *pTrans)
{
    unsigned short pixel;
    GUI_Context.DrawMode = LCD_DRAWMODE_TRANS; // mcl add

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

                if (pixel)      //mcl mod for ttf  awf
                {
                    //if (pixel!=255) {       //   MCL
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

static void _DrawBitLine8BPP_Trans(int x, int y, U8 const *p, int xsize, const LCD_PIXELINDEX *pTrans, LCD_PIXELINDEX index)
{
    unsigned short pixel;
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

/*********************************************************************
*
*       _DrawBitLine16BPP
*/
#if (LCD_BITSPERPIXEL > 8)
static void _DrawBitLine16BPP(int x, int y, U16 const *p, int xsize, const LCD_PIXELINDEX *pTrans)
{
    unsigned short pixel;

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
void LCD_L0_DrawPixel(int x, int y)
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
void LCD_L0_DrawHLine(int x0, int y,  int x1)
{
__log("xxxxxxxxxxxx LCD_L0_DrawHLine");
    if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR)
    {
        __log("nnnnnnnn");

        for (; x0 <= x1; x0++)
        {
            XORPIXEL(x0, y);
        }
    }
    else
    {
    __log("llllllll");
        for (; x0 <= x1; x0++)
        {
            SETPIXEL(x0, y, LCD_COLORINDEX);
        }
    }

    //__msg("here!\n");
}





/*********************************************************************
*
*       LCD_L0_DrawVLine
*/
void LCD_L0_DrawVLine(int x, int y0,  int y1)
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
*       LCD_L0_FillRect
*/
void LCD_L0_FillRect(int x0, int y0, int x1, int y1)
{
    for (; y0 <= y1; y0++)
    {
        LCD_L0_DrawHLine(x0, y0, x1);
    }
}



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


/*********************************************************************
*
*       LCD_L0_DrawBitmap
*/
void LCD_L0_DrawBitmap(int x0, int y0,
                       int xsize, int ysize,
                       int BitsPerPixel,
                       int BytesPerLine,
                       const U8 *pData, int Diff,
                       const LCD_PIXELINDEX *pTrans)
{
    int i;

    /*
       Use DrawBitLineXBPP
    */
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
                _DrawBitLine8BPP(x0, i + y0, pData, xsize, pTrans);
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




void LCD_L0_SetOrg(int x, int y)
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


void LCD_L0_SetLUTEntry(U8 Pos, LCD_COLOR color)
{
    //LCDSIM_SetLUTEntry(Pos, color, LCD_DISPLAY_INDEX);
}



/*********************************************************************
*
*       LCD_L0_Init
*/
int  LCD_L0_Init(void)
{
    //LCD_INIT_CONTROLLER_0();
    return 0;
}


int  LCD_L0_CheckInit(void)
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
void LCD_L0_ReInit(void) {}

unsigned LCD_L0_GetPixelIndex(int x, int y)
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
void LCD_L0_XorPixel(int x, int y)
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
void LCD_L0_SetPixelIndex(int x, int y, int ColorIndex)
{
    SETPIXEL(x, y, ColorIndex);
}




#endif


