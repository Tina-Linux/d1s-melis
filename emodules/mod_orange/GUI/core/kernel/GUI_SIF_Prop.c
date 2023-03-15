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
*       _GetNumCharAreas
*/
static int _GetNumCharAreas(const GUI_FONT GUI_UNI_PTR *pFont)
{
    U16 NumCharAreas;
    const U8 *pData;
    pData = (const U8 *)pFont->p.pFontData + 14 /* Skip ID, YSize, YDist, Baseline, LHeight and CHeight */;
    NumCharAreas = GUI__Read16(&pData);
    return NumCharAreas;
}

/*********************************************************************
*
*       _GetpCharInfo
*/
static const U8 *_GetpCharInfo(const GUI_FONT GUI_UNI_PTR *pFont, U16P c)
{
    const U8 *pCharArea, * pCharInfo;
    int NumCharAreas;
    NumCharAreas = _GetNumCharAreas(pFont);                 /* Get # of char areas */
    pCharArea = (const U8 *)pFont->p.pFontData
                + sizeof(GUI_SI_FONT);                      /* Set char area pointer to first char area */
    pCharInfo = pCharArea
                + sizeof(GUI_SIF_CHAR_AREA) * NumCharAreas; /* Set char info pointer to first character info */

    do
    {
        GUI_SIF_CHAR_AREA CharArea;
        CharArea.First    = GUI__Read16(&pCharArea);
        CharArea.Last     = GUI__Read16(&pCharArea);

        if ((c >= CharArea.First) && (c <= CharArea.Last))
        {
            pCharInfo += sizeof(GUI_SIF_CHARINFO) * (c - CharArea.First);
            return pCharInfo;
        }

        pCharInfo += sizeof(GUI_SIF_CHARINFO) * (CharArea.Last - CharArea.First + 1);
    } while (--NumCharAreas);

    return 0;
}

/*********************************************************************
*
*       Exported code
*
**********************************************************************
*/

/*********************************************************************
*
*       GUI_SIF_DispChar
*/
static void _GUI_SIF_DispChar(U16P c)
{
    const U8 *pCharInfo, * pData;
    pCharInfo = _GetpCharInfo(GUI_Context.pAFont, c);

    if (pCharInfo)
    {
        GUI_DRAWMODE DrawMode, OldDrawMode;
        GUI_SIF_CHARINFO CharInfo;
        CharInfo.XSize        = GUI__Read16(&pCharInfo);
        CharInfo.XDist        = GUI__Read16(&pCharInfo);
        CharInfo.BytesPerLine = GUI__Read16(&pCharInfo);
        GUI__Read16(&pCharInfo); /* Dummy */
        CharInfo.OffData      = GUI__Read32(&pCharInfo);
        pData = (const U8 *)GUI_Context.pAFont->p.pFontData + CharInfo.OffData;
        DrawMode = GUI_Context.TextMode;
        OldDrawMode  = LCD_SetDrawMode(DrawMode);
        LCD_DrawBitmap(GUI_Context.DispPosX[GUI_Context.SelLayer], GUI_Context.DispPosY[GUI_Context.SelLayer],
                       CharInfo.XSize,
                       GUI_Context.pAFont->YSize,
                       GUI_Context.pAFont->XMag,
                       GUI_Context.pAFont->YMag,
                       1,
                       CharInfo.BytesPerLine,
                       pData,
                       &LCD_BKCOLORINDEX);

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
                             GUI_Context.DispPosX[GUI_Context.SelLayer] + CharInfo.XSize,
                             GUI_Context.DispPosY[GUI_Context.SelLayer] + YDist);
                GUI_SetColor(OldColor);
            }
        }

        LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
        GUI_Context.DispPosX[GUI_Context.SelLayer] += CharInfo.XDist;
    }
}

/*********************************************************************
*
*       GUI_SIF_GetCharDistX
*/
static int _GUI_SIF_GetCharDistX(U16P c)
{
    const U8 *pCharInfo;
    U16 DistX = 0;
    pCharInfo = _GetpCharInfo(GUI_Context.pAFont, c); /* Get pointer to char info data */

    if (pCharInfo)
    {
        pCharInfo += 2 /* Skip XSize */;
        DistX = GUI__Read16(&pCharInfo);
    }

    return DistX;
}

/*********************************************************************
*
*       GUI_SIF_GetFontInfo
*/
static void _GUI_SIF_GetFontInfo(const GUI_FONT GUI_UNI_PTR *pFont, GUI_FONTINFO *pfi)
{
    const U8 *pData;
    pData = (const U8 *)pFont->p.pFontData + 4 /* Skip XSize and XDist */;
    pfi->Baseline = GUI__Read16(&pData);
    pfi->LHeight  = GUI__Read16(&pData);
    pfi->CHeight  = GUI__Read16(&pData);
    pfi->Flags    = GUI_FONTINFO_FLAG_PROP;
}

/*********************************************************************
*
*       GUI_SIF_IsInFont
*/
static char _GUI_SIF_IsInFont(const GUI_FONT GUI_UNI_PTR *pFont, U16 c)
{
    const U8 *pCharInfo;
    GUI_USE_PARA(pFont);
    pCharInfo = _GetpCharInfo(GUI_Context.pAFont, c);
    return (pCharInfo) ? 1 : 0;
}

/*********************************************************************
*
*       GUI_SIF_TYPE_PROP
*/
const tGUI_SIF_APIList GUI_SIF_APIList_Prop =
{
    _GUI_SIF_DispChar,
    _GUI_SIF_GetCharDistX,
    _GUI_SIF_GetFontInfo,
    _GUI_SIF_IsInFont,
    NULL, NULL
};
