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
#include <stddef.h>
#include "gui_core_kernel_private.h"

#if GUI_SUPPORT_ROTATION

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/

GUI_RECT GUI_RectDispString; /* Used by LCD_Rotate... */

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static const GUI_ROTATION *_SetRotation(const GUI_ROTATION *pLCD_Api)
{
    const GUI_ROTATION *pLCD_ApiOld;
    GUI_LOCK();
    pLCD_ApiOld = GUI_pLCD_APIList;

    if (pLCD_Api)
    {
        GUI_Context.pClipRect_HL[GUI_Context.SelLayer] = NULL;           /* Do not perform high level clipping in rotated mode */
    }
    else
    {
        GUI_Context.pClipRect_HL[GUI_Context.SelLayer] = &GUI_Context.ClipRect[GUI_Context.SelLayer];
    }

    GUI_pLCD_APIList = pLCD_Api;
    GUI_UNLOCK();
    return pLCD_ApiOld;
}

/*********************************************************************
*
*       Exported code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DispStringInRectEx
*/
void GUI_DispStringInRectEx(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int TextAlign, int MaxLen, const GUI_ROTATION *pLCD_Api)
{
    const GUI_ROTATION *pLCD_ApiOld;

    if (pLCD_Api == GUI_ROTATE_0)
    {
        /* Do nothing else but calling GUI_DispStringInRectMax if pLCD_Api == GUI_ROTATE_0 */
        GUI_DispStringInRectMax(s, pRect, TextAlign, MaxLen);
    }
    else
    {
#if (!GUI_WINSUPPORT)
        GUI_RECT Rect_Old;
#else
        const GUI_RECT *pOldClipRect = NULL;
#endif
        GUI_RECT rClip, rText = {0};
        GUI_LOCK();

        /* Set the clip rectangle */
        if (pRect)
        {
            rText = *pRect;
            GUI_RectDispString = rText;
#if (GUI_WINSUPPORT)
            pOldClipRect = WM_SetUserClipRect(pRect);

            if (pOldClipRect)
            {
                GUI__IntersectRects(&rClip, pRect, pOldClipRect);
                WM_SetUserClipRect(&rClip);
            }

#else
            Rect_Old = GUI_Context.ClipRect[GUI_Context.SelLayer];
            GUI__IntersectRects(&rClip, pRect, &Rect_Old);
            LCD_SetClipRectEx(&rClip);
#endif
        }

        pLCD_ApiOld = _SetRotation(pLCD_Api);                /* Set the new function pointer for rotating text */
        pLCD_Api->pfRect2TextRect(&rText);                   /* Calculate the text rectangle */
        GUI__DispStringInRect(s, &rText, TextAlign, MaxLen); /* Display the string */
        /* Restore old clipping rectangle */
#if (GUI_WINSUPPORT)
        WM_SetUserClipRect(pOldClipRect);
#else
        LCD_SetClipRectEx(&Rect_Old);
#endif
        _SetRotation(pLCD_ApiOld);
        GUI_UNLOCK();
    }
}

#else                                 /* Avoid empty object files */
void GUI_DispStringInRectEx_C(void);
void GUI_DispStringInRectEx_C(void) {}
#endif

/*************************** End of file ****************************/
