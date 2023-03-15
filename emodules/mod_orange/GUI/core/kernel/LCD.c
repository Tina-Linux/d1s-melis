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
#define LCD_C

#include "gui_core_kernel_private.h"
#ifdef GUI_DEBUG_LOG
#undef GUI_DEBUG_LOG
#endif
#define GUI_DEBUG_LOG __log
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define RETURN_IF_Y_OUT() \
    if (y < GUI_Context.ClipRect[GUI_Context.SelLayer].y0) return;             \
    if (y > GUI_Context.ClipRect[GUI_Context.SelLayer].y1) return;

#define RETURN_IF_X_OUT() \
    if (x < GUI_Context.ClipRect[GUI_Context.SelLayer].x0) return;             \
    if (x > GUI_Context.ClipRect[GUI_Context.SelLayer].x1) return;

#define CLIP_X() \
    if (x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0) { x0 = GUI_Context.ClipRect[GUI_Context.SelLayer].x0; } \
    if (x1 > GUI_Context.ClipRect[GUI_Context.SelLayer].x1) { x1 = GUI_Context.ClipRect[GUI_Context.SelLayer].x1; }

#define CLIP_Y() \
    if (y0 < GUI_Context.ClipRect[GUI_Context.SelLayer].y0) { y0 = GUI_Context.ClipRect[GUI_Context.SelLayer].y0; } \
    if (y1 > GUI_Context.ClipRect[GUI_Context.SelLayer].y1) { y1 = GUI_Context.ClipRect[GUI_Context.SelLayer].y1; }

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetColorIndex
*/
static int _GetColorIndex(int i)  /* i is 0 or 1 */
{
    return (GUI_Context.DrawMode & LCD_DRAWMODE_REV) ? i - 1 : i;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_SetColorIndex
*/
void LCD_SetColorIndex(int Index)
{
    LCD_ACOLORINDEX[_GetColorIndex(1)] = Index;
}

/*********************************************************************
*
*       LCD_SetBkColorIndex
*/
void LCD_SetBkColorIndex(int Index)
{
    LCD_ACOLORINDEX[_GetColorIndex(0)] = Index;
}

/*********************************************************************
*
*       LCD_SetDrawMode
*/
LCD_DRAWMODE LCD_SetDrawMode(LCD_DRAWMODE dm)
{
    LCD_DRAWMODE OldDM = GUI_Context.DrawMode;

    if ((GUI_Context.DrawMode ^ dm) & LCD_DRAWMODE_REV)
    {
        LCD_PIXELINDEX temp = LCD_BKCOLORINDEX;
        LCD_BKCOLORINDEX    = LCD_COLORINDEX;
        LCD_COLORINDEX      = temp;
    }

    GUI_Context.DrawMode = dm;
    return OldDM;
}

LCD_FONTMODE  LCD_SetFontMode(LCD_FONTMODE fm)
{
    LCD_FONTMODE OldFM = GUI_Context.FontMode;
    GUI_Context.FontMode = fm;
    return OldFM;
}

/*********************************************************************
*
*       LCD_DrawPixel
*/
void LCD_DrawPixel(int x, int y)
{
    RETURN_IF_Y_OUT();
    RETURN_IF_X_OUT();

    if (GUI_Context.DrawMode & LCD_DRAWMODE_XOR)
    {
        LCDDEV_L0_XorPixel(x, y);
    }
    else
    {
        LCDDEV_L0_SetPixelIndex(x, y, LCD_COLORINDEX);
    }
}

/*********************************************************************
*
*       LCD_DrawHLine
*/
void LCD_DrawHLine(int x0, int y,  int x1)
{
    /* Perform clipping and check if there is something to do */
    RETURN_IF_Y_OUT();
    CLIP_X();

    if (x1 < x0)
    {
        return;
    }

    /* Call driver to draw */
    LCDDEV_L0_DrawHLine(x0, y, x1);
}

/*********************************************************************
*
*       LCD_FillRect
*/
void LCD_FillRect(int x0, int y0, int x1, int y1)
{
    /* Perform clipping and check if there is something to do */
    CLIP_X();

    if (x1 < x0)
    {
        return;
    }

    CLIP_Y();

    if (y1 < y0)
    {
        return;
    }
    /* Call driver to draw */
     //__log("LCDDEV_L0_FillRect x0::%d,y0::%d,x1::%d,y1::%d,\n",x0,y0,x1,y1);
    LCDDEV_L0_FillRect(x0, y0, x1, y1);
}

/*********************************************************************
*
*       LCD_DrawBitmap
*/
void LCD_DrawBitmap(int x0, int y0, int xsize, int ysize, int xMul, int yMul,
                    int BitsPerPixel, int BytesPerLine,
                    const U8 GUI_UNI_PTR *pPixel, const LCD_PIXELINDEX *pTrans)
{
    U8  Data = 0;
    int x1, y1;
    /*remove warning*/
    Data = Data;
    /* Handle rotation if necessary */
#if GUI_SUPPORT_ROTATION

    if (GUI_pLCD_APIList)
    {
        GUI_pLCD_APIList->pfDrawBitmap(x0, y0, xsize, ysize, xMul, yMul, BitsPerPixel, BytesPerLine, pPixel, pTrans);
        return;
    }

#endif
    /* Handle the optional Y-magnification */
    y1 = y0 + ysize - 1;
    x1 = x0 + xsize - 1;

    /*  Handle BITMAP without magnification */
    if ((xMul | yMul) == 1)
    {
        int Diff;
        /*  Clip y0 (top) */
        Diff = GUI_Context.ClipRect[GUI_Context.SelLayer].y0 - y0;

        if (Diff > 0)
        {
            ysize -= Diff;

            if (ysize <= 0)
            {
                return;
            }

            y0     = GUI_Context.ClipRect[GUI_Context.SelLayer].y0;
#if GUI_SUPPORT_LARGE_BITMAPS                       /* Required only for 16 bit CPUs if some bitmaps are >64kByte */
            pPixel += (U32)     Diff * (U32)     BytesPerLine;
#else
            pPixel += (unsigned)Diff * (unsigned)BytesPerLine;
#endif
        }

        /*  Clip y1 (bottom) */
        Diff = y1 - GUI_Context.ClipRect[GUI_Context.SelLayer].y1;

        if (Diff > 0)
        {
            ysize -= Diff;

            if (ysize <= 0)
            {
                return;
            }
        }

        /*        Clip right side    */
        Diff = x1 - GUI_Context.ClipRect[GUI_Context.SelLayer].x1;

        if (Diff > 0)
        {
            xsize -= Diff;
        }

        /*        Clip left side ... (The difficult side ...)    */
        Diff = 0;

        if (x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0)
        {
            Diff = GUI_Context.ClipRect[GUI_Context.SelLayer].x0 - x0;
            xsize -= Diff;

            switch (BitsPerPixel)
            {
                case 1:
                    pPixel += (Diff >> 3);
                    x0 += (Diff >> 3) << 3;
                    Diff &= 7;
                    break;

                case 2:
                    pPixel += (Diff >> 2);
                    x0 += (Diff >> 2) << 2;
                    Diff &= 3;
                    break;

                case 4:
                    pPixel += (Diff >> 1);
                    x0 += (Diff >> 1) << 1;
                    Diff &= 1;
                    break;

                case 8:
                    pPixel += Diff;
                    x0 += Diff;
                    Diff = 0;
                    break;

                case 16:
                    pPixel += (Diff << 1);
                    x0 += Diff;
                    Diff = 0;
                    break;
            }
        }

        if (xsize <= 0)
        {
            return;
        }

        LCDDEV_L0_DrawBitmap(x0, y0, xsize, ysize, BitsPerPixel, BytesPerLine, pPixel, Diff, pTrans);
    }
    else
    {
        /**** Handle BITMAP with magnification ***/
        int x, y;
        int yi;
        int Shift = 8 - BitsPerPixel;

        for (y = y0, yi = 0; yi < ysize; yi++, y += yMul, pPixel += BytesPerLine)
        {
            int yMax = y + yMul - 1;

            /* Draw if within clip area (Optimization ... "if" is not required !) */
            if ((yMax >= GUI_Context.ClipRect[GUI_Context.SelLayer].y0) && (y <= GUI_Context.ClipRect[GUI_Context.SelLayer].y1))
            {
                int BitsLeft = 0;
                int xi;
                const U8 GUI_UNI_PTR *pDataLine = pPixel;

                for (x = x0, xi = 0; xi < xsize; xi++, x += xMul)
                {
                    U8  Index;

                    if (!BitsLeft)
                    {
                        Data = *pDataLine++;
                        BitsLeft = 8;
                    }

                    Index = Data >> Shift;
                    Data    <<= BitsPerPixel;
                    BitsLeft -= BitsPerPixel;

                    if (Index || ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS) == 0))
                    {
                        LCD_PIXELINDEX  OldColor = LCD_COLORINDEX;

                        if (pTrans)
                        {
                            LCD_COLORINDEX = *(pTrans + Index);
                        }
                        else
                        {
                            LCD_COLORINDEX = Index;
                        }

                        LCD_FillRect(x, y, x + xMul - 1, yMax);
                        LCD_COLORINDEX = OldColor;
                    }
                }
            }
        }
    }
}
/* mcl add for BMP trans */
void LCD_DrawBitmap_Trans(int x0, int y0, int xsize, int ysize,
                          int BitsPerPixel, int BytesPerLine,
                          const U8 GUI_UNI_PTR *pPixel, const LCD_PIXELINDEX *pTrans, U8 index)
{
    U8  Data = 0;
    int x1, y1;
    int Diff;
    /*remove warning*/
    Data = Data;
    /* Handle rotation if necessary */
    /* Handle the optional Y-magnification */
    y1 = y0 + ysize - 1;
    x1 = x0 + xsize - 1;
    /*  Handle BITMAP without magnification */
    /*  Clip y0 (top) */
    Diff = GUI_Context.ClipRect[GUI_Context.SelLayer].y0 - y0;

    if (Diff > 0)
    {
        ysize -= Diff;

        if (ysize <= 0)
        {
            return;
        }

        y0     = GUI_Context.ClipRect[GUI_Context.SelLayer].y0;
#if GUI_SUPPORT_LARGE_BITMAPS                       /* Required only for 16 bit CPUs if some bitmaps are >64kByte */
        pPixel += (U32)     Diff * (U32)     BytesPerLine;
#else
        pPixel += (unsigned)Diff * (unsigned)BytesPerLine;
#endif
    }

    /*  Clip y1 (bottom) */
    Diff = y1 - GUI_Context.ClipRect[GUI_Context.SelLayer].y1;

    if (Diff > 0)
    {
        ysize -= Diff;

        if (ysize <= 0)
        {
            return;
        }
    }

    /*        Clip right side    */
    Diff = x1 - GUI_Context.ClipRect[GUI_Context.SelLayer].x1;

    if (Diff > 0)
    {
        xsize -= Diff;
    }

    /*        Clip left side ... (The difficult side ...)    */
    Diff = 0;

    if (x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0)
    {
        Diff = GUI_Context.ClipRect[GUI_Context.SelLayer].x0 - x0;
        xsize -= Diff;

        switch (BitsPerPixel)
        {
            case 1:
                pPixel += (Diff >> 3);
                x0 += (Diff >> 3) << 3;
                Diff &= 7;
                break;

            case 2:
                pPixel += (Diff >> 2);
                x0 += (Diff >> 2) << 2;
                Diff &= 3;
                break;

            case 4:
                pPixel += (Diff >> 1);
                x0 += (Diff >> 1) << 1;
                Diff &= 1;
                break;

            case 8:
                pPixel += Diff;
                x0 += Diff;
                Diff = 0;
                break;

            case 16:
                pPixel += (Diff << 1);
                x0 += Diff;
                Diff = 0;
                break;
        }
    }

    if (xsize <= 0)
    {
        return;
    }

    LCDDEV_L0_DrawBitmap_trans(x0, y0, xsize, ysize, BitsPerPixel, BytesPerLine, pPixel, Diff, pTrans, index);
}


/*********************************************************************
*
*       LCD_SetClipRectMax
*/
void LCD_SetClipRectMax(void)
{
    LCDDEV_L0_GetRect(&GUI_Context.ClipRect[GUI_Context.SelLayer]);
}

/*********************************************************************
*
*       LCD_Init
*/
int LCD_Init(void)
{
    int r = 0;
    LCD_SetClipRectMax();   // get buffer size
    r |= LCD_L0_Init();
#if  0   // mcl del
#if GUI_NUM_LAYERS > 1
    r |= LCD_L0_1_Init();
#endif
#if GUI_NUM_LAYERS > 2
    r |= LCD_L0_2_Init();
#endif
#if GUI_NUM_LAYERS > 3
    r |= LCD_L0_3_Init();
#endif
#if GUI_NUM_LAYERS > 4
    r |= LCD_L0_4_Init();
#endif
#endif
    // LCD_InitLUT(); // only for LCD_BITSPERPIXEL<=8
    {
#if GUI_NUM_LAYERS > 1
        GUI_SelectLayer(GUI_Context.SelLayer);  //mcl
#else
        {
#endif
        //  #if (GUI_DEFAULT_BKCOLOR != GUI_INVALID_COLOR)    // mcl  08.12.11
        /* Clear video memory */
        //     LCD_SetDrawMode(GUI_DRAWMODE_REV);
        //   LCD_FillRect(0,0, GUI_XMAX, GUI_YMAX);  // clear fb   only for layer 0;
        // LCD_SetDrawMode(0);
        //#endif
    }
    return r;
}
/*********************************************************************
*
*       LCD_Init_Layer
*/
int LCD_Init_layer(void)
{
    int r = 0;
   //GUI_DEBUG_LOG("\nLCD_Init..x.");
    LCD_SetClipRectMax();
    r |= LCD_L0_Init();
#if  0    //mcl del
#if GUI_NUM_LAYERS > 1
    r |= LCD_L0_1_Init();
#endif
#if GUI_NUM_LAYERS > 2
    r |= LCD_L0_2_Init();
#endif
#if GUI_NUM_LAYERS > 3
    r |= LCD_L0_3_Init();
#endif
#if GUI_NUM_LAYERS > 4
    r |= LCD_L0_4_Init();
#endif
#endif
    LCD_InitLUT();
    {
#if GUI_NUM_LAYERS > 1
        //int i;
        //for (i = GUI_NUM_LAYERS - 1; i >= 0; i--) {
        GUI_SelectLayer(GUI_Context.SelLayer);  //mcl
        // GUI_SelectLayer(0);
#else
        {
#endif
#if (GUI_DEFAULT_BKCOLOR != GUI_INVALID_COLOR)
        LCD_SetDrawMode(0);
#endif
    }
    return r;
}
/*********************************************************************
*
*       LCD_Color2Index
*/
int LCD_Color2Index(LCD_COLOR Color)
{
    return LCDDEV_L0_Color2Index(Color);
}
/*********************************************************************
*
*       LCD_Index2Color
*/
LCD_COLOR LCD_Index2Color(int Index)
{
    return LCDDEV_L0_Index2Color(Index);
}
/*********************************************************************
*
*       LCD_SetBkColor
*/
void LCD_SetBkColor(GUI_COLOR color)
{
    if (GUI_Context.BkColor != color)
    {
        GUI_Context.BkColor = color;
        LCD_SetBkColorIndex(LCD_Color2Index(color));
    }
}
/*********************************************************************
*
*       LCD_SetColor
*/
void LCD_SetColor(GUI_COLOR color)
{
    if (GUI_Context.Color != color)
    {
        GUI_Context.Color = color;
        LCD_SetColorIndex(LCD_Color2Index(color));
    }
}
/*************************** End of file ****************************/
