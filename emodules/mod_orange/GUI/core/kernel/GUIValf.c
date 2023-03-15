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
#include "math.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DispFloatFix
*/
static void _DispFloatFix(float f, char Len, char Decs, int DrawPlusSign)
{
    f *= GUI_Pow10[(unsigned)Decs];
    f += 0.5;
    f = (float) floor(f);

    if (DrawPlusSign)
    {
        GUI_DispSDecShift((long)f, Len, Decs);
    }
    else
    {
        GUI_DispDecShift((long)f, Len, Decs);
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
*       GUI_DispFloatFix
*/
void GUI_DispFloatFix(float f, char Len, char Decs)
{
    _DispFloatFix(f, Len, Decs, 0);
}

/*********************************************************************
*
*       GUI_DispFloatMin
*/
void GUI_DispFloatMin(float f, char Fract)
{
    char Len;
    Len = GUI_Long2Len((long)f);

    if ((f < 0) && (f > -1))   /* If value < 0 and > -1 (e.g. -0.123) increment length by 1 */
    {
        Len++;
    }

    _DispFloatFix(f, (char)(Len + Fract + (Fract ? 1 : 0)), (char)Fract, 0);
}

/*********************************************************************
*
*       GUI_DispFloat
*/
void GUI_DispFloat(float f, char Len)
{
    int Decs;
    Decs =  Len - GUI_Long2Len((long)f) - 1;

    if ((f < 0) && (f > -1))   /* If value < 0 and > -1 (e.g. -0.123) decrement Decs */
    {
        Decs--;
    }

    if (Decs < 0)
    {
        Decs = 0;
    }

    _DispFloatFix(f, Len, (char)Decs, 0);
}

/*********************************************************************
*
*       GUI_DispSFloatFix
*/
void GUI_DispSFloatFix(float f, char Len, char Fract)
{
    _DispFloatFix(f, Len, Fract, 1);
}

/*********************************************************************
*
*       GUI_DispSFloatMin
*/
void GUI_DispSFloatMin(float f, char Fract)
{
    char Len;
    Len = GUI_Long2Len((long)f);

    if ((f < 0) && (f > -1))   /* If value < 0 and > -1 (e.g. -0.123) increment length by 1 */
    {
        Len++;
    }

    if (f > 0)
    {
        Len++;
    }

    _DispFloatFix(f, (char)(Len + Fract + (Fract ? 1 : 0)), (char)Fract, 1);
}

/*************************** End of file ****************************/
