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
#include "GUI_Private.h"

#if GUI_SUPPORT_AA

#include <string.h>

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define MAX_CHAR_SIZE 100

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

/* used by transparent characters */
static const int aConvTable[4] = {0, 5, 10, 15};

/* used by non transparent characters */
static LCD_COLOR aColor[4];
static LCD_PIXELINDEX OldColorIndex, OldBkColorIndex;
static GUI_LOGPALETTE Palette = {4, 0, &aColor[0]};
static GUI_BITMAP Bitmap = {0, 0, 0, 2, 0, &Palette, 0};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       DrawNoTrans
*/
static void DrawNoTrans(int x0, int y0, int XSize, int YSize, int BytesPerLine, const U8 *pData)
{
    if ((OldColorIndex   != LCD_COLORINDEX) ||
        (OldBkColorIndex != LCD_BKCOLORINDEX))
    {
        int i;
        LCD_PIXELINDEX BkColorIndex = LCD_BKCOLORINDEX;
        LCD_PIXELINDEX ColorIndex   = LCD_COLORINDEX;
        LCD_COLOR BkColor = LCD_Index2Color(BkColorIndex);
        LCD_COLOR Color   = LCD_Index2Color(ColorIndex);
        aColor[0] = BkColor;

        for (i = 1; i < 3; i++)
        {
            U8 Intens;
            Intens = 5 * i;
            aColor[i] = LCD_AA_MixColors(Color, BkColor, Intens);
        }

        aColor[3] = Color;
        LCD_GetpPalConvTableUncached(&Palette);
        OldColorIndex = ColorIndex;
        OldBkColorIndex = BkColorIndex;
    }

    Bitmap.XSize = XSize;
    Bitmap.YSize = YSize;
    Bitmap.BytesPerLine = BytesPerLine;
    Bitmap.pData = pData;
    GL_DrawBitmap(&Bitmap, x0, y0);
}

/*********************************************************************
*
*       DrawTrans
*/
static void DrawTrans(int x0, int y0, int XSize, int YSize, int BytesPerLine, const U8 *pData)
{
    int x, y;

    for (y = 0; y < YSize; y++)
    {
        const U8 *pData0 = pData;
        U8 Rem = XSize & 3;

        for (x = 0; x < XSize - 1; x += 4)
        {
            LCD_SetPixelAA(x + x0, y0 + y, aConvTable[((*pData0) >> 6)       ]);
            LCD_SetPixelAA(x + x0 + 1, y0 + y, aConvTable[((*pData0) >> 4) & 0x03]);
            LCD_SetPixelAA(x + x0 + 2, y0 + y, aConvTable[((*pData0) >> 2) & 0x03]);
            LCD_SetPixelAA(x + x0 + 3, y0 + y, aConvTable[((*pData0++)) & 0x03]);
        }

        if (Rem)
        {
            LCD_SetPixelAA(x + x0, y0 + y, aConvTable[((*pData0) >> 6)       ]);

            if (Rem > 1)
            {
                LCD_SetPixelAA(x + x0 + 1, y0 + y, aConvTable[((*pData0) >> 4) & 0x03]);

                if (Rem > 2)
                {
                    LCD_SetPixelAA(x + x0 + 2, y0 + y, aConvTable[((*pData0) >> 2) & 0x03]);
                }
            }
        }

        pData += BytesPerLine;
    }
}

/*********************************************************************
*
*       GUIPROP_FindChar
*/
static const GUI_FONT_PROP *GUIPROP_FindChar(const GUI_FONT_PROP *pProp, U16P c)
{
    for (pProp = GUI_Context.pAFont->p.pProp; pProp; pProp = (const GUI_FONT_PROP *) pProp->pNext)
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
*       GUIPROP_AA2_DispChar
*/
void GUIPROP_AA2_DispChar(U16P c)
{
    int BytesPerLine;
    GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
    const GUI_FONT_PROP *pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);

    if (pProp)
    {
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO *pCharInfo = pProp->paCharInfo + (c - pProp->First);
        BytesPerLine = pCharInfo->BytesPerLine;
        OldDrawMode  = LCD_SetDrawMode(DrawMode);

        if (DrawMode & GUI_TM_TRANS)
        {
            DrawTrans(GUI_Context.DispPosX,
                      GUI_Context.DispPosY,
                      pCharInfo->XSize,
                      GUI_Context.pAFont->YSize,
                      BytesPerLine,
                      (U8 const *)pCharInfo->pData
                     );
        }
        else
        {
            DrawNoTrans(GUI_Context.DispPosX,
                        GUI_Context.DispPosY,
                        pCharInfo->XSize,
                        GUI_Context.pAFont->YSize,
                        BytesPerLine,
                        (U8 const *)pCharInfo->pData
                       );
        }

        LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
        GUI_Context.DispPosX += pCharInfo->XDist;
    }
}

/*********************************************************************
*
*       GUIPROP_AA2_GetCharDistX
*/
int GUIPROP_AA2_GetCharDistX(U16P c)
{
    const GUI_FONT_PROP *pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
    return (pProp) ? (pProp->paCharInfo + (c - pProp->First))->XSize : 0;
}

/*********************************************************************
*
*       GUIPROP_AA2_GetFontInfo
*/
void GUIPROP_AA2_GetFontInfo(const GUI_FONT *pFont, GUI_FONTINFO *pfi)
{
    GUI_USE_PARA(pFont);
    pfi->Flags = GUI_FONTINFO_FLAG_PROP | GUI_FONTINFO_FLAG_AA2;
}

/*********************************************************************
*
*       GUIPROP_AA2_IsInFont
*/
char GUIPROP_AA2_IsInFont(const GUI_FONT *pFont, U16 c)
{
    const GUI_FONT_PROP *pProp = GUIPROP_FindChar(pFont->p.pProp, c);
    return (pProp == NULL) ? 0 : 1;
}

#else                            /* Avoid problems with empty object modules */
void GUIAAChar2_C(void);
void GUIAAChar2_C(void) {}
#endif /* GUI_SUPPORT_AA */

/*************************** End of file ****************************/
