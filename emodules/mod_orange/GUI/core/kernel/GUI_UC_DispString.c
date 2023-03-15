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
*       _GetLineDistX
*/
static int _GetLineDistX(const U16 GUI_UNI_PTR *s, int MaxNumChars)
{
    int Dist = 0;

    if (s)
    {
        U16 Char;

        while (((Char = *s) != 0) && MaxNumChars >= 0)
        {
            s++;
            MaxNumChars--;
            Dist += GUI_GetCharDistX(Char);
        }
    }

    return Dist;
}

/*********************************************************************
*
*       _GetLineLen
*/
static int _GetLineLen(const U16 GUI_UNI_PTR *s, int MaxLen)
{
    int Len = 0;

    if (!s)
    {
        return 0;
    }

    {
        while ((*s != 0) && Len < MaxLen)
        {
            Len++;
            s++;
        }
    }

    return Len;
}

/*********************************************************************
*
*       _DispLine_UC
*/
static void _DispLine_UC(const U16 GUI_UNI_PTR *s, int Len, const GUI_RECT *pRect)
{
    if (GUI_Context.pClipRect_HL[GUI_Context.SelLayer])
    {
        if (GUI_RectsIntersect(GUI_Context.pClipRect_HL[GUI_Context.SelLayer], pRect) == 0)
        {
            return;
        }
    }

    {
        U16 c0;
        c0 = *s;

        if ((c0 == '\n') || (c0 == '\r')) //modified by Derek 2010.12.02.10:10
        {
            GUI_DispNextLine();
        }
        else
        {
            while (--Len >= 0)
            {
                c0 = *s++;
                GL_DispChar(c0);
            }
        }
    }
}

/*********************************************************************
*
*       _DispLine
*/
static void _DispLine(const U16 GUI_UNI_PTR *s, int Len, const GUI_RECT *pr)
{
    GUI_RECT r;
    r = *pr;
#if GUI_WINSUPPORT
    WM_ADDORG(r.x0, r.y0);
    WM_ADDORG(r.x1, r.y1);
    WM_ITERATE_START(&r)
    {
#endif
        GUI_Context.DispPosX[GUI_Context.SelLayer] = r.x0;
        GUI_Context.DispPosY[GUI_Context.SelLayer] = r.y0;
        _DispLine_UC(s, Len, &r);    /* Do the actual drawing via routine call. */
#if GUI_WINSUPPORT
    }
    WM_ITERATE_END();
    WM_SUBORG(GUI_Context.DispPosX[GUI_Context.SelLayer], GUI_Context.DispPosY[GUI_Context.SelLayer]);
#endif
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_UC_DispString
*/
void GUI_UC_DispString(const U16 GUI_UNI_PTR *s)
{
    int xAdjust, yAdjust, xOrg;
    int FontSizeY;

    if (!s)
    {
        return;
    }

    GUI_LOCK();
    FontSizeY = GUI_Context.pAFont->YSize;
    xOrg = GUI_Context.DispPosX[GUI_Context.SelLayer];
    /* Adjust vertical position */
    yAdjust = GUI_GetYAdjust();
    GUI_Context.DispPosY[GUI_Context.SelLayer] -= yAdjust;

    for (; *s; s++)
    {
        GUI_RECT r;
        int LineLen = _GetLineLen(s, 0x7fff);
        int xLineSize = _GetLineDistX(s, LineLen);

        /* Check if x-position needs to be changed due to h-alignment */
        switch (GUI_Context.TextAlign & GUI_TA_HORIZONTAL)
        {
            case GUI_TA_CENTER:
                xAdjust = xLineSize / 2;
                break;

            case GUI_TA_RIGHT:
                xAdjust = xLineSize;
                break;

            default:
                xAdjust = 0;
        }

        r.x0 = GUI_Context.DispPosX[GUI_Context.SelLayer] -= xAdjust;
        r.x1 = r.x0 + xLineSize - 1;
        r.y0 = GUI_Context.DispPosY[GUI_Context.SelLayer];
        r.y1 = r.y0 + FontSizeY - 1;
        _DispLine(s, LineLen, &r);
        GUI_Context.DispPosY[GUI_Context.SelLayer] = r.y0;
        s += LineLen;

        if (*s == '\n')
        {
            switch (GUI_Context.TextAlign & GUI_TA_HORIZONTAL)
            {
                case GUI_TA_CENTER:
                case GUI_TA_RIGHT:
                    GUI_Context.DispPosX[GUI_Context.SelLayer] = xOrg;
                    break;

                default:
                    GUI_Context.DispPosX[GUI_Context.SelLayer] = GUI_Context.LBorder;
                    break;
            }

            GUI_Context.DispPosY[GUI_Context.SelLayer] += GUI_GetFontDistY();
        }
        else
        {
            GUI_Context.DispPosX[GUI_Context.SelLayer] = r.x0 + xLineSize;
        }

        if (*s == 0)  /* end of string (last line) reached ? */
        {
            break;
        }
    }

    GUI_Context.DispPosY[GUI_Context.SelLayer] += yAdjust;
    GUI_Context.TextAlign &= ~GUI_TA_HORIZONTAL;
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
