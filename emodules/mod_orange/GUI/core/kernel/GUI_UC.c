/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUI_UC_EncodeUTF8.C
* Purpose : Encoding routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 16:47:34
**********************************************************************************************************************
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
