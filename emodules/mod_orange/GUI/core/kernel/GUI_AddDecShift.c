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
#include "string.h"

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/

const U32 GUI_Pow10[10] =
{
    1, 10, 100, 1000, 10000,
    100000, 1000000, 10000000, 100000000, 1000000000
};

/*********************************************************************
*
*       Static routines
*
**********************************************************************
*/
/*********************************************************************
*
*       _Check_NegLong
*/
static int _Check_NegLong(I32 *pv, char **ps)
{
    if (*pv < 0)
    {
        *(*ps)++ = '-';
        *pv = -*pv;
        return 1;
    }

    return 0;
}

/*********************************************************************
*
*       Module internal routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_Long2Len
*/
int GUI_Long2Len(I32 vSign)
{
    int Len = 1;
    I32 v = (vSign > 0) ? vSign : -vSign;

    while ((((U32)v) >= GUI_Pow10[Len]) && (Len < 9))
    {
        Len++;
    }

    if (vSign < 0)
    {
        Len++;
    }

    return Len;
}

/*********************************************************************
*
*       GUI_AddSign
*/
long GUI_AddSign(long v, char **ps)
{
    char c;

    if (v < 0)
    {
        c = '-';
        v = -v;
    }
    else
    {
        c = '+';
    }

    *(*ps)++ = c;
    **ps     = '\0';
    return v;
}

/*********************************************************************
*
*       Public routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_AddDecShift
*/
void GUI_AddDecShift(I32 v, U8 Len, U8 Shift, char **ps)
{
    char c;
    long d;
#ifndef _CM16C
    Len -= _Check_NegLong(&v, ps); /* TASKING: Tool internal error S003: asertion failed - please report */
#else

    if (v < 0)
    {
        *(*ps)++ = '-';
        v = -v;
    }

#endif

    if (Shift)
    {
        Len--;
    }

#if GUI_DEBUG_LEVEL >1

    if (Len > 9)
    {
        Len = 9;
        GUI_DEBUG_ERROROUT("Can not display more than 9 dec. digits");
    }

#endif

    if ((U32)v >= GUI_Pow10[Len])
    {
        v = GUI_Pow10[Len] - 1;
    }

    while (Len)
    {
        if (Len-- == Shift)
        {
            *(*ps)++ = GUI_DecChar;
        }

        d = GUI_Pow10[Len];
        c = (char)(v / d);
        v -= c * d;
        *(*ps)++ = c + '0';
    }

    **ps = 0;
}

/*************************** End of file ****************************/
