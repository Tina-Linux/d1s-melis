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
*       Types
*
**********************************************************************
*/

typedef void tSetPixelAA(int x, int y, U8 Intens);

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static const U8 Bit2Mask0[] = {1 << 7, 1 << 5, 1 << 3, 1 << 1};
static const U8 Bit2Mask1[] = {1 << 6, 1 << 4, 1 << 2, 1 << 0};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       Draw
*/
static void Draw(int x0, int y0, int XSize, int YSize, int BytesPerLine, const U8 *pData)
{
    int x, y;
    tSetPixelAA *pfSetPixelAA;
    pfSetPixelAA = (GUI_Context.TextMode & GUI_TM_TRANS)
                   ? LCD_SetPixelAA : LCD_SetPixelAA_NoTrans;

    for (y = 0; y < YSize; y++)
    {
        const U8 *pData0 = pData;
        const U8 *pData1 = pData + BytesPerLine;

        for (x = 0; x < XSize; x++)
        {
            int PixelCnt = 0;
            int Mask0 = Bit2Mask0[x & 3];
            int Mask1 = Bit2Mask1[x & 3];

            if ((*pData0) & Mask0)
            {
                PixelCnt++;
            }

            if ((*pData0) & Mask1)
            {
                PixelCnt++;
            }

            if ((*pData1) & Mask0)
            {
                PixelCnt++;
            }

            if ((*pData1) & Mask1)
            {
                PixelCnt++;
            }

            if ((x & 3) == 3)
            {
                pData0++;
                pData1++;
            }

            switch (PixelCnt)
            {
                case 4:
                    LCD_HL_DrawPixel(x0 + x, y0 + y);
                    break;

                case 3:
                    (*pfSetPixelAA)(x0 + x, y0 + y, 12);
                    break;

                case 2:
                    (*pfSetPixelAA)(x0 + x, y0 + y, 8);
                    break;

                case 1:
                    (*pfSetPixelAA)(x0 + x, y0 + y, 4);
                    break;
            }
        }

        pData += 2 * BytesPerLine;
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
*       GUIPROPAA_DispChar
*/
void GUIPROPAA_DispChar(U16P c)
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
        Draw(GUI_Context.DispPosX, GUI_Context.DispPosY,
             (pCharInfo->XSize + 1) / 2,
             GUI_Context.pAFont->YSize,
             BytesPerLine,
             (U8 const *) pCharInfo->pData
            );
        LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
        GUI_Context.DispPosX += (pCharInfo->XDist + 1) / 2;
    }
}

/*********************************************************************
*
*       GUIPROPAA_GetCharDistX
*/
int GUIPROPAA_GetCharDistX(U16P c)
{
    int r;
    const GUI_FONT_PROP *pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
    r = (pProp) ? (pProp->paCharInfo + (c - pProp->First))->XSize : 0;
    return (r + 1) / 2;
}

/*********************************************************************
*
*       GUIPROPAA_GetFontInfo
*/
void GUIPROPAA_GetFontInfo(const GUI_FONT *pFont, GUI_FONTINFO *pfi)
{
    GUI_USE_PARA(pFont);
    pfi->Flags = GUI_FONTINFO_FLAG_PROP | GUI_FONTINFO_FLAG_AA;
}

/*********************************************************************
*
*       GUIPROPAA_IsInFont
*/
char GUIPROPAA_IsInFont(const GUI_FONT *pFont, U16 c)
{
    const GUI_FONT_PROP *pProp = GUIPROP_FindChar(pFont->p.pProp, c);
    return (pProp == NULL) ? 0 : 1;
}

#else                            /* Avoid problems with empty object modules */
void GUIAAChar_C(void);
void GUIAAChar_C(void) {}
#endif /* GUI_SUPPORT_AA */

/*************************** End of file ****************************/
