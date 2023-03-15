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
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       static code
*
**********************************************************************
*/



/*********************************************************************
*
*       GL_DrawBitmapEx_Trans
*/
static void GL_DrawBitmapEx_Trans(const GUI_BITMAP GUI_UNI_PTR *pBitmap, int x0, int y0,
                                  int xCenter, int yCenter, int xMag, int yMag, int color)
{
    LCD_PIXELINDEX Index, IndexPrev = 0;
    LCD_COLOR Color;
    int x, y, xi, yi, xSize, ySize, xAct, xStart, xMagAbs, xiMag, yMin, yMax, yEnd, yPrev, yStep;
    char Cached, HasTrans = 0;
    /* Use clipping rect to reduce calculation */
    yMin = GUI_Context.ClipRect[GUI_Context.SelLayer].y0;
    yMax = GUI_Context.ClipRect[GUI_Context.SelLayer].y1;
    /* Init some values */
    xSize    = pBitmap->XSize;
    ySize    = pBitmap->YSize;
    xMagAbs  = ((xMag < 0) ? -xMag : xMag);
    x0      -= (I32)((xMag < 0) ? xSize - xCenter - 1 : xCenter) * (I32)(xMagAbs) / (I32)(1000);
    yEnd     = y0 + GUI__DivideRound32(((I32)(-yCenter) * (I32)(yMag)), 1000);
    yPrev    = yEnd + 1;
    yStep = (yMag < 0) ? -1 : 1;

    if (pBitmap->pPal)
    {
        if (pBitmap->pPal->HasTrans)
        {
            HasTrans = 1;
        }
    }

    for (yi = 0; yi < ySize; yi++)
    {
        y = yEnd;
        yEnd = y0 + GUI__DivideRound32(((I32)(yi + 1 - yCenter) * (I32)(yMag)), 1000);

        if (y != yPrev)
        {
            yPrev = y;

            do
            {
                if ((y >= yMin) && (y <= yMax))
                {
                    xStart = -1;
                    x      =  0;
                    xiMag  =  0;
                    Cached =  0;

                    for (xi = 0; xi < xSize; xi++)
                    {
                        xiMag += xMagAbs;

                        if (xiMag >= 1000)
                        {
                            xAct  = (xMag > 0) ? xi : xSize - xi - 1;
                            Index = GUI_GetBitmapPixelIndex(pBitmap, xAct, yi);

                            if (Index != IndexPrev || xStart == -1)
                            {
                                // 2008/07/11 11:34:34  mcl changes  if (((Index == 0) && HasTrans)||Index==0x00) {
                                if (Index == color)  //mcl add for  trans
                                {
                                    // end
                                    /* Transparent color ... clear cache */
                                    if (Cached)
                                    {
                                        LCD_DrawHLine(x0 + xStart, y, x0 + x - 1);
                                        Cached = 0;
                                    }
                                }
                                else
                                {
                                    /* Another color ... draw contents of cache */
                                    if (Cached && xStart != -1)
                                    {
                                        LCD_DrawHLine(x0 + xStart, y, x0 + x - 1);
                                    }

                                    xStart    = x;
                                    Cached    = 1;

                                    if (pBitmap->pMethods)
                                    {
                                        Color = pBitmap->pMethods->pfIndex2Color(Index);
                                    }
                                    else
                                    {
                                        Color = pBitmap->pPal->pPalEntries[Index];
                                    }

                                    LCD_SetColorIndex(LCDDEV_L0_Color2Index(Color));
                                }

                                IndexPrev = Index;
                            }

                            do
                            {
                                x++;
                                xiMag -= 1000;
                            } while (xiMag >= 1000);
                        }
                    }

                    //mcl add for  trans
                    /* Clear cache */
                    if (Cached)
                    {
                        LCD_DrawHLine(x0 + xStart, y, x0 + x - 1);
                    }
                }

                y += yStep;
            } while ((yMag < 0) ? (y > yEnd) : (y < yEnd));
        }
    }
}

/*********************************************************************
*
*       public code
*
**********************************************************************
*/

/*********************************************************************
*
*       GUI_DrawBitmapEx_Trans
*/
void GUI_DrawBitmapEx_Trans(const GUI_BITMAP GUI_UNI_PTR *pBitmap, int x0, int y0,
                            int xCenter, int yCenter, int xMag, int yMag, int color)
{
    GUI_COLOR OldColor;
#if (GUI_WINSUPPORT)
    GUI_RECT r;
#endif
    GUI_LOCK();
    OldColor = GUI_GetColor();
#if (GUI_WINSUPPORT)
    WM_ADDORG(x0, y0);

    if (xMag >= 0)
    {
        r.x0 = x0 + GUI__DivideRound32(((I32)(-xCenter) * (I32)(xMag)), 1000);
        r.x1 = x0 + GUI__DivideRound32(((I32)(pBitmap->XSize - xCenter - 1) * (I32)(xMag)), 1000);
    }
    else
    {
        r.x1 = x0 + GUI__DivideRound32(((I32)(-xCenter) * (I32)(xMag)), 1000);
        r.x0 = x0 + GUI__DivideRound32(((I32)(pBitmap->XSize - xCenter - 1) * (I32)(xMag)), 1000);
    }

    if (yMag >= 0)
    {
        r.y0 = y0 + GUI__DivideRound32(((I32)(-yCenter) * (I32)(yMag)), 1000);
        r.y1 = y0 + GUI__DivideRound32(((I32)(pBitmap->YSize - yCenter - 1) * (I32)(yMag)), 1000);
    }
    else
    {
        r.y1 = y0 + GUI__DivideRound32(((I32)(-yCenter) * (I32)(yMag)), 1000);
        r.y0 = y0 + GUI__DivideRound32(((I32)(pBitmap->YSize - yCenter - 1) * (I32)(yMag)), 1000);
    }

    WM_ITERATE_START(&r)
    {
#endif
        GL_DrawBitmapEx_Trans(pBitmap, x0, y0, xCenter, yCenter, xMag, yMag, color);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_SetColor(OldColor);
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
