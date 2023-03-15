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
#ifndef __MOD_CHARSET_I_H__
#define __MOD_CHARSET_I_H__

#include "typedef.h"
#include <kconfig.h>

typedef enum
{
    EINVAL = 1,
    ENAMETOOLONG,
    CS_ERR_
} charset_err_e;

#undef  MIN
#define MIN(x, y)       ( (x) < (y) ? (x) : (y) )

#undef  MAX
#define MAX(x, y)       ( (x) > (y) ? (x) : (y) )

//config which charset need be support
#ifdef     CONFIG_CHARSET_BIG5_EN
extern  __s32 BIG5_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 BIG5_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 BIG5_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 BIG5_get_charset2lower_table(void *buf, __u32 size);
#else
#define BIG5_uni2char(...)                              (0)
#define BIG5_char2uni(...)                              (0)
#define BIG5_get_charset2upper_table(...)               (0)
#define BIG5_get_charset2lower_table(...)               (0)
#endif


#ifdef     CONFIG_CHARSET_CP874_EN
extern  __s32 CP874_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 CP874_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 CP874_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 CP874_get_charset2lower_table(void *buf, __u32 size);
#else
#define CP874_uni2char(...)                                 (0)
#define CP874_char2uni(...)                                 (0)
#define CP874_get_charset2upper_table(...)                  (0)
#define CP874_get_charset2lower_table(...)                  (0)

#endif


#ifdef     CONFIG_CHARSET_CP1250_EN
extern  __s32 CP1250_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 CP1250_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 CP1250_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 CP1250_get_charset2lower_table(void *buf, __u32 size);
#else
#define CP1250_uni2char(...)                                    (0)
#define CP1250_char2uni(...)                                    (0)
#define CP1250_get_charset2upper_table(...)                     (0)
#define CP1250_get_charset2lower_table(...)                     (0)
#endif


#ifdef     CONFIG_CHARSET_CP1251_EN
extern  __s32 CP1251_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 CP1251_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 CP1251_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 CP1251_get_charset2lower_table(void *buf, __u32 size);
#else
#define CP1251_uni2char(...)                                    (0)
#define CP1251_char2uni(...)                                    (0)
#define CP1251_get_charset2upper_table(...)                     (0)
#define CP1251_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_CP1253_EN
extern  __s32 CP1253_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 CP1253_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 CP1253_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 CP1253_get_charset2lower_table(void *buf, __u32 size);
#else
#define CP1253_uni2char(...)                                    (0)
#define CP1253_char2uni(...)                                    (0)
#define CP1253_get_charset2upper_table(...)                     (0)
#define CP1253_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_CP1255_EN
extern  __s32 CP1255_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 CP1255_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 CP1255_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 CP1255_get_charset2lower_table(void *buf, __u32 size);
#else
#define CP1255_uni2char(...)                                    (0)
#define CP1255_char2uni(...)                                    (0)
#define CP1255_get_charset2upper_table(...)                     (0)
#define CP1255_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_EUC_KR_EN
extern  __s32 EUCKR_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 EUCKR_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 EUCKR_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 EUCKR_get_charset2lower_table(void *buf, __u32 size);
#else
#define EUCKR_uni2char(...)                                    (0)
#define EUCKR_char2uni(...)                                    (0)
#define EUCKR_get_charset2upper_table(...)                     (0)
#define EUCKR_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_GBK_EN
extern  __s32 GBK_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 GBK_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 GBK_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 GBK_get_charset2lower_table(void *buf, __u32 size);
#else
#define GBK_uni2char(...)                                    (0)
#define GBK_char2uni(...)                                    (0)
#define GBK_get_charset2upper_table(...)                     (0)
#define GBK_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_1_EN
extern  __s32 ISO8859_1_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_1_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_1_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_1_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_1_uni2char(...)                                    (0)
#define ISO8859_1_char2uni(...)                                    (0)
#define ISO8859_1_get_charset2upper_table(...)                     (0)
#define ISO8859_1_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_2_EN
extern  __s32 ISO8859_2_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_2_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_2_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_2_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_2_uni2char(...)                                    (0)
#define ISO8859_2_char2uni(...)                                    (0)
#define ISO8859_2_get_charset2upper_table(...)                     (0)
#define ISO8859_2_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_3_EN
extern  __s32 ISO8859_3_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_3_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_3_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_3_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_3_uni2char(...)                                    (0)
#define ISO8859_3_char2uni(...)                                    (0)
#define ISO8859_3_get_charset2upper_table(...)                     (0)
#define ISO8859_3_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_4_EN
extern  __s32 ISO8859_4_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_4_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_4_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_4_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_4_uni2char(...)                                    (0)
#define ISO8859_4_char2uni(...)                                    (0)
#define ISO8859_4_get_charset2upper_table(...)                     (0)
#define ISO8859_4_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_5_EN
extern  __s32 ISO8859_5_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_5_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_5_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_5_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_5_uni2char(...)                                    (0)
#define ISO8859_5_char2uni(...)                                    (0)
#define ISO8859_5_get_charset2upper_table(...)                     (0)
#define ISO8859_5_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_6_EN
extern  __s32 ISO8859_6_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_6_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_6_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_6_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_6_uni2char(...)                                    (0)
#define ISO8859_6_char2uni(...)                                    (0)
#define ISO8859_6_get_charset2upper_table(...)                     (0)
#define ISO8859_6_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_7_EN
extern  __s32 ISO8859_7_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_7_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_7_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_7_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_7_uni2char(...)                                    (0)
#define ISO8859_7_char2uni(...)                                    (0)
#define ISO8859_7_get_charset2upper_table(...)                     (0)
#define ISO8859_7_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_9_EN
extern  __s32 ISO8859_9_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_9_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_9_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_9_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_9_uni2char(...)                                    (0)
#define ISO8859_9_char2uni(...)                                    (0)
#define ISO8859_9_get_charset2upper_table(...)                     (0)
#define ISO8859_9_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_13_EN
extern  __s32 ISO8859_13_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_13_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_13_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_13_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_13_uni2char(...)                                    (0)
#define ISO8859_13_char2uni(...)                                    (0)
#define ISO8859_13_get_charset2upper_table(...)                     (0)
#define ISO8859_13_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_14_EN
extern  __s32 ISO8859_14_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_14_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_14_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_14_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_14_uni2char(...)                                    (0)
#define ISO8859_14_char2uni(...)                                    (0)
#define ISO8859_14_get_charset2upper_table(...)                     (0)
#define ISO8859_14_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_15_EN
extern  __s32 ISO8859_15_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_15_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_15_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_15_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_15_uni2char(...)                                    (0)
#define ISO8859_15_char2uni(...)                                    (0)
#define ISO8859_15_get_charset2upper_table(...)                     (0)
#define ISO8859_15_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_KIO8_R_EN
extern  __s32 KIO8_R_uni2char(__u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 KIO8_R_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 KIO8_R_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 KIO8_R_get_charset2lower_table(void *buf, __u32 size);
#else
#define KIO8_R_uni2char(...)                                    (0)
#define KIO8_R_char2uni(...)                                    (0)
#define KIO8_R_get_charset2upper_table(...)                     (0)
#define KIO8_R_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_SJIS_EN
extern  __s32 SJIS_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 SJIS_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 SJIS_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 SJIS_get_charset2lower_table(void *buf, __u32 size);
#else
#define SJIS_uni2char(...)                                    (0)
#define SJIS_char2uni(...)                                    (0)
#define SJIS_get_charset2upper_table(...)                     (0)
#define SJIS_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_UTF8_EN
extern  __s32 UTF8_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 UTF8_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 UTF8_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 UTF8_get_charset2lower_table(void *buf, __u32 size);
#else
#define UTF8_uni2char(...)                                    (0)
#define UTF8_char2uni(...)                                    (0)
#define UTF8_get_charset2upper_table(...)                     (0)
#define UTF8_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_UTF16BE_EN
extern  __s32 UTF16BE_uni2char(__u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 UTF16BE_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 UTF16BE_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 UTF16BE_get_charset2lower_table(void *buf, __u32 size);
#else
#define UTF16BE_uni2char(...)                                    (0)
#define UTF16BE_char2uni(...)                                    (0)
#define UTF16BE_get_charset2upper_table(...)                     (0)
#define UTF16BE_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_UTF16LE_EN
extern  __s32 UTF16LE_uni2char(__u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 UTF16LE_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 UTF16LE_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 UTF16LE_get_charset2lower_table(void *buf, __u32 size);
#else
#define UTF16LE_uni2char(...)                                    (0)
#define UTF16LE_char2uni(...)                                    (0)
#define UTF16LE_get_charset2upper_table(...)                     (0)
#define UTF16LE_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_CP1252_EN
extern  __s32 CP1252_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 CP1252_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 CP1252_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 CP1252_get_charset2lower_table(void *buf, __u32 size);
#else
#define CP1252_uni2char(...)                                    (0)
#define CP1252_char2uni(...)                                    (0)
#define CP1252_get_charset2upper_table(...)                     (0)
#define CP1252_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_CP1254_EN
extern  __s32 CP1254_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 CP1254_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 CP1254_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 CP1254_get_charset2lower_table(void *buf, __u32 size);
#else
#define CP1254_uni2char(...)                                    (0)
#define CP1254_char2uni(...)                                    (0)
#define CP1254_get_charset2upper_table(...)                     (0)
#define CP1254_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_CP1256_EN
extern  __s32 CP1256_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 CP1256_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 CP1256_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 CP1256_get_charset2lower_table(void *buf, __u32 size);
#else
#define CP1256_uni2char(...)                                    (0)
#define CP1256_char2uni(...)                                    (0)
#define CP1256_get_charset2upper_table(...)                     (0)
#define CP1256_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_CP1257_EN
extern  __s32 CP1257_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 CP1257_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 CP1257_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 CP1257_get_charset2lower_table(void *buf, __u32 size);
#else
#define CP1257_uni2char(...)                                    (0)
#define CP1257_char2uni(...)                                    (0)
#define CP1257_get_charset2upper_table(...)                     (0)
#define CP1257_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_8_EN
extern  __s32 ISO8859_8_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_8_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_8_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_8_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_8_uni2char(...)                                    (0)
#define ISO8859_8_char2uni(...)                                    (0)
#define ISO8859_8_get_charset2upper_table(...)                     (0)
#define ISO8859_8_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_10_EN
extern  __s32 ISO8859_10_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_10_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_10_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_10_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_10_uni2char(...)                                    (0)
#define ISO8859_10_char2uni(...)                                    (0)
#define ISO8859_10_get_charset2upper_table(...)                     (0)
#define ISO8859_10_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_11_EN
extern  __s32 ISO8859_11_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_11_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_11_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_11_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_11_uni2char(...)                                    (0)
#define ISO8859_11_char2uni(...)                                    (0)
#define ISO8859_11_get_charset2upper_table(...)                     (0)
#define ISO8859_11_get_charset2lower_table(...)                     (0)
#endif

#ifdef     CONFIG_CHARSET_ISO_8859_16_EN
extern  __s32 ISO8859_16_uni2char(const __u16 uni, __u8 *out, __u32 boundlen);
extern  __s32 ISO8859_16_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni);
extern  __u32 ISO8859_16_get_charset2upper_table(void *buf, __u32 size);
extern  __u32 ISO8859_16_get_charset2lower_table(void *buf, __u32 size);
#else
#define ISO8859_16_uni2char(...)                                    (0)
#define ISO8859_16_char2uni(...)                                    (0)
#define ISO8859_16_get_charset2upper_table(...)                     (0)
#define ISO8859_16_get_charset2lower_table(...)                     (0)
#endif

extern  __s32 _CHS_Init(void);
extern  void _CHS_Exit(void);
extern  __s32 CHS_Uni2Char(__s32 charset_type, __u16 uni, __u8 *str, __u32 len);
extern  __s32 CHS_Char2Uni(__s32 charset_type, const __u8 *str, __u32 len, __u16 *uni);
extern  __u32 CHS_GetChUpperTbl(__s32 charset_type, void *buf, __u32 size);
extern  __u32 CHS_GetChLowerTbl(__s32 charset_type, void *buf, __u32 size);


#endif  //__CHARSET_H__
