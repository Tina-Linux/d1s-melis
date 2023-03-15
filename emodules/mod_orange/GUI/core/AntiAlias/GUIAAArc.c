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
#include <string.h>
#include <math.h>

#include "GUI_Protected.h"

#if GUI_SUPPORT_AA

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawArcAA
*/
static void _DrawArcAA(int x0, int y0, int rx, int ry, int a0, int a1)
{
    int PenSizePrev = GUI_GetPenSize();
    int PenSize_AA;
    int Factor;
    Factor = GUI_Context.AA_HiResEnable ? 1 : GUI_Context.AA_Factor;
    /* Convert into Hires coordinates (if not already done) */
    x0 *= Factor;
    y0 *= Factor;
    rx *= Factor;
    ry *= Factor;
    PenSize_AA = PenSizePrev * GUI_Context.AA_Factor;
    GUI_AA_Init_HiRes(x0 - rx - PenSize_AA, x0 + rx + PenSize_AA);
    GUI_SetPenSize(PenSize_AA);
    GL_DrawArc(x0, y0, rx, ry, a0, a1);
    GUI_SetPenSize(PenSizePrev);          /* restore former pen size */
    /* Cleanup */
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
*       GUI_AA_DrawArc
*/
void GUI_AA_DrawArc(int x0, int y0, int rx, int ry, int a0, int a1)
{
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG_AA(x0, y0);
    WM_ITERATE_START(NULL)
    {
#endif
        _DrawArcAA(x0, y0, rx, ry, a0, a1);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

#else                            /* Avoid problems with empty object modules */
void GUIAAArc_C(void);
void GUIAAArc_C(void) {}
#endif /* GUI_SUPPORT_AA */

/*************************** End of file ****************************/
