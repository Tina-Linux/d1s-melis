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
* File    : GUI_UC_EncodeNone.C
* Purpose : Encoding routines for non unicode systems (default)
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
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
    return *(const U8 GUI_UNI_PTR *)s;
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
    GUI_USE_PARA(s);
    return 1;
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
    GUI_USE_PARA(Char);
    return 1;
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
    *s = (U8)(Char);
    return 1;
}

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _API_Table
*/
const GUI_UC_ENC_APILIST GUI__API_TableNone =
{
    _GetCharCode,     /*  return character code as U16 */
    _GetCharSize,     /*  return size of character: 1 */
    _CalcSizeOfChar,  /*  return size of character: 1 */
    _Encode           /*  Encode character */
};

/*********************************************************************
*
*       Exported code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_UC_SetEncodeNone
*/
void GUI_UC_SetEncodeNone(void)
{
    GUI_LOCK();
    GUI_Context.pUC_API = &GUI__API_TableNone;
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
