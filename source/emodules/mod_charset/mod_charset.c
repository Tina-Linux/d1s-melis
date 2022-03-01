/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                CharSet Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : charset.c
* By      : kevin.z
* Version : v1.0
* Date    : 2010-11-23 17:31
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-23 17:31    kevin.z     1.0     build the file.
*********************************************************************************************************
*/
#include "mod_charset.h"
#include "mod_charset_i.h"
#include <kconfig.h>


__s32 _CHS_Init(void)
{
    return 0;
}

void _CHS_Exit(void)
{
    return;
}

__s32 CHS_Uni2Char(__s32 charset_type, __u16 uni, __u8 *str, __u32 len)
{
    __s32 ret;

    switch (charset_type)
    {
        case EPDK_CHARSET_ENM_GB2312 :
        case EPDK_CHARSET_ENM_GBK:
            ret = GBK_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_BIG5:
            ret = BIG5_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_SJIS:
            ret = SJIS_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_EUC_KR:
            ret = EUCKR_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_1:
            ret = ISO8859_1_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_UTF8:
            ret = UTF8_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_UTF16BE:
            ret = UTF16BE_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_UTF16LE:
            ret = UTF16LE_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_KIO8_R:
            ret = KIO8_R_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_5:
            ret = ISO8859_5_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_2:
            ret = ISO8859_2_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_3:
            ret = ISO8859_3_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_4:
            ret = ISO8859_4_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_6:
            ret = ISO8859_6_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_7:
            ret = ISO8859_7_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_9:
            ret = ISO8859_9_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_13:
            ret = ISO8859_13_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_14:
            ret = ISO8859_14_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_15:
            ret = ISO8859_15_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_CP874  :
            ret = CP874_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_CP1250 :
            ret = CP1250_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_CP1251 :
            ret = CP1251_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_CP1253 :
            ret = CP1253_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_CP1255 :
            ret = CP1255_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_8:
            ret = ISO8859_8_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_10:
            ret = ISO8859_10_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_11:
            ret = ISO8859_11_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_16:
            ret = ISO8859_16_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_CP1252 :
            ret = CP1252_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_CP1254 :
            ret = CP1254_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_CP1256 :
            ret = CP1256_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_CP1257 :
            ret = CP1257_uni2char(uni, str, len);
            break;

        case EPDK_CHARSET_ENM_EUC_JP:
        case EPDK_CHARSET_ENM_CP1258 :
        case EPDK_CHARSET_ENM_ISO_8859_12:
            return 0;

        default:
            return 0;
    }

    if (ret < 0)
    {
        return 0;
    }

    return ret;
}


__s32 CHS_Char2Uni(__s32 charset_type, const __u8 *str, __u32 len, __u16 *uni)
{
    __s32 ret;

    switch (charset_type)
    {
        case EPDK_CHARSET_ENM_GB2312 :
        case EPDK_CHARSET_ENM_GBK:
            ret = GBK_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_BIG5:
            ret = BIG5_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_SJIS:
            ret = SJIS_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_EUC_KR:
            ret = EUCKR_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_1:
            ret = ISO8859_1_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_UTF8:
            ret = UTF8_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_UTF16BE:
            ret = UTF16BE_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_UTF16LE:
            ret = UTF16LE_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_KIO8_R:
            ret = KIO8_R_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_5:
            ret = ISO8859_5_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_2:
            ret = ISO8859_2_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_3:
            ret = ISO8859_3_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_4:
            ret = ISO8859_4_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_6:
            ret = ISO8859_6_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_7:
            ret = ISO8859_7_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_9:
            ret = ISO8859_9_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_13:
            ret = ISO8859_13_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_14:
            ret = ISO8859_14_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_15:
            ret = ISO8859_15_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_CP874  :
            ret = CP874_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_CP1250 :
            ret = CP1250_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_CP1251 :
            ret = CP1251_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_CP1253 :
            ret = CP1253_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_CP1255 :
            ret = CP1255_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_CP1252 :
            ret = CP1252_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_CP1254 :
            ret = CP1254_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_CP1256 :
            ret = CP1256_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_CP1257 :
            ret = CP1257_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_8:
            ret = ISO8859_8_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_10:
            ret = ISO8859_10_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_11:
            ret = ISO8859_11_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_ISO_8859_16:
            ret = ISO8859_16_char2uni(str, len, uni);
            break;

        case EPDK_CHARSET_ENM_EUC_JP:
        case EPDK_CHARSET_ENM_CP1258 :
        case EPDK_CHARSET_ENM_ISO_8859_12:
            return 0;

        default:
            return 0;
    }

    if (ret < 0)
    {
        return 0;
    }

    return ret;
}


__u32 CHS_GetChUpperTbl(__s32 charset_type, void *buf, __u32 size)
{
    switch (charset_type)
    {
        case EPDK_CHARSET_ENM_GB2312 :
        case EPDK_CHARSET_ENM_GBK:
            return GBK_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_BIG5:
            return BIG5_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_SJIS:
            return SJIS_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_EUC_KR:
            return EUCKR_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_1:
            return ISO8859_1_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_UTF8:
            return UTF8_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_UTF16BE:
            return UTF16BE_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_UTF16LE:
            return UTF16LE_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_KIO8_R:
            return KIO8_R_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_5:
            return ISO8859_5_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_2:
            return ISO8859_2_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_3:
            return ISO8859_3_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_4:
            return ISO8859_4_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_6:
            return ISO8859_6_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_7:
            return ISO8859_7_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_9:
            return ISO8859_9_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_13:
            return ISO8859_13_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_14:
            return ISO8859_14_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_15:
            return ISO8859_15_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_CP874  :
            return CP874_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_CP1250 :
            return CP1250_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_CP1251 :
            return CP1251_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_CP1253 :
            return CP1253_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_CP1255 :
            return CP1255_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_CP1252 :
            return CP1252_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_CP1254 :
            return CP1254_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_CP1256 :
            return CP1256_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_CP1257 :
            return CP1257_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_8:
            return ISO8859_8_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_10:
            return ISO8859_10_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_11:
            return ISO8859_11_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_16:
            return ISO8859_16_get_charset2upper_table(buf, size);

        case EPDK_CHARSET_ENM_EUC_JP:
        case EPDK_CHARSET_ENM_CP1258 :
        case EPDK_CHARSET_ENM_ISO_8859_12:
            return 0;

        default :
            return 0;
    }
}


__u32 CHS_GetChLowerTbl(__s32 charset_type, void *buf, __u32 size)
{
    switch (charset_type)
    {
        case EPDK_CHARSET_ENM_GB2312 :
        case EPDK_CHARSET_ENM_GBK:
            return GBK_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_BIG5:
            return BIG5_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_SJIS:
            return SJIS_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_EUC_KR:
            return EUCKR_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_1:
            return ISO8859_1_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_UTF8:
            return UTF8_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_UTF16BE:
            return UTF16BE_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_UTF16LE:
            return UTF16LE_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_KIO8_R:
            return KIO8_R_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_5:
            return ISO8859_5_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_2:
            return ISO8859_2_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_3:
            return ISO8859_3_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_4:
            return ISO8859_4_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_6:
            return ISO8859_6_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_7:
            return ISO8859_7_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_9:
            return ISO8859_9_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_13:
            return ISO8859_13_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_14:
            return ISO8859_14_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_15:
            return ISO8859_15_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_CP874  :
            return CP874_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_CP1250 :
            return CP1250_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_CP1251 :
            return CP1251_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_CP1253 :
            return CP1253_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_CP1255 :
            return CP1255_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_CP1252 :
            return CP1252_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_CP1254 :
            return CP1254_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_CP1256 :
            return CP1256_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_CP1257 :
            return CP1257_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_8:
            return ISO8859_8_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_10:
            return ISO8859_10_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_11:
            return ISO8859_11_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_ISO_8859_16:
            return ISO8859_16_get_charset2lower_table(buf, size);

        case EPDK_CHARSET_ENM_EUC_JP:
        case EPDK_CHARSET_ENM_CP1258 :
        case EPDK_CHARSET_ENM_ISO_8859_12:
            return 0;

        default :
            return 0;
    }
}
