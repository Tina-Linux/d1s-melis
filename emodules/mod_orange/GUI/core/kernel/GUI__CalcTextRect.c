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
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__CalcTextRect
*/
void GUI__CalcTextRect(const char GUI_UNI_PTR *pText, const GUI_RECT *pTextRectIn, GUI_RECT *pTextRectOut, int TextAlign)
{
    if (pText)
    {
        int xPos, yPos, TextWidth, TextHeight;
        /* Calculate X-pos of text */
        TextWidth = GUI_GetStringDistX(pText);

        switch (TextAlign & GUI_TA_HORIZONTAL)
        {
            case GUI_TA_HCENTER:
                xPos = pTextRectIn->x0 + ((pTextRectIn->x1 - pTextRectIn->x0 + 1) - TextWidth) / 2;
                break;

            case GUI_TA_RIGHT:
                xPos = pTextRectIn->x1 - TextWidth + 1;
                break;

            default:
                xPos = pTextRectIn->x0;
        }

        /* Calculate Y-pos of text*/
        TextHeight = GUI_GetFontDistY();

        switch (TextAlign & GUI_TA_VERTICAL)
        {
            case GUI_TA_VCENTER:
                yPos = pTextRectIn->y0 + ((pTextRectIn->y1 - pTextRectIn->y0 + 1) - TextHeight) / 2;
                break;

            case GUI_TA_BOTTOM:
                yPos = pTextRectIn->y1 - TextHeight + 1;
                break;

            case GUI_TA_BASELINE:
            default:
                yPos = pTextRectIn->y0;
        }

        /* Return text rectangle */
        pTextRectOut->x0 = xPos;
        pTextRectOut->y0 = yPos;
        pTextRectOut->x1 = xPos + TextWidth  - 1;
        pTextRectOut->y1 = yPos + TextHeight - 1;
    }
    else
    {
        *pTextRectOut = *pTextRectIn;
    }
}

/*************************** End of file ****************************/
