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
#include "GUI_Protected.h"

#include <string.h>
#include <math.h>

#if GUI_SUPPORT_AA

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       FillCircle
*/
static void FillCircle(int x0, int y0, int r)
{
    int i, x;
    int sqmax = r * r + r / 2;
    int yMin, yMax;
    /* First step : find uppermost and lowermost coordinates */
    yMin = y0 - r;
    yMax = y0 + r;

    /* Use Clipping rect to reduce calculation (if possible) */
    if (GUI_Context.pClipRect_HL)
    {
        if (yMax > GUI_Context.pClipRect_HL->y1)
        {
            yMax = GUI_Context.pClipRect_HL->y1;
        }

        if (yMin < GUI_Context.pClipRect_HL->y0)
        {
            yMin = GUI_Context.pClipRect_HL->y0;
        }
    }

    /* Draw top half */
    for (i = 0, x = r; i < r; i++)
    {
        int y = y0 - i;

        if ((y >= yMin) && (y <= yMax))
        {
            /* calc proper x-value */
            while ((i * i + x * x) > sqmax)
            {
                --x;
            }

            LCD_HL_DrawHLine(x0 - x, y, x0 + x);
        }
    }

    /* Draw bottom half */
    for (i = 0, x = r; i < r; i++)
    {
        int y = y0 + i;

        if ((y >= yMin) && (y <= yMax))
        {
            /* calc proper x-value */
            while ((i * i + x * x) > sqmax)
            {
                --x;
            }

            LCD_HL_DrawHLine(x0 - x, y, x0 + x);
        }
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GL_FillCircleAA_HiRes
*/
void GL_FillCircleAA_HiRes(int x0, int y0, int r)
{
    /* Init AA Subsystem, pass horizontal limits */
    GUI_AA_Init_HiRes(x0 - r, x0 + r);
    /* Do the actual drawing */
    FillCircle(x0, y0, r);
    /* Cleanup */
    GUI_AA_Exit();
}

/*********************************************************************
*
*       GUI_AA_FillCircle
*/
void GUI_AA_FillCircle(int x0, int y0, int r)
{
#if (GUI_WINSUPPORT)
    GUI_RECT Rect;
#endif
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG_AA(x0, y0);
#endif

    if (!GUI_Context.AA_HiResEnable)
    {
        x0 *= GUI_Context.AA_Factor;
        y0 *= GUI_Context.AA_Factor;
        r  *= GUI_Context.AA_Factor;
    }

#if (GUI_WINSUPPORT)
    Rect.x0 = GUI_AA_HiRes2Pixel(x0 - r);
    Rect.x1 = GUI_AA_HiRes2Pixel(x0 + r);
    Rect.y0 = GUI_AA_HiRes2Pixel(y0 - r);
    Rect.y1 = GUI_AA_HiRes2Pixel(y0 + r);
    WM_ITERATE_START(&Rect);
    {
#endif
        GL_FillCircleAA_HiRes(x0, y0, r);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

#else                            /* Avoid problems with empty object modules */
void GUIAACircle_C(void);
void GUIAACircle_C(void) {}
#endif /* GUI_SUPPORT_AA */

/*************************** End of file ****************************/
