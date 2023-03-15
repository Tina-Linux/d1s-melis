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
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_UC_GetCharSize
*/
int GUI_UC_GetCharSize(const char GUI_UNI_PTR *s)
{
    int r;
    GUI_LOCK();
    r =  GUI_Context.pUC_API->pfGetCharSize(s);
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_UC_GetCharCode
*/
U16 GUI_UC_GetCharCode(const char GUI_UNI_PTR *s)
{
    U16 r;
    GUI_LOCK();
    r =  GUI_Context.pUC_API->pfGetCharCode(s);
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_UC_Encode
*/
int GUI_UC_Encode(char *s, U16 Char)
{
#if GUI_COMPILER_SUPPORTS_FP
    int r;
    GUI_LOCK();
    r = GUI_Context.pUC_API->pfEncode(s, Char);
    GUI_UNLOCK();
    return r;
#else
    GUI_USE_PARA(s);
    GUI_USE_PARA(Char);
    return 0;
#endif
}

/*********************************************************************
*
*       GUI_UC__CalcSizeOfChar
*/
int GUI_UC__CalcSizeOfChar(U16 Char)
{
    return GUI_Context.pUC_API->pfCalcSizeOfChar(Char);
}

/*********************************************************************
*
*       GUI_UC__GetCharCodeInc
*/
U16 GUI_UC__GetCharCodeInc(const char GUI_UNI_PTR **ps)
{
    const char GUI_UNI_PTR *s;
    U16 r;
    s   = *ps;
    r   = GUI_UC__GetCharCode(s);
    s  += GUI_UC__GetCharSize(s);
    *ps = s;
    return r;
}

/*********************************************************************
*
*       GUI_UC__NumChars2NumBytes
*/
int GUI_UC__NumChars2NumBytes(const char GUI_UNI_PTR *s, int NumChars)
{
    int CharSize, NumBytes = 0;

    while (NumChars--)
    {
        CharSize = GUI_UC__GetCharSize(s);
        s += CharSize;
        NumBytes += CharSize;
    }

    return NumBytes;
}

/*********************************************************************
*
*       GUI_UC__NumBytes2NumChars
*/
int GUI_UC__NumBytes2NumChars(const char GUI_UNI_PTR *s, int NumBytes)
{
    int CharSize, Chars = 0, Bytes = 0;

    while (NumBytes > Bytes)
    {
        CharSize = GUI_UC__GetCharSize(s + Bytes);
        Bytes += CharSize;
        Chars++;
    }

    return Chars;
}

/*************************** End of file ****************************/
