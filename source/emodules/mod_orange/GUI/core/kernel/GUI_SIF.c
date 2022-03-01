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
* File    : GUI_SIF.C
* Purpose : Implementation of system independend fonts
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:54:43
**********************************************************************************************************************
*/


#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Exported code
*
**********************************************************************
*/

/*********************************************************************
*
*       GUI_SIF_CreateFont
*/
void GUI_SIF_CreateFont(void *pFontData, GUI_FONT *pFont, const GUI_SIF_TYPE *pFontType)
{
    const U8 *pSrc = (const U8 *)pFontData;
    U32 ID;
    /* Set pFontData of GUI_FONT structure to the first byte */
    pFont->p.pFontData = pFontData;
    /* Check ID */
    ID = GUI__Read32(&pSrc);

    if (ID != 0x50495547)   /* 'GUIP' */
    {
        GUI_DEBUG_ERROROUT("ID of downloaded font wrong!");
        return;
    }

    /* Read properties of the font */
    pFont->YSize    = GUI__Read16(&pSrc);
    pFont->YDist    = GUI__Read16(&pSrc);
    pFont->Baseline = GUI__Read16(&pSrc);
    pFont->LHeight  = GUI__Read16(&pSrc);
    pFont->CHeight  = GUI__Read16(&pSrc);
    /* Magnification is always 1 */
    pFont->XMag = 1;
    pFont->YMag = 1;
    /* Set function pointers */
    pFont->pfDispChar     = pFontType->pfDispChar;
    pFont->pfGetCharDistX = pFontType->pfGetCharDistX;
    pFont->pfGetFontInfo  = pFontType->pfGetFontInfo;
    pFont->pfIsInFont     = pFontType->pfIsInFont;
    pFont->pafEncode      = 0;
    /* Use the new font */
    GUI_SetFont(pFont);
}

/*********************************************************************
*
*       GUI_SIF_DeleteFont
*/
void GUI_SIF_DeleteFont(GUI_FONT *pFont)
{
    GUI_USE_PARA(pFont);
}
