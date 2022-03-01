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
* File    : GUI_SFT.c
* Purpose : SFT interface in Lemon
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-1-6 11:55:42
**********************************************************************************************************************
*/

#include "GUI_SFT_i.h"
#include "sfte/sfte.h"


#define GUI_FONT_MAGIC   0xa45f086d
#if GUI_COMPILER_SUPPORTS_FP

#ifndef test_bit
#define test_bit( x, y )     ( (x)  &  ( 1U << (y) ) )
#endif

int  SHADOW_SIZE = 0;

static   GUI_COLOR Colors33_b[256] =
{
    0xff000000, 0xff010101, 0xff020202, 0xff030303,
    0xff040404, 0xff050505, 0xff060606, 0xff070707,
    0xff080808, 0xff080808, 0xff0a0a0a,
    0xff0a0a0a, 0xff0c0c0c, 0xff0c0c0c, 0xff0c0c0c,
    0xff0f0f0f, 0xff0f0f0f, 0xff0f0f0f, 0xff121212,
    0xff121212, 0xff121212, 0xff151515, 0xff151515,
    0xff151515, 0xff181818, 0xff181818, 0xff181818,
    0xff181818, 0xff1c1c1c, 0xff1c1c1c, 0xff1c1c1c,
    0xff1c1c1c, 0xff202020, 0xff202020, 0xff202020,
    0xff202020, 0xff242424, 0xff242424, 0xff242424,
    0xff242424, 0xff282828, 0xff282828, 0xff282828,
    0xff282828, 0xff282828, 0xff2d2d2d, 0xff2d2d2d,
    0xff2d2d2d, 0xff2d2d2d, 0xff2d2d2d, 0xff323232,
    0xff323232, 0xff323232, 0xff323232, 0xff323232,
    0xff373737, 0xff373737, 0xff373737, 0xff373737,
    0xff373737, 0xff3c3c3c, 0xff3c3c3c, 0xff3c3c3c,
    0xff3c3c3c, 0xff3c3c3c, 0xff3c3c3c, 0xff424242,
    0xff424242, 0xff424242, 0xff424242, 0xff424242,
    0xff424242, 0xff484848, 0xff484848, 0xff484848,
    0xff484848, 0xff484848, 0xff484848, 0xff4e4e4e,
    0xff4e4e4e, 0xff4e4e4e, 0xff4e4e4e, 0xff4e4e4e,
    0xff4e4e4e, 0xff545454, 0xff545454, 0xff545454,
    0xff545454, 0xff545454, 0xff545454, 0xff545454,
    0xff5b5b5b, 0xff5b5b5b, 0xff5b5b5b, 0xff5b5b5b,
    0xff5b5b5b, 0xff5b5b5b, 0xff5b5b5b, 0xff626262,
    0xff626262, 0xff626262, 0xff626262, 0xff626262,
    0xff626262, 0xff626262, 0xff696969, 0xff696969,
    0xff696969, 0xff696969, 0xff696969, 0xff696969,
    0xff696969, 0xff707070, 0xff707070, 0xff707070,
    0xff707070, 0xff707070, 0xff707070, 0xff707070,
    0xff707070, 0xff787878, 0xff787878, 0xff787878,
    0xff787878, 0xff787878, 0xff787878, 0xff787878,
    0xff7f7f7f, 0xff7f7f7f, 0xff818181, 0xff818181,
    0xff818181, 0xff818181, 0xff818181, 0xff818181,
    0xff818181, 0xff888888, 0xff888888, 0xff888888,
    0xff888888, 0xff888888, 0xff888888, 0xff888888,
    0xff888888, 0xff909090, 0xff909090, 0xff909090,
    0xff909090, 0xff909090, 0xff909090, 0xff909090,
    0xff979797, 0xff979797, 0xff979797, 0xff979797,
    0xff979797, 0xff979797, 0xff979797, 0xff9e9e9e,
    0xff9e9e9e, 0xff9e9e9e, 0xff9e9e9e, 0xff9e9e9e,
    0xff9e9e9e, 0xff9e9e9e, 0xffa5a5a5, 0xffa5a5a5,
    0xffa5a5a5, 0xffa5a5a5, 0xffa5a5a5, 0xffa5a5a5,
    0xffa5a5a5, 0xffacacac, 0xffacacac, 0xffacacac,
    0xffacacac, 0xffacacac, 0xffacacac, 0xffb2b2b2,
    0xffb2b2b2, 0xffb2b2b2, 0xffb2b2b2, 0xffb2b2b2,
    0xffb2b2b2, 0xffb8b8b8, 0xffb8b8b8, 0xffb8b8b8,
    0xffb8b8b8, 0xffb8b8b8, 0xffb8b8b8, 0xffbebebe,
    0xffbebebe, 0xffbebebe, 0xffbebebe, 0xffbebebe,
    0xffbebebe, 0xffc4c4c4, 0xffc4c4c4, 0xffc4c4c4,
    0xffc4c4c4, 0xffc4c4c4, 0xffc9c9c9, 0xffc9c9c9,
    0xffc9c9c9, 0xffc9c9c9, 0xffc9c9c9, 0xffcecece,
    0xffcecece, 0xffcecece, 0xffcecece, 0xffcecece,
    0xffd3d3d3, 0xffd3d3d3, 0xffd3d3d3, 0xffd3d3d3,
    0xffd3d3d3, 0xffd8d8d8, 0xffd8d8d8, 0xffd8d8d8,
    0xffd8d8d8, 0xffdcdcdc, 0xffdcdcdc, 0xffdcdcdc,
    0xffdcdcdc, 0xffe0e0e0, 0xffe0e0e0, 0xffe0e0e0,
    0xffe0e0e0, 0xffe4e4e4, 0xffe4e4e4, 0xffe4e4e4,
    0xffe4e4e4, 0xffe8e8e8, 0xffe8e8e8, 0xffe8e8e8,
    0xffebebeb, 0xffebebeb, 0xffebebeb, 0xffeeeeee,
    0xffeeeeee, 0xffeeeeee, 0xfff1f1f1, 0xfff1f1f1,
    0xfff1f1f1, 0xfff4f4f4, 0xfff4f4f4, 0xfff6f6f6,
    0xfff6f6f6, 0xfff8f8f8, 0xfff9f9f9, 0xfffafafa,
    0xfffbfbfb, 0xfffcfcfc, 0xfffdfdfd, 0xfffefefe,
    0xffffffff
};


static GUI_CONST_STORAGE GUI_LOGPALETTE Pal33gray =
{
    256,    /* number of entries */
    0,  /* No transparency */
    &Colors33_b[0]
};


/*pal  for aRGB*/
static  GUI_COLOR   Colors_aRGB[256];
static  U32         fontBuffer[50 * 50]; // for shadow
static  U8          MonoFontBuf[50 * 50];
static  U8          magbuffer[200 * 200];

/*********************************************************************
*
*       _CreateARGBPal
*/
static char _CreateARGBPal(GUI_COLOR *pColor)
{
    int i = 0;

    for (i = 0; i < 256; i++)
    {
        *(pColor + i) = LCD_Color2Index((i << 24) | (GUI_Context.Color & 0x00ffffff));
    }

    return  0;
}

static void insert(char *src, int WIDTH, int HEIGHT, int n)
{
    int i, j;
    int k;
    char *dest = magbuffer;
    //char *dest = (char *)esMEMS_Calloc(0,WIDTH *n ,HEIGHT*n );
    orange_memset(dest, 0, WIDTH * n * HEIGHT * n);

    for (i = 0; i < HEIGHT; i++)
    {
        for (j = 0; j < WIDTH; j++)
        {
            *(char *)(dest + (i * n * WIDTH * n) + (j * n)) = *(src + i * WIDTH + j);

            for (k = 1; k < n; k++)
            {
                *(char *)(dest + (i * n * WIDTH * n) + (j * n + k)) = *(src + i * WIDTH + j);
                // *(dest+(i*n+k)*WIDTH+(j*n)) = src[i][j];
            }
        }
    }

    for (i = 0; i < HEIGHT * n; i++)
    {
        if (i % n)
        {
            for (j = 0; j < WIDTH * n; j++)
            {
                *(dest + i * WIDTH * n + j) = *(dest + (i - 1) * WIDTH * n + j);
            }
        }
    }
}

/*********************************************************************
*
*       _RequestGlyph
*
* Purpose:
*   Draws a glyph (if DoRender == 1) and returns its width
*/
static int _RequestGlyph(U16P c, unsigned DoRender)
{
    int             bufferSize = 0;
    int             i;
    int             j;
    U32             num;
    I32             pitch;
    __u8            temp;
    __s32           pixelmode;
    __u8            *pPixel;
    int             rows;
    int             width;
    int r = 0;
    const LCD_PIXELINDEX  *pTrans = NULL;  //mcl add
    int rb = 0;
    rb = SFTE_Get_Glyph((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle), c);
    pPixel = ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.buffer;
    rows = ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.rows;
    width = ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.width;
    pitch = ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.pitch;
    pixelmode = ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.pixel_mode;

    //GUI_SetDrawMode(0);
    if (!rb)
    {
        if (DoRender)
        {
            switch (GUI_LayerInfo.colorMode[GUI_Context.SelLayer])
            {
                case PIXEL_COLOR_ARGB8888:
                case PIXEL_COLOR_RGB0888:
                    if (GUI_Context.FontMode == LCD_FONTMODE_32BPPSHADOW)
                    {
                        bufferSize = ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.rows * ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.width;    //mcl  2008.12.15

                        for (i = 0; i < bufferSize; i++)
                        {
                            U8 pixel = *(((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.buffer + i);
                            fontBuffer[i] = LCD_Color2Index((pixel << 24) | (GUI_Context.Color & 0x00ffffff));
                        }

                        shadow(fontBuffer, ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.width, ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.rows);
                        pTrans = fontBuffer;
                        SHADOW_SIZE = 2;
                    }
                    else
                    {
                        SHADOW_SIZE = 0;
                        _CreateARGBPal(Colors_aRGB);   //  mcl  2008.12.02
                        pTrans = Colors_aRGB;
                    }

                    break;

                case PIXEL_MONO_8BPP:
                {
                    SHADOW_SIZE = 0;

                    switch (GUI_Context.FontMode)
                    {
                        case LCD_FONTMODE_8BPP32:
                        {
                            pTrans = NULL;  // set  pTrans NULL to use user's palette
                            bufferSize =  rows * width;

                            if (pixelmode == SFTE_PIXEL_MODE_GRAY)
                            {
                                for (i = 0; i < bufferSize; i++)      // mcl add for  8 bpp  fonts
                                {
                                    U8 Pixel = *(pPixel + i) ;

                                    if (Pixel > 15)
                                    {
                                        MonoFontBuf[i] = Pixel / 16 + GUI_Context.Color; // last 32 palette reserved  for fonts
                                    }
                                    else
                                    {
                                        MonoFontBuf[i] = 0;
                                    }
                                }
                            }
                            else
                            {
                                pTrans = NULL;

                                for (i = 0;  i < rows; i++)
                                {
                                    for (j = 0;  j < width;  j++)
                                    {
                                        num = i * width + j;

                                        if (pitch > 0)
                                        {
                                            temp = pPixel[ i * pitch + (j >> 3) ];
                                        }
                                        else
                                        {
                                            temp = pPixel[(rows - 1 - i) * -(pitch) + (j >> 3) ];
                                        }

                                        temp = test_bit(temp, 7 - (j & 0x7));

                                        if (temp == 0)
                                        {
                                            MonoFontBuf[num] = 0; // last 32 palette reserved  for fonts
                                        }
                                        else
                                        {
                                            MonoFontBuf[num] = 0x0f + GUI_Context.Color; // last 32 palette reserved  for fonts
                                        }
                                    }
                                }
                            }
                        }
                        break;

                        case LCD_FONTMODE_8BPP32_FRAME_MAG:
                        {
                            int bufferSize = 0;
                            int i;
                            pTrans                  = NULL;  // set  pTrans NULL to use user's palette
                            SHADOW_SIZE             = 0;
                            insert(pPixel, width, rows, 2);
                            //((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.buffer = magbuffer;
                            width *= 2;
                            rows  *= 2;
                            pitch *= 2;
                            bufferSize =  rows * width;
                            pPixel  = magbuffer;

                            for (i = 0; i < bufferSize; i++)      // mcl add for  8 bpp  fonts
                            {
                                U8 Pixel = *(pPixel + i);

                                if (Pixel > 15)
                                {
                                    *(pPixel + i) = Pixel / 16 + GUI_Context.Color; // last 32 palette reserved  for fonts
                                }
                                else
                                {
                                    *(pPixel + i) = 0;
                                }
                            }

                            shadow_Dilation(pPixel,  GUI_Context.pAFont->Font_EXT.pFrame8bpp32,  width, rows);
                        }
                        break;

                        case LCD_FONTMODE_8BPP32_FRAME:
                        {
                            pTrans = NULL;  // set  pTrans NULL to use user's palette
                            bufferSize =  rows * width;

                            if (pixelmode == SFTE_PIXEL_MODE_GRAY)
                            {
                                for (i = 0; i < bufferSize; i++)      // mcl add for  8 bpp  fonts
                                {
                                    U8 Pixel = *(pPixel + i) ;

                                    if (Pixel > 15)
                                    {
                                        MonoFontBuf[i] = Pixel / 16 + GUI_Context.Color; // last 32 palette reserved  for fonts
                                    }
                                    else
                                    {
                                        MonoFontBuf[i] = 0;
                                    }
                                }

                                shadow_Dilation((unsigned char *)MonoFontBuf,  GUI_Context.pAFont->Font_EXT.pFrame8bpp32,  width, rows);
                            }
                            else
                            {
                                pTrans = NULL;

                                for (i = 0;  i < rows; i++)
                                {
                                    for (j = 0;  j < width;  j++)
                                    {
                                        num = i * width + j;

                                        if (pitch > 0)
                                        {
                                            temp = pPixel[ i * pitch + (j >> 3) ];
                                        }
                                        else
                                        {
                                            temp = pPixel[(rows - 1 - i) * -(pitch) + (j >> 3) ];
                                        }

                                        temp = test_bit(temp, 7 - (j & 0x7));

                                        if (temp == 0)
                                        {
                                            MonoFontBuf[num] = 0; // last 32 palette reserved  for fonts
                                        }
                                        else
                                        {
                                            MonoFontBuf[num] = 0x0f + GUI_Context.Color;
                                        }
                                    }
                                }

                                shadow_Dilation((unsigned char *)MonoFontBuf,  GUI_Context.pAFont->Font_EXT.pFrame8bpp32,  width, rows);
                            }
                        }
                        break;

                        case LCD_FONTMODE_8BPP256:
                        {
                            pTrans = NULL;  // set  pTrans NULL to use user's palette

                            if (pixelmode == SFTE_PIXEL_MODE_GRAY)
                            {
                                pTrans = NULL;
                            }
                            else
                            {
                                pTrans = NULL;

                                for (i = 0;  i < rows; i++)
                                {
                                    for (j = 0;  j < width;  j++)
                                    {
                                        num = i * width + j;

                                        if (pitch > 0)
                                        {
                                            temp = pPixel[ i * pitch + (j >> 3) ];
                                        }
                                        else
                                        {
                                            temp = pPixel[(rows - 1 - i) * -(pitch) + (j >> 3) ];
                                        }

                                        temp = test_bit(temp, 7 - (j & 0x7));

                                        if (temp == 0)
                                        {
                                            MonoFontBuf[num] = 0; // last 32 palette reserved  for fonts
                                        }
                                        else
                                        {
                                            MonoFontBuf[num] = 255;
                                        }
                                    }
                                }
                            }
                        }
                        break;

                        case  LCD_FONTMODE_8BPP128_1:
                        {
                            pTrans = NULL;  // set  pTrans NULL to use user's palette
                            bufferSize =  rows * width;

                            if (pixelmode == SFTE_PIXEL_MODE_GRAY)
                            {
                                for (i = 0; i < bufferSize; i++)      // mcl add for  8 bpp  fonts
                                {
                                    U8 Pixel = *(pPixel + i) ;
                                    MonoFontBuf[i] = Pixel / 2;
                                }
                            }
                            else
                            {
                                pTrans = NULL;

                                for (i = 0;  i < rows; i++)
                                {
                                    for (j = 0;  j < width;  j++)
                                    {
                                        num = i * width + j;

                                        if (pitch > 0)
                                        {
                                            temp = pPixel[ i * pitch + (j >> 3) ];
                                        }
                                        else
                                        {
                                            temp = pPixel[(rows - 1 - i) * -(pitch) + (j >> 3) ];
                                        }

                                        temp = test_bit(temp, 7 - (j & 0x7));

                                        if (temp == 0)
                                        {
                                            MonoFontBuf[num] = 0; // last 32 palette reserved  for fonts
                                        }
                                        else
                                        {
                                            MonoFontBuf[num] = 127;
                                        }
                                    }
                                }
                            }
                        }
                        break;

                        case LCD_FONTMODE_8BPP128_2:
                        {
                            pTrans = NULL;  // set  pTrans NULL to use user's palette
                            bufferSize =  rows * width;

                            if (pixelmode == SFTE_PIXEL_MODE_GRAY)
                            {
                                for (i = 0; i < bufferSize; i++)      // mcl add for  8 bpp  fonts
                                {
                                    U8 Pixel = *(pPixel + i) ;
                                    MonoFontBuf[i] = Pixel / 2 + 128;
                                }
                            }
                            else
                            {
                                for (i = 0;  i < rows; i++)
                                {
                                    for (j = 0;  j < width;  j++)
                                    {
                                        num = i * width + j;

                                        if (pitch > 0)
                                        {
                                            temp = pPixel[ i * pitch + (j >> 3) ];
                                        }
                                        else
                                        {
                                            temp = pPixel[(rows - 1 - i) * -(pitch) + (j >> 3) ];
                                        }

                                        temp = test_bit(temp, 7 - (j & 0x7));

                                        if (temp == 0)
                                        {
                                            MonoFontBuf[num] = 0; // last 32 palette reserved  for fonts
                                        }
                                        else
                                        {
                                            MonoFontBuf[num] = 255;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
                break;

                case PIXEL_MONO_2BPP:
                    SHADOW_SIZE = 0;
                    pTrans = NULL;
                    break;

                case PIXEL_COLOR_RGB565:
                    SHADOW_SIZE = 0;
                    pTrans = LCD_GetpPalConvTable(&Pal33gray);
                    break;

                default:
                    __msg(" wrong color mode\n");
            }

            if (((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.pixel_mode == SFTE_PIXEL_MODE_GRAY)
            {
                if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_MONO_8BPP  && GUI_Context.FontMode == LCD_FONTMODE_8BPP32_FRAME)
                {
                    /* Rendering cache data using the bitmap routine */
                    LCD_DrawBitmap(GUI_Context.DispPosX[GUI_Context.SelLayer] + ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_left,
                                   GUI_Context.DispPosY[GUI_Context.SelLayer] - ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_top + GUI_Context.pAFont->Baseline,
                                   width + 4,
                                   rows + 4,
                                   1, 1, 8,     // mcl change 1 to 8
                                   pitch + 4,
                                   GUI_Context.pAFont->Font_EXT.pFrame8bpp32,
                                   pTrans);
                }
                else if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_MONO_8BPP  && (GUI_Context.FontMode == LCD_FONTMODE_8BPP32 || GUI_Context.FontMode == LCD_FONTMODE_8BPP128_1 || GUI_Context.FontMode == LCD_FONTMODE_8BPP128_2))
                {
                    /* Rendering cache data using the bitmap routine */
                    LCD_DrawBitmap(GUI_Context.DispPosX[GUI_Context.SelLayer] + ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_left,
                                   GUI_Context.DispPosY[GUI_Context.SelLayer] - ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_top + GUI_Context.pAFont->Baseline,
                                   width + SHADOW_SIZE,
                                   rows + SHADOW_SIZE,
                                   1, 1, 8,     // mcl change 1 to 8
                                   pitch + SHADOW_SIZE * 4,
                                   (U8 *)MonoFontBuf,
                                   pTrans);
                }
                else
                {
                    /* Rendering cache data using the bitmap routine */
                    LCD_DrawBitmap(GUI_Context.DispPosX[GUI_Context.SelLayer] + ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_left,
                                   GUI_Context.DispPosY[GUI_Context.SelLayer] - ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_top + GUI_Context.pAFont->Baseline,
                                   width + SHADOW_SIZE,
                                   rows + SHADOW_SIZE,
                                   1, 1, 8,     // mcl change 1 to 8
                                   pitch + SHADOW_SIZE * 4,
                                   ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.buffer,
                                   pTrans);
                }
            }

            if (((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.pixel_mode == SFTE_PIXEL_MODE_MONO)
            {
                if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_MONO_8BPP)
                {
                    if (GUI_Context.FontMode == LCD_FONTMODE_8BPP32_FRAME)
                    {
                        /* Rendering cache data using the bitmap routine */
                        LCD_DrawBitmap(GUI_Context.DispPosX[GUI_Context.SelLayer] + ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_left,
                                       GUI_Context.DispPosY[GUI_Context.SelLayer] - ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_top + GUI_Context.pAFont->Baseline,
                                       width + 4,
                                       rows + 4,
                                       1, 1, 8,     // mcl change 1 to 8
                                       width + 4,
                                       GUI_Context.pAFont->Font_EXT.pFrame8bpp32,
                                       pTrans);
                    }
                    else
                    {
                        /* Rendering cache data using the bitmap routine */
                        LCD_DrawBitmap(GUI_Context.DispPosX[GUI_Context.SelLayer] + ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_left,
                                       GUI_Context.DispPosY[GUI_Context.SelLayer] - ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_top + GUI_Context.pAFont->Baseline,
                                       width,
                                       rows,
                                       1, 1, 8,     // mcl change 1 to 8
                                       width,
                                       (U8 *)MonoFontBuf,
                                       pTrans);
                    }
                }
                else
                {
                    /* Rendering cache data using the bitmap routine */
                    LCD_DrawBitmap(GUI_Context.DispPosX[GUI_Context.SelLayer] + ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_left,
                                   GUI_Context.DispPosY[GUI_Context.SelLayer] - ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap_top + GUI_Context.pAFont->Baseline,
                                   width,
                                   rows,
                                   1, 1, 1,     // mcl change 1 to 8
                                   pitch,
                                   pPixel,
                                   &LCD_BKCOLORINDEX);
                }
            }
        }

        r = ((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->advance.x >> 6 ;

        if (GUI_Context.FontMode == LCD_FONTMODE_32BPPMAG2 || GUI_Context.FontMode == LCD_FONTMODE_8BPP32_FRAME_MAG)
        {
            r = r * 2;
        }
    }
    else
    {
        r = (GUI_Context.pAFont)->YSize;

        if (GUI_Context.FontMode == LCD_FONTMODE_32BPPMAG2 || GUI_Context.FontMode == LCD_FONTMODE_8BPP32_FRAME_MAG)
        {
            r = r * 2;
        }
    }

    return r;
}


static int _GlyphGetXDist(U16P c, unsigned DoRender)
{
    int rb = 0;
    rb = SFTE_Get_XAdvance((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle), c);
    return rb;
}


/*********************************************************************
*
*       _DispChar
*/
static void _DispChar(U16P c)
{
    int xDist;
    // U16P unicode;

    if (c == 0xffff)
    {
        xDist = _RequestGlyph(0x20, 1) + SHADOW_SIZE;  //need edit

        if (xDist >= 0)
        {
            GUI_Context.DispPosX[GUI_Context.SelLayer] += xDist;
        }

        return ;
    }
    else
    {
        xDist = _RequestGlyph(c, 1) + SHADOW_SIZE;  //need edit

        if (xDist >= 0)
        {
            GUI_Context.DispPosX[GUI_Context.SelLayer] += xDist;
        }
    }
}



/*********************************************************************
*
*       _ClearLine
*
* Purpose:
*   If text should be rendered not in transparent mode first the whole line
*   needs to be cleared, because internally the characters always are drawn in
*   transparent mode to be sure, that also compound characters are drawn well.
*/
static void _ClearLine(const char GUI_UNI_PTR *s, int Len)
{
    // do not clear rect to write inorder to save wrting time       mcl
    /*  U16 c;
      int xDist, yDist, xSize, x0, y0;
      LCD_COLOR OldColor;
      U16P unicode;
      OldColor = GUI_GetColor();
      GUI_SetColor((GUI_Context.TextMode & GUI_TM_REV) ? GUI_GetColor() : GUI_GetBkColor());
      xDist = 0;
      yDist = GUI_Context.pAFont->YDist * GUI_Context.pAFont->YMag;
      x0    = GUI_Context.DispPosX[GUI_Context.SelLayer];
      y0    = GUI_Context.DispPosY[GUI_Context.SelLayer];
      while (--Len >= 0) {
        c     = GUI_UC__GetCharCodeInc(&s);
        if( c < 128 )
            {
                unicode = (U16P)c;

            }
            else
            {
                unicode = GB2312_to_Unicode( c );

            }
         xSize = _RequestGlyph(unicode, 0);  //      need  edit     // mcl
        if (xSize >= 0) {
          xDist += xSize;
        }
      }
      LCD_FillRect(x0, y0, x0 + xDist - 1, y0 + yDist - 1);

      GUI_SetColor(OldColor);
    */
}



/*********************************************************************
*
*       _DispLine
*/
static void _DispLine(const char GUI_UNI_PTR *s, int Len)
{
    U16 Char;
    int OldMode;

    /* Clear if not transparency mode has been selected */
    if (!(GUI_Context.TextMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)))
    {
        _ClearLine(s, Len);
    }

    /* Draw characters always transparent */
    OldMode = GUI_Context.TextMode;
    GUI_Context.DrawMode |= GUI_DM_TRANS;

    while (--Len >= 0)
    {
        Char = GUI_UC__GetCharCodeInc(&s);
        GUI_Context.pAFont->pfDispChar(Char);
    }

    GUI_Context.DrawMode = OldMode;
}


/*********************************************************************
*
*       _GetCharDistX
*/
static int _GetCharDistX(U16P c)
{
    int xDist;
    // U16P unicode;
#if 0

    //for chinese
    if (c < 128)
    {
        unicode = (U16P)c;
    }
    else
    {
        unicode = GB2312_to_Unicode(c);
    }

#endif
    xDist = _GlyphGetXDist(c, 0) ;   //  only get x size
    return (xDist >= 0) ? (xDist + SHADOW_SIZE) : 0;
}


/*********************************************************************
*
*       _GetFontInfo
*/
static void _GetFontInfo(const GUI_FONT GUI_UNI_PTR *pFont, GUI_FONTINFO *pfi)
{
    pfi->Baseline = pFont->Baseline;
    pfi->LHeight  = pFont->LHeight;
    pfi->CHeight  = pFont->CHeight;
    pfi->Flags    = GUI_FONTINFO_FLAG_PROP;
}

/*********************************************************************
*
*       _IsInFont
*/
static char _IsInFont(const GUI_FONT GUI_UNI_PTR *pFont, U16 c)
{
    //   suppose is in font
    return  1;
}


/*********************************************************************
*
*       _APIList
*/
static const tGUI_ENC_APIList _APIList =
{
    NULL,
    NULL,
    _DispLine
};



/*********************************************************************
*
*       GUI_SFT_CreateFont
*/

GUI_FONT *GUI_SFT_CreateFont(unsigned int  pixelSize, const char  *font_file)
{
    GUI_FONT   *pSFTFont;
	
    pSFTFont = (GUI_FONT *) esMEMS_Malloc(0, sizeof(GUI_FONT));

    if (!pSFTFont)
    {
        __log("malloc font fail, gui_sft.c\n");
        return NULL;
    }

    pSFTFont->Font_EXT.magic = GUI_FONT_MAGIC;
    //(SFTE_Face)( pSFTFont->Font_EXT.faceHandle) = SFTE_New_Face(font_file);
    pSFTFont->Font_EXT.faceHandle = (unsigned long)SFTE_New_Face(font_file);
    pSFTFont->XMag = 1;
    pSFTFont->YMag = 1;
    /* Set function pointers */
    pSFTFont->p.pFontData    = NULL;
    pSFTFont->pfDispChar     = _DispChar;
    pSFTFont->pfGetCharDistX = _GetCharDistX;
    pSFTFont->pfGetFontInfo  = _GetFontInfo;
    pSFTFont->pfIsInFont     = _IsInFont;
    pSFTFont->pafEncode      = &_APIList;

    if (SFTE_Set_Pixel_Sizes((SFTE_Face)(pSFTFont->Font_EXT.faceHandle), pixelSize) == -1)
    {
        __log("set pixel size fail\n");
        return NULL;
    }

    if (SFTE_Get_Index_Table((SFTE_Face)(pSFTFont->Font_EXT.faceHandle)) == -1) //added by Derek
    {
        __inf("get index table fail\n");
        return NULL;
    }

    /* Calculate baseline and vertical size */
    pSFTFont->Baseline = ((SFTE_Face)(pSFTFont->Font_EXT.faceHandle))->size->metrics.ascender >> 6;
    pSFTFont->YSize    = pSFTFont->Baseline - (((SFTE_Face)(pSFTFont->Font_EXT.faceHandle))->size->metrics.descender >> 6);
    pSFTFont->YDist    = pSFTFont->YSize;
    pSFTFont->LHeight = ((SFTE_Face)(pSFTFont->Font_EXT.faceHandle))->glyph->metrics.horiBearingY >> 6;
    pSFTFont->CHeight = ((SFTE_Face)(pSFTFont->Font_EXT.faceHandle))->glyph->metrics.height  >> 6;
    pSFTFont->Font_EXT.pFrame8bpp32 = (U8 *)esMEMS_Malloc(0, (pixelSize + 8) * (pixelSize + 8));
    return  pSFTFont;
}

/*********************************************************************
*
*       GUI_TTF_Done
*/
void GUI_SFT_ReleaseFont(GUI_FONT *pFont)
{
    if (GUI_Context.pAFont == pFont)
    {
        __inf("Error: the font is inuse, cann't be deleted.(GUI_SFT.c)\n");
        return;
    }

    SFTE_Done_Face((SFTE_Face)(pFont->Font_EXT.faceHandle));
    esMEMS_Mfree(0, pFont->Font_EXT.pFrame8bpp32);
    pFont->Font_EXT.pFrame8bpp32 = NULL;
    esMEMS_Mfree(0, pFont);
    pFont = NULL;
}

#else

void GUI_SFT_C(void);
void GUI_SFT_C(void) {} /* Avoid empty object files */

#endif



