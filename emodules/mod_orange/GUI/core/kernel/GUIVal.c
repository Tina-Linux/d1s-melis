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
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DispDecShift
*/
void GUI_DispDecShift(I32 v, U8 Len, U8 Shift)
{
    char ac[12];
    char *s = ac;
    GUI_AddDecShift(v, Len, Shift, &s);
    GUI_DispString(ac);
}

/*********************************************************************
*
*       GUI_DispSDecShift
*/
void GUI_DispSDecShift(I32 v, U8 Len, U8 Shift)
{
    char ac[12];
    char *s = ac;
    v = GUI_AddSign(v, &s);
    GUI_AddDecShift(v, --Len, Shift, &s);
    GUI_DispString(ac);
}

/*********************************************************************
*
*       GUI_DispDecMin
*/
void GUI_DispDecMin(I32 v)
{
    char ac[12];
    char *s = ac;
    GUI_AddDecMin(v, &s);
    GUI_DispString(ac);
}

/*********************************************************************
*
*       GUI_DispDecSpace
*/
void GUI_DispDecSpace(I32 v, U8 MaxDigits)
{
    char ac[12];
    char *s = ac;
    U8 Len;

    if (v < 0)
    {
        v = GUI_AddSign(v, &s);
        MaxDigits--;
    }

    Len = GUI_Long2Len(v);

    while (Len++ < MaxDigits)
    {
        *s++ = ' ';
    }

    GUI_AddDecMin(v, &s);
    GUI_DispString(ac);
}

/*********************************************************************
*
*       GUI_DispSDec
*/
void GUI_DispSDec(I32 v, U8 Len)
{
    GUI_DispSDecShift(v, Len, 0);
}

/*********************************************************************
*
*       GUI_DispDec
*/
void GUI_DispDec(I32 v, U8 Len)
{
    GUI_DispDecShift(v, Len, 0);
}

/*********************************************************************
*
*       GUI_DispDecAt
*/
void GUI_DispDecAt(I32 v, I16P x, I16P y, U8 Len)
{
    char ac[12];
    char *s = ac;
    GUI_AddDecShift(v, Len, 0, &s);
    GUI_DispStringAt(ac, x, y);
}

/*************************** End of file ****************************/
