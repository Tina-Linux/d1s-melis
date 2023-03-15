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
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       CL_DispChar
*/
#if (GUI_WINSUPPORT)
static void CL_DispChar(U16 c)
{
    GUI_RECT r;
    WM_ADDORG(GUI_Context.DispPosX[GUI_Context.SelLayer], GUI_Context.DispPosY[GUI_Context.SelLayer]);
    r.x1 = (r.x0 = GUI_Context.DispPosX[GUI_Context.SelLayer]) + GUI_GetCharDistX(c) - 1;
    r.y1 = (r.y0 = GUI_Context.DispPosY[GUI_Context.SelLayer]) + GUI_GetFontSizeY()  - 1;
    WM_ITERATE_START(&r)
    {
        GL_DispChar(c);
    }
    WM_ITERATE_END();

    if (c != '\n')
    {
        GUI_Context.DispPosX[GUI_Context.SelLayer] = r.x1 + 1;
    }

    WM_SUBORG(GUI_Context.DispPosX[GUI_Context.SelLayer], GUI_Context.DispPosY[GUI_Context.SelLayer]);
}
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DispChar
*/
void GUI_DispChar(U16 c)
{
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    CL_DispChar(c);
#else
    GL_DispChar(c);
#endif
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_DispCharAt
*/
void GUI_DispCharAt(U16 c, I16P x, I16P y)
{
    GUI_LOCK();
    GUI_Context.DispPosX[GUI_Context.SelLayer] = x;
    GUI_Context.DispPosY[GUI_Context.SelLayer] = y;
#if (GUI_WINSUPPORT)
    CL_DispChar(c);
#else
    GL_DispChar(c);
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
