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
*       _DispLine
*/
static void _DispLine(const char GUI_UNI_PTR *s, int MaxNumChars, const GUI_RECT *pRect)
{
    /* Check if we have anything to do at all ... */
    if (GUI_Context.pClipRect_HL[GUI_Context.SelLayer])
    {
        if (GUI_RectsIntersect(GUI_Context.pClipRect_HL[GUI_Context.SelLayer], pRect) == 0)
        {
            return;
        }
    }

#if GUI_COMPILER_SUPPORTS_FP

    if (GUI_Context.pAFont->pafEncode)
    {
        GUI_Context.pAFont->pafEncode->pfDispLine(s, MaxNumChars);
    }
    else
    {
#else
    {
#endif
        U16 Char;

        while (--MaxNumChars >= 0)
        {
            Char = GUI_UC__GetCharCodeInc(&s);
            GUI_Context.pAFont->pfDispChar(Char);
            //if (GUI_pfDispCharStyle) {
            //GUI_pfDispCharStyle(Char);
            //}
        }
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
*       GUI__GetLineNumChars
*/
int GUI__GetLineNumChars(const char GUI_UNI_PTR *s, int MaxNumChars)
{
    int NumChars = 0;

    if (s)
    {
#if GUI_COMPILER_SUPPORTS_FP

        if (GUI_Context.pAFont->pafEncode)
        {
            //2008/07/22 9:08:22
            if (GUI_Context.pAFont->pafEncode->pfGetLineLen) //mcl add  to avoid  Error by NULL pfGetLineLen
                //2008/07/22 9:08:33
            {
                return GUI_Context.pAFont->pafEncode->pfGetLineLen(s, MaxNumChars);
            }
        }

#endif

        for (; NumChars < MaxNumChars; NumChars++)
        {
            U16 Data = GUI_UC__GetCharCodeInc(&s);

            if ((Data == 0) || (Data == '\n') || (Data == '\r')) //modified ba Derek 2010.12.02.11:00
            {
                break;
            }
        }
    }

    return NumChars;
}

/*********************************************************************
*
*       GUI_GetLineDistX
*
*  This routine is used to calculate the length of a line in pixels.
*/
int GUI__GetLineDistX(const char GUI_UNI_PTR *s, int MaxNumChars)
{
    int Dist = 0;

    if (s)
    {
        U16 Char;
#if GUI_COMPILER_SUPPORTS_FP

        if (GUI_Context.pAFont->pafEncode)
        {
            //2008/07/22 9:11:08
            if (GUI_Context.pAFont->pafEncode->pfGetLineDistX) //mcl add  to avoid  Error by NULL pfGetLineDistX
                //2008/07/22 9:11:18
            {
                return GUI_Context.pAFont->pafEncode->pfGetLineDistX(s, MaxNumChars);
            }
        }

#endif

        while (--MaxNumChars >= 0)
        {
            Char  = GUI_UC__GetCharCodeInc(&s);
            Dist += GUI_GetCharDistX(Char);
        }
    }

    return Dist;
}

/*********************************************************************
*
*       GUI__DispLine
*/
void GUI__DispLine(const char GUI_UNI_PTR *s, int MaxNumChars, const GUI_RECT *pr)
{
    GUI_RECT r;
#if GUI_SUPPORT_ROTATION

    if (GUI_pLCD_APIList)
    {
#if GUI_WINSUPPORT
        WM_ITERATE_START(NULL)
        {
#endif
            /* Do the actual drawing via routine call. */
            _DispLine(s, MaxNumChars, &r);
#if GUI_WINSUPPORT
        }
        WM_ITERATE_END();
#endif
    }
    else
#endif
    {
        r = *pr;
#if GUI_WINSUPPORT
        WM_ADDORG(r.x0, r.y0);
        WM_ADDORG(r.x1, r.y1);
        WM_ITERATE_START(&r)
        {
#endif
            GUI_Context.DispPosX[GUI_Context.SelLayer] = r.x0;
            GUI_Context.DispPosY[GUI_Context.SelLayer] = r.y0;
            /* Do the actual drawing via routine call. */
            _DispLine(s, MaxNumChars, &r);
#if GUI_WINSUPPORT
        }
        WM_ITERATE_END();
        WM_SUBORG(GUI_Context.DispPosX[GUI_Context.SelLayer], GUI_Context.DispPosY[GUI_Context.SelLayer]);
#endif
    }
}

/*************************** End of file ****************************/
