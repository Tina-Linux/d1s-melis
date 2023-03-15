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
#include <stddef.h>

#if GUI_SUPPORT_AA

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _AA_DrawLine
*/
static void _AA_DrawLine(int x0, int y0, int x1, int y1)
{
    int xMin, xMax;
    U8 PenSizeOld = GUI_GetPenSize();
    U8 PenSizeHR  = PenSizeOld * GUI_Context.AA_Factor;
    U8 OldPenShape = GUI_SetPenShape(GUI_PS_FLAT);

    /* Calculate left and right borders for AA module */
    if (x0 < x1)
    {
        xMin = x0;
        xMax = x1;
    }
    else
    {
        xMin = x1;
        xMax = x0;
    }

    if (GUI_Context.AA_HiResEnable)
    {
        xMin -= PenSizeHR;
        xMax += PenSizeHR;
        xMin /= GUI_Context.AA_Factor;
        xMax /= GUI_Context.AA_Factor;
    }
    else
    {
        xMin -= PenSizeOld;
        xMax += PenSizeOld;
        x0 *= GUI_Context.AA_Factor;
        x1 *= GUI_Context.AA_Factor;
        y0 *= GUI_Context.AA_Factor;
        y1 *= GUI_Context.AA_Factor;
    }

    /* Do the actual drawing */
    GUI_AA_Init(xMin, xMax);
    GUI_SetPenSize(PenSizeHR);
    GL_DrawLine(x0, y0, x1,  y1);
    GUI_AA_Exit();

    /* Draw end points (can be optimized away in future, TBD*/
    switch (OldPenShape)
    {
        case GUI_PS_ROUND:
        {
            int r = GUI_Context.AA_Factor * PenSizeOld / 2;
            GL_FillCircleAA_HiRes(x0, y0, r);
            GL_FillCircleAA_HiRes(x1, y1, r);
        }
        break;
    }

    GUI_SetPenSize(PenSizeOld);
    GUI_SetPenShape(OldPenShape);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_AA_DrawLine
*/
void GUI_AA_DrawLine(int x0, int y0, int x1, int y1)
{
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG_AA(x0, y0);
    WM_ADDORG_AA(x1, y1);
    WM_ITERATE_START(NULL);
    {
#endif
        _AA_DrawLine(x0, y0, x1, y1);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

#else                            /* Avoid problems with empty object modules */
void GUIAALine_C(void);
void GUIAALine_C(void) {}
#endif /* GUI_SUPPORT_AA */

/*************************** End of file ****************************/
