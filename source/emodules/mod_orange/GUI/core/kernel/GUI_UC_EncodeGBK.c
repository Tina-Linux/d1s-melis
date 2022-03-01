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
* File    : GUI_UC_EncodeGBK.C
* Purpose : Convert GB312 to ASCII
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 16:47:34
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetCharCode
*
*
*
*/
static U16 _GetCharCode(const char GUI_UNI_PTR *s)
{
    U16 r;

    if (esCHS_Char2Uni(EPDK_CHARSET_ENM_GBK, (__u8 *)s, 8, &r) <= 0)
    {
        r = 1;
    }

    return r;
}

/*********************************************************************
*
*       _GetCharSize
*
*
*
*/
static int _GetCharSize(const char GUI_UNI_PTR *s)
{
    U8 ch;
    ch = *s;

    if (ch > 0x80)
    {
        return 2;
    }
    else if (ch < 0x80)
    {
        return 1;
    }
    else
    {
        return 1;
    }
}
/*********************************************************************
*
*       _CalcSizeOfChar
*
*
*
*/
static int _CalcSizeOfChar(U16 Char)
{
    int r = 1;

    if ((Char >= 0x2e00) && (Char <= 0x9fff)) /* Single byte (ASCII)  */
    {
        r = 2;
    }
    else if (Char < 0x80)                /* Double byte sequence */
    {
        r = 1;
    }

    return r;
}

/*********************************************************************
*
*       _Encode
*
*
*/
static int _Encode(char *s, U16 Char)
{
    int r;
    r = esCHS_Uni2Char(EPDK_CHARSET_ENM_GBK, Char, (__u8 *)s, 3);

    if (r <= 0)
    {
        return 1;
    }

    return r;
}


/*********************************************************************
*
*       _API_Table
*/
static const GUI_UC_ENC_APILIST _API_Table =
{
    _GetCharCode,     /*  return character code as ASCII */
    _GetCharSize,     /*  return size of character        */
    _CalcSizeOfChar,  /*  return size of character       */
    _Encode           /*  Encode character into 1 or 2     */
};

/*********************************************************************
*
*       Exported code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_UC_SetEncodeGB2312
*/
void GUI_UC_SetEncodeGBK(void)
{
    GUI_LOCK();
    GUI_Context.pUC_API = &_API_Table;
    GUI_Context.charset_enum = EPDK_CHARSET_ENM_GBK;
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
