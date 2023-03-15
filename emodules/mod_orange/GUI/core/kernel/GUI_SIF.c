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
