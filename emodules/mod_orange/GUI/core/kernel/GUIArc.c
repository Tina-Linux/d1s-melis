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
#include <stddef.h>           /* needed for definition of NULL */
#include <math.h>
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _CalcX
*/
static void _CalcX(int *px, int y, U32 r2)
{
    int x = *px;
    U32 y2 = (U32)y * (U32)y;
    U32 r2y2 = r2 - y2;
    U32 x2;

    if (y2 >= r2)
    {
        *px = 0;
        return;
    }

    /* x2 = r2-y2 */
    do
    {
        x++;
        x2 = (U32)x * (U32)x;
    } while (x2 < r2y2);

    *px = x - 1;
}

/*********************************************************************
*
*       _CalcInterSectLin
*/
static float _CalcInterSectLin(float y, float y0, float y1, float x0, float x1)
{
    if (y1 == y0)
    {
        return y0;
    }
    else
    {
        float Slope = (x1 - x0) / (y1 - y0);
        return (y - y0) * Slope + x0;
    }
}

/*********************************************************************
*
*       _DrawArc
*/
static void _DrawArc(int x0, int y0, int rx, int ry, int Angle0, int Angle1, int xMul, int yMul)
{
    float afx[4];
    float afy[4];
    float ri = rx - (GUI_Context.PenSize + 1.5) / 2;
    float ro = rx + (GUI_Context.PenSize + 1.5) / 2;
    float fAngle0 = Angle0 * 3.1415926 / 180;
    float fAngle1 = Angle1 * 3.1415926 / 180;
    float sin0 = sin(fAngle0);
    float sin1 = sin(fAngle1);
    float cos0 = cos(fAngle0);
    float cos1 = cos(fAngle1);
    U32   ri2 = ri * ri;
    U32   ro2 = ro * ro;
    int y, yMax, yMin;
    afy[0] = ri * sin0;
    afy[1] = ro * sin0;
    afy[2] = ri * sin1;
    afy[3] = ro * sin1;
    afx[0] = ri * cos0;
    afx[1] = ro * cos0;
    afx[2] = ri * cos1;
    afx[3] = ro * cos1;
    yMin = ceil(afy[0]);
    yMax = floor(afy[3]);

    /* Use Clipping rect to reduce calculation (if possible) */
    if (GUI_Context.pClipRect_HL[GUI_Context.SelLayer])
    {
        if (yMul == 1)
        {
            if (yMax > (GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y1 - y0))
            {
                yMax = (GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y1 - y0);
            }

            if (yMin < (GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y0 - y0))
            {
                yMin = (GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y0 - y0);
            }
        }

        if (yMul == -1)
        {
            if (yMin > (GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y1 - y0))
            {
                yMin = (GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y1 - y0);
            }

            if (yMax < (GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y0 - y0))
            {
                yMax = (GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y0 - y0);
            }
        }
    }

    /* Start drawing lines ... */
    {
        int xMinDisp, xMaxDisp, xMin = 0, xMax = 0;

        for (y = yMax; y >= yMin; y--)
        {
            _CalcX(&xMin, y, ri2);
            _CalcX(&xMax, y, ro2);

            if ((float)y < afy[1])
            {
                xMaxDisp = _CalcInterSectLin(y, afy[0], afy[1], afx[0], afx[1]);
            }
            else
            {
                xMaxDisp = xMax;
            }

            if ((float)y > afy[2])
            {
                xMinDisp = _CalcInterSectLin(y, afy[2], afy[3], afx[2], afx[3]);
            }
            else
            {
                xMinDisp = xMin;
            }

            if (xMul > 0)
            {
                LCD_HL_DrawHLine(xMinDisp + x0, yMul * y + y0, xMaxDisp + x0);
            }
            else
            {
                LCD_HL_DrawHLine(-xMaxDisp + x0, yMul * y + y0, -xMinDisp + x0);
            }
        }
    }
#if 0  /* Test code */
    {
        int i;
        GUI_SetColor(GUI_WHITE);

        for (i = 0; i < 4; i++)
        {
            LCD_HL_DrawPixel(afx[i] + x0, afy[i] + y0);
        }
    }
#endif
    GUI_USE_PARA(ry);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GL_DrawArc
*/
void GL_DrawArc(int x0, int y0, int rx, int ry, int a0, int a1)
{
    int aEnd;
    a0 += 360;
    a1 += 360;

    while (a0 >= 360)
    {
        a0 -= 360;
        a1 -= 360;
    }

    /* Do first quadrant 0-90 degree */
DoFirst:

    if (a1 <= 0)
    {
        return;
    }

    if (a0 < 90)
    {
        if (a0 < 0)
        {
            a0 = 0;
        }

        aEnd = (a1 < 90) ? a1 : 90;
        _DrawArc(x0, y0, rx, ry, a0, aEnd, 1, -1);
    }

    a1 -= 90;
    a0 -= 90;

    /* Do second quadrant 90-180 degree */
    if (a1 <= 0)
    {
        return;
    }

    if (a0 < 90)
    {
        if (a0 < 0)
        {
            a0 = 0;
        }

        aEnd = (a1 < 90) ? a1 : 90;
        _DrawArc(x0, y0, rx, ry, 90 - aEnd, 90 - a0, -1, -1);
    }

    a1 -= 90;
    a0 -= 90;

    /* Do third quadrant 180-270 degree */
    if (a1 <= 0)
    {
        return;
    }

    if (a0 < 90)
    {
        if (a0 < 0)
        {
            a0 = 0;
        }

        aEnd = (a1 < 90) ? a1 : 90;
        _DrawArc(x0, y0, rx, ry, a0, aEnd, -1, 1);
    }

    a1 -= 90;
    a0 -= 90;

    /* Do last quadrant 270-360 degree */
    if (a1 <= 0)
    {
        return;
    }

    if (a0 < 90)
    {
        if (a0 < 0)
        {
            a0 = 0;
        }

        aEnd = (a1 < 90) ? a1 : 90;
        _DrawArc(x0, y0, rx, ry, 90 - aEnd, 90 - a0, 1, 1);
    }

    a1 -= 90;
    a0 -= 90;
    goto DoFirst;
}

/*********************************************************************
*
*       GUI_DrawArc
*/
void GUI_DrawArc(int x0, int y0, int rx, int ry, int a0, int a1)
{
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG(x0, y0);
    WM_ITERATE_START(NULL)
    {
#endif
        GL_DrawArc(x0, y0, rx, ry, a0, a1);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
