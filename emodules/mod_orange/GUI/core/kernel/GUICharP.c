/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUICharP.C
* Purpose : Implementation of Proportional fonts
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
**********************************************************************************************************************
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
