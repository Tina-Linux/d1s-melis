/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              Apps Sub-System
*
*                                   (c) Copyright 2007-2009, john.fu <john.fu@allwinnerme.com>
*                                             All Rights Reserved
*
* Moudle  : Charset
* File    : utf16BE.c
*
* By      : john.fu
* Version : v1.0
* Date    : 2009-8-25
**********************************************************************************************************************
*/
#include "mod_charset_i.h"

__s32 UTF16BE_uni2char(__u16 uni, __u8 *out, __u32 boundlen)
{
    if (boundlen <= 1)
    {
        return -1;
    }

    out[0] = (uni >> 8) & 0xFF;
    out[1] = uni & 0xFF;
    return 2;
}


__s32 UTF16BE_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni)
{
    __u16 out;

    if (boundlen <= 1)
    {
        return -1;
    }

    out = rawstring[0] & 0xFF;
    *uni = (out << 8) | (rawstring[1] & 0xFF);
    return 2;
}

__u32 UTF16BE_get_charset2upper_table(void *buf, __u32 size)
{
    return 0;
}

__u32 UTF16BE_get_charset2lower_table(void *buf, __u32 size)
{
    return 0;
}

