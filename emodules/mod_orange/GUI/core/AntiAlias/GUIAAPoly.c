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
#include "GUI_Private.h"

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
*       _FillPolygonAA
*/
static void _FillPolygonAA(GUI_POINT *paPoint, int NumPoints, int x, int y)
{
    int i;
    int Stat;
    int x0, x1;
    int PosFactor = GUI_Context.AA_HiResEnable ? 1 : GUI_Context.AA_Factor;
    /* Calc horizontal limits and init GUI_AA */
    x0 = x1 = paPoint[0].x;

    for (i = 1; i < NumPoints; i++)
    {
        int x =  paPoint[i].x;

        if (x < x0)
        {
            x0 = x;
        }

        if (x > x1)
        {
            x1 = x;
        }
    }

    if (!GUI_Context.AA_HiResEnable)
    {
        Stat = GUI_AA_Init(x0 + x, x1 + x);
    }
    else
    {
        Stat = GUI_AA_Init((x0 + x) / GUI_Context.AA_Factor, (x1 + x) / GUI_Context.AA_Factor);
    }

    if (Stat == 0)
    {
        /* Modify pointlist */
        if (!GUI_Context.AA_HiResEnable)
        {
            for (i = 0; i < NumPoints; i++)
            {
                paPoint[i].x *= GUI_Context.AA_Factor;
                paPoint[i].y *= GUI_Context.AA_Factor;
            }
        }

        GL_FillPolygon(paPoint, NumPoints, x * PosFactor, y * PosFactor);

        /* Restore pointlist */
        if (!GUI_Context.AA_HiResEnable)
        {
            for (i = 0; i < NumPoints; i++)
            {
                paPoint[i].x /= GUI_Context.AA_Factor;
                paPoint[i].y /= GUI_Context.AA_Factor;
            }
        }

        /* Cleanup */
    }

    GUI_AA_Exit();
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_AA_FillPolygon
*/
void GUI_AA_FillPolygon(/*const*/ GUI_POINT *pPoints, int NumPoints, int x0, int y0)
{
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG_AA(x0, y0);
    WM_ITERATE_START(NULL);
    {
#endif
        /* Variables in MEMDEV module (with memory devices only) */
#if GUI_SUPPORT_MEMDEV

        if (GUI_Context.pDeviceAPI->pfFillPolygonAA)
        {
            GUI_Context.pDeviceAPI->pfFillPolygonAA(pPoints, NumPoints, x0, y0);
        }
        else
#endif
            _FillPolygonAA(pPoints, NumPoints, x0, y0);

#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

#else                            /* Avoid problems with empty object modules */
void GUIAAPoly_C(void);
void GUIAAPoly_C(void) {}
#endif /* GUI_SUPPORT_AA */

/*************************** End of file ****************************/
