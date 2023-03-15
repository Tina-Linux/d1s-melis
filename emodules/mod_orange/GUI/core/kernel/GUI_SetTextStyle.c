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
*       GUI__cbDrawTextStyle
*/
static void GUI__cbDrawTextStyle(U16 Char)
{
    int x0, x1;
    x1 = GUI_Context.DispPosX[GUI_Context.SelLayer] - 1;
    x0 = x1 - GUI_Context.pAFont->pfGetCharDistX(Char) + 1;

    /* Handle Underline */
    if (GUI_Context.TextStyle & GUI_TS_UNDERLINE)
    {
        int yOff = GUI_Context.pAFont->Baseline;

        if (yOff >= GUI_Context.pAFont->YSize)
        {
            yOff = GUI_Context.pAFont->YSize - 1;
        }

        LCD_DrawHLine(x0, GUI_Context.DispPosY[GUI_Context.SelLayer] + yOff, x1);
    }

    /* Handle strike thru */
    if (GUI_Context.TextStyle & GUI_TS_STRIKETHRU)
    {
        int yOff = GUI_Context.pAFont->Baseline - ((GUI_Context.pAFont->CHeight + 1) / 2);
        LCD_DrawHLine(x0, GUI_Context.DispPosY[GUI_Context.SelLayer] + yOff, x1);
    }

    /* Handle over line */
    if (GUI_Context.TextStyle & GUI_TS_OVERLINE)
    {
        int yOff = GUI_Context.pAFont->Baseline - GUI_Context.pAFont->CHeight - 1;

        if (yOff < 0)
        {
            yOff = 0;
        }

        LCD_DrawHLine(x0, GUI_Context.DispPosY[GUI_Context.SelLayer] + yOff, x1);
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
*       GUI_SetTextStyle
*/
char GUI_SetTextStyle(char Style)
{
    char OldStyle;
    GUI_LOCK();
    OldStyle = GUI_Context.TextStyle;
    //GUI_pfDispCharStyle = GUI__cbDrawTextStyle;    /* Init function pointer (function in this module) */
    GUI_Context.TextStyle = Style;
    GUI_UNLOCK();
    return OldStyle;
}

/*************************** End of file ****************************/
