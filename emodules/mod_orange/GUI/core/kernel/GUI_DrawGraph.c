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
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawGraph
*/
static void _DrawGraph(I16 *pay, int NumPoints, int xOff, int yOff)
{
    int i;
    int y0, y1;
    int yClip0, yClip1;
    /* Perform high level clipping in x */
    yClip0 = GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y0;
    yClip1 = GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y1;
    i = 0;

    if (GUI_Context.pClipRect_HL[GUI_Context.SelLayer])
    {
        if (xOff < GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->x0)
        {
            i = GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->x0 - xOff;
        }

        if (xOff + NumPoints - 1 > GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->x1)
        {
            NumPoints = GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->x1 - xOff + 1;
        }
    }

    /* Start drawing if there is something left to do after x-clipping */
    if (i < NumPoints)
    {
        y0 = *(pay + i++) + yOff;

        /* Iterate */
        for (; i < NumPoints; i++)
        {
            /* Perform high level clipping in y */
            y1 = *(pay + i) + yOff;

            if ((y0 >= yClip0) || (y1 >= yClip0))
            {
                if ((y0 <= yClip1) || (y1 <= yClip1))
                {
                    int y01;
                    y01 = (y1 + y0) / 2;

                    if (y0 <= y1)
                    {
                        LCD_DrawVLine(i + xOff - 1, y0, y01);
                        LCD_DrawVLine(i + xOff, y01, y1);
                    }
                    else
                    {
                        LCD_DrawVLine(i + xOff - 1, y01, y0);
                        LCD_DrawVLine(i + xOff, y1, y01);
                    }
                }
            }

            y0 = y1;
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
*       GUI_DrawGraph
*/
void GUI_DrawGraph(I16 *pay, int NumPoints, int x0, int y0)
{
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG(x0, y0);
    WM_ITERATE_START(NULL);
    {
#endif
        _DrawGraph(pay, NumPoints, x0, y0);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
