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
*       GUIPROP_FindChar
*/
static const GUI_FONT_PROP GUI_UNI_PTR *GUIPROP_FindChar(const GUI_FONT_PROP GUI_UNI_PTR *pProp, U16P c)
{
    for (; pProp; pProp = pProp->pNext)
    {
        if ((c >= pProp->First) && (c <= pProp->Last))
        {
            break;
        }
    }

    return pProp;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIPROP_DispChar
*
* Purpose:
*   This is the routine that displays a character. It is used by all
*   other routines which display characters as a subroutine.
*/
void GUIPROP_DispChar(U16P c)
{
    int BytesPerLine;
    GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);

    if (pProp)
    {
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO GUI_UNI_PTR *pCharInfo = pProp->paCharInfo + (c - pProp->First);
        BytesPerLine = pCharInfo->BytesPerLine;
        OldDrawMode  = LCD_SetDrawMode(DrawMode);
        LCD_DrawBitmap(GUI_Context.DispPosX[GUI_Context.SelLayer], GUI_Context.DispPosY[GUI_Context.SelLayer],
                       pCharInfo->XSize,
                       GUI_Context.pAFont->YSize,
                       GUI_Context.pAFont->XMag,
                       GUI_Context.pAFont->YMag,
                       1,     /* Bits per Pixel */
                       BytesPerLine,
                       pCharInfo->pData,
                       &LCD_BKCOLORINDEX
                      );

        /* Fill empty pixel lines */
        if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize)
        {
            int YMag = GUI_Context.pAFont->YMag;
            int YDist = GUI_Context.pAFont->YDist * YMag;
            int YSize = GUI_Context.pAFont->YSize * YMag;

            if (DrawMode != LCD_DRAWMODE_TRANS)
            {
                LCD_COLOR OldColor = GUI_GetColor();
                GUI_SetColor(GUI_GetBkColor());
                LCD_FillRect(GUI_Context.DispPosX[GUI_Context.SelLayer],
                             GUI_Context.DispPosY[GUI_Context.SelLayer] + YSize,
                             GUI_Context.DispPosX[GUI_Context.SelLayer] + pCharInfo->XSize,
                             GUI_Context.DispPosY[GUI_Context.SelLayer] + YDist);
                GUI_SetColor(OldColor);
            }
        }

        LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
        GUI_Context.DispPosX[GUI_Context.SelLayer] += pCharInfo->XDist * GUI_Context.pAFont->XMag;
    }
}

/*********************************************************************
*
*       GUIPROP_GetCharDistX
*/
int GUIPROP_GetCharDistX(U16P c)
{
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
    return (pProp) ? (pProp->paCharInfo + (c - pProp->First))->XSize * GUI_Context.pAFont->XMag : 0;
}

/*********************************************************************
*
*       GUIPROP_GetFontInfo
*/
void GUIPROP_GetFontInfo(const GUI_FONT GUI_UNI_PTR *pFont, GUI_FONTINFO *pfi)
{
    GUI_USE_PARA(pFont);
    pfi->Flags = GUI_FONTINFO_FLAG_PROP;
}

/*********************************************************************
*
*       GUIPROP_IsInFont
*/
char GUIPROP_IsInFont(const GUI_FONT GUI_UNI_PTR *pFont, U16 c)
{
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUIPROP_FindChar(pFont->p.pProp, c);
    return (pProp == NULL) ? 0 : 1;
}

/*************************** End of file ****************************/
