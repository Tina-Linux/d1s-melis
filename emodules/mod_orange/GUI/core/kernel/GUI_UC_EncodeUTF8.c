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
    U8 Char = *(const U8 *)s;

    if ((Char & 0x80) == 0)                  /* Single byte (ASCII)  */
    {
        r = Char;
    }
    else if ((Char & 0xe0) == 0xc0)          /* Double byte sequence */
    {
        r = (Char & 0x1f) << 6;
        Char = *(++s);
        Char &= 0x3f;
        r |= Char;
    }
    else if ((Char & 0xf0) == 0xe0)          /* 3 byte sequence      */
    {
        r = (Char & 0x0f) << 12;
        Char = *(++s);
        Char &= 0x3f;
        r |= (Char << 6);
        Char = *(++s);
        Char &= 0x3f;
        r |= Char;
    }
    else
    {
        GUI_DEBUG_ERROROUT("Illegal character during UTF-8 decoding!");
        r = 1;       /* Illegal character. To avoid endless loops in upper layers, we return 1 rather than 0. */
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
    U8 Char = *s;

    if ((Char & 0x80) == 0)
    {
        return 1;
    }
    else if ((Char & 0xe0) == 0xc0)
    {
        return 2;
    }
    else if ((Char & 0xf0) == 0xe0)
    {
        return 3;
    }

    GUI_DEBUG_ERROROUT("Illegal character during UTF-8 decoding!");
    return 1;       /* Illegal character. To avoid endless loops in upper layers, we return 1 rather than 0. */
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
    int r;

    if (Char & 0xF800)                  /* Single byte (ASCII)  */
    {
        r = 3;
    }
    else if (Char & 0xFF80)             /* Double byte sequence */
    {
        r = 2;
    }
    else                                /* 3 byte sequence      */
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
    r = _CalcSizeOfChar(Char);

    switch (r)
    {
        case 1:
            *s = (char)Char;
            break;

        case 2:
            *s++ = 0xC0 | (Char >> 6);
            *s   = 0x80 | (Char & 0x3F);
            break;

        case 3:
            *s++ = 0xE0 | (Char >> 12);
            *s++ = 0x80 | ((Char >> 6) & 0x3F);
            *s   = 0x80 | (Char & 0x3F);
            break;
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
*       GUI_UC_SetEncodeUTF8
*/
void GUI_UC_SetEncodeUTF8(void)
{
    GUI_LOCK();
    GUI_Context.pUC_API = &_API_Table;
    GUI_Context.charset_enum = EPDK_CHARSET_ENM_UTF8;
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
