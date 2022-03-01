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
* File    : GUI_UC_EncodeBig5.C
* Purpose : Encoding routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-5-6 16:27:41
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
* Purpose:
*   Return the UNICODE character code of the current character.
*/
static U16 _GetCharCode(const char GUI_UNI_PTR *s)
{
    U16 r;

    if (esCHS_Char2Uni(EPDK_CHARSET_ENM_BIG5, (__u8 *)s, 8, &r) <= 0)
    {
        r = 1;
    }

    return r;
}

/*********************************************************************
*
*       _GetCharSize
*
* Purpose:
*   Return the number of bytes of the current character.
*/
static int _GetCharSize(const char GUI_UNI_PTR *s)
{
    U8 ch;
    ch = *s;

    if (ch > 0xa0)
    {
        return 2;
    }
    else if (ch < 0xa0)
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
* Purpose:
*   Return the number of bytes needed for the given character.
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
* Purpose:
*   Encode character into 1/2/3 bytes.
*/
static int _Encode(char *s, U16 Char)
{
    int r;
    r = esCHS_Uni2Char(EPDK_CHARSET_ENM_BIG5, Char, (__u8 *)s, 3);

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
    _GetCharCode,     /*  return character code as U16 (Unicode) */
    _GetCharSize,     /*  return size of character: 1/2/3        */
    _CalcSizeOfChar,  /*  return size of character: 1/2/3        */
    _Encode           /*  Encode character into 1/2/3 bytes      */
};

/*********************************************************************
*
*       Exported code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_UC_SetEncodeBig5
*/
void GUI_UC_SetEncodeBig5(void)
{
    GUI_LOCK();
    GUI_Context.pUC_API = &_API_Table;
    GUI_Context.charset_enum = EPDK_CHARSET_ENM_BIG5;
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
