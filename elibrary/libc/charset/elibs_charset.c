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
#ifndef  __elibs_charset_c
#define  __elibs_charset_c
#include <typedef.h>
#include <sys_charset.h>
#include <mod_charset.h>
#include <libc.h>

#define x_strcmp              eLIBs_strcmp
#define x_strcpy              eLIBs_strcpy

typedef struct _charset_map_t
{
    char   name[EPDK_CHARSET_NAME_LEN_MAX + 1];
    __s32  enm;
} charset_map_t;


const static charset_map_t charset_map_tbl[ ] =
{
    { EPDK_CHARSET_NAME_UTF8, EPDK_CHARSET_ENM_UTF8        },
    { EPDK_CHARSET_NAME_UTF16BE, EPDK_CHARSET_ENM_UTF16BE     },
    { EPDK_CHARSET_NAME_UTF16LE, EPDK_CHARSET_ENM_UTF16LE     },
    { EPDK_CHARSET_NAME_BIG5, EPDK_CHARSET_ENM_BIG5        },
    { EPDK_CHARSET_NAME_GBK, EPDK_CHARSET_ENM_GBK         },
    { EPDK_CHARSET_NAME_SJIS, EPDK_CHARSET_ENM_SJIS        },
    { EPDK_CHARSET_NAME_EUC_JP, EPDK_CHARSET_ENM_EUC_JP      },
    { EPDK_CHARSET_NAME_EUC_KR, EPDK_CHARSET_ENM_EUC_KR      },
    { EPDK_CHARSET_NAME_KIO8_R, EPDK_CHARSET_ENM_KIO8_R      },
    { EPDK_CHARSET_NAME_ISO_8859_1, EPDK_CHARSET_ENM_ISO_8859_1  },
    { EPDK_CHARSET_NAME_ISO_8859_2, EPDK_CHARSET_ENM_ISO_8859_2  },
    { EPDK_CHARSET_NAME_ISO_8859_3, EPDK_CHARSET_ENM_ISO_8859_3  },
    { EPDK_CHARSET_NAME_ISO_8859_4, EPDK_CHARSET_ENM_ISO_8859_4  },
    { EPDK_CHARSET_NAME_ISO_8859_5, EPDK_CHARSET_ENM_ISO_8859_5  },
    { EPDK_CHARSET_NAME_ISO_8859_6, EPDK_CHARSET_ENM_ISO_8859_6  },
    { EPDK_CHARSET_NAME_ISO_8859_7, EPDK_CHARSET_ENM_ISO_8859_7  },
    { EPDK_CHARSET_NAME_ISO_8859_8, EPDK_CHARSET_ENM_ISO_8859_8  },
    { EPDK_CHARSET_NAME_ISO_8859_9, EPDK_CHARSET_ENM_ISO_8859_9  },
    { EPDK_CHARSET_NAME_ISO_8859_10, EPDK_CHARSET_ENM_ISO_8859_10 },
    { EPDK_CHARSET_NAME_ISO_8859_11, EPDK_CHARSET_ENM_ISO_8859_11 },
    { EPDK_CHARSET_NAME_ISO_8859_12, EPDK_CHARSET_ENM_ISO_8859_12 },
    { EPDK_CHARSET_NAME_ISO_8859_13, EPDK_CHARSET_ENM_ISO_8859_13 },
    { EPDK_CHARSET_NAME_ISO_8859_14, EPDK_CHARSET_ENM_ISO_8859_14 },
    { EPDK_CHARSET_NAME_ISO_8859_15, EPDK_CHARSET_ENM_ISO_8859_15 },
    { EPDK_CHARSET_NAME_ISO_8859_16, EPDK_CHARSET_ENM_ISO_8859_16 },
    { EPDK_CHARSET_NAME_CP874, EPDK_CHARSET_ENM_CP874       },
    { EPDK_CHARSET_NAME_CP1250, EPDK_CHARSET_ENM_CP1250      },
    { EPDK_CHARSET_NAME_CP1251, EPDK_CHARSET_ENM_CP1251      },
    { EPDK_CHARSET_NAME_CP1253, EPDK_CHARSET_ENM_CP1253      },
    { EPDK_CHARSET_NAME_CP1255, EPDK_CHARSET_ENM_CP1255      },
    { EPDK_CHARSET_NAME_CP1256, EPDK_CHARSET_ENM_CP1256      },
    { EPDK_CHARSET_NAME_CP1257, EPDK_CHARSET_ENM_CP1257      },
    { EPDK_CHARSET_NAME_CP1258, EPDK_CHARSET_ENM_CP1258      }
};


#define CHARSET_NR            ( sizeof(charset_map_tbl) / sizeof(charset_map_tbl[0]) )


/***************************************************************************************************
*Name        : eLIBs_charset_name2enm
*Prototype   : __s32  eLIBs_charset_name2enm( const char *charset_name )
*Arguments   : charset_name   input. the name of a charset
*Return      : the enum value of the charset. return EPDK_CHARSET_ENM_UNKNOWN, if fail.
*Description : get the enum value of the charset through its name
*Other       :
***************************************************************************************************/
__s32  eLIBs_charset_name2enm(const char *charset_name)
{
    __u32  i;

    for (i = 0;  i < CHARSET_NR;  i++)
    {
        if (x_strcmp(charset_name, charset_map_tbl[i].name) == 0)
        {
            return charset_map_tbl[i].enm;
        }
    }

    return EPDK_CHARSET_ENM_UNKNOWN;
}


/***************************************************************************************************
*Name        : eLIBs_charset_enm2name
*Prototype   : __s32  eLIBs_charset_enm2name( __s32 charset_enm, char *charset_name )
*Arguments   : charset_enm    input. the enum value of a charset
*              charset_name   output.the name of charset
*Return      : ==0      succeed
*              !=0      fail
*Description : get the name of the charset through its enum value
*Other       :
***************************************************************************************************/
__s32  eLIBs_charset_enm2name(__s32 charset_enm, char *charset_name)
{
    __u32  i;

    if (charset_name == NULL)
    {
        return -1;
    }

    for (i = 0;  i < CHARSET_NR;  i++)
    {
        if (charset_enm == charset_map_tbl[i].enm)
        {
            x_strcpy(charset_name, charset_map_tbl[i].name);
            return 0;
        }
    }

    return -1;
}



/***************************************************************************************************
*Name        : eLIBs_charset_convert_size
*Prototype   : __u32  eLIBs_charset_convert_size( __s32 src_charset, __s32 dst_charset,
                                                  const __u8 *src, __u32 src_size );
*Arguments   : src_charset   input. the charset of the source byte steam
*              dst_charset   input. the charset of the destination byte steam
*              src           input. the source byte stream
*              src_size      input. the size of the source byte stream
*Return      : the buffer size occupied by the destination bytes stream converting from the
*              corresponding source byte stream
*Description : get the buffer size occupied by the destination bytes stream converting from the
*              corresponding source byte stream.
*Other       :
***************************************************************************************************/
__u32  eLIBs_charset_convert_size(__s32 src_charset, __s32 dst_charset,
                                  const __u8 *src, __u32 src_size)
{
    __s32  src_char_len;
    __s32  dst_char_len;
    __s32  src_total_len;
    __s32  dst_total_len;
    __u16  uni;
    __u8   dst[10];

    if (src == NULL || src_size == 0)
    {
        return 0;
    }

    src_char_len  = 0;
    dst_char_len  = 0;
    src_total_len = 0;
    dst_total_len = 0;

    while (1)
    {
        src_char_len = esCHS_Char2Uni(src_charset, src + src_total_len, src_size - src_total_len, &uni);

        if (src_char_len <= 0)
        {
            return dst_total_len;
        }

        src_total_len += src_char_len;
        dst_char_len = esCHS_Uni2Char(dst_charset, uni, dst, sizeof(dst));

        if (dst_char_len <= 0)
        {
            return dst_total_len;
        }

        dst_total_len += dst_char_len;
    }
}



/***************************************************************************************************
*Name        : eLIBs_charset_convert
*Prototype   : __u32  eLIBs_charset_convert( __s32 src_charset, __s32 dst_charset,
                                             const __u8 *src, __u32 src_size,
                                             __u8 *dst, __u32 dst_size );
*Arguments   : src_charset    input. the charset of the source byte steam
*              dst_charset    input. the charset of the destination byte steam
*              src            input. the source byte stream
*              src_size       input. the size of the source byte stream
*              dst            output. a buffer to store destination charset
*              dst_size       input. the size of the destnination byte stream
*Return      : the buffer size occupied by the destination bytes stream converting from the
*              corresponding source byte stream
*Description : convert the source byte stream with one charset to a destination byte stream with
*              another charset
*Other       :
***************************************************************************************************/
__u32  eLIBs_charset_convert(__s32 src_charset, __s32 dst_charset,
                             const __u8 *src, __u32 src_size,
                             __u8 *dst, __u32 dst_size)
{
    __s32  src_char_len;
    __s32  dst_char_len;
    __s32  src_total_len;
    __s32  dst_total_len;
    __u16  uni;

    if (src == NULL || src_size == 0 || dst == NULL || dst_size == 0)
    {
        return 0;
    }

    src_char_len  = 0;
    dst_char_len  = 0;
    src_total_len = 0;
    dst_total_len = 0;

    while (1)
    {
        src_char_len = esCHS_Char2Uni(src_charset, src + src_total_len, src_size - src_total_len, &uni);

        if (src_char_len <= 0)
        {
            return dst_total_len;
        }

        src_total_len += src_char_len;
        dst_char_len = esCHS_Uni2Char(dst_charset, uni, dst + dst_total_len, dst_size - dst_total_len);

        if (dst_char_len <= 0)
        {
            return dst_total_len;
        }

        dst_total_len += dst_char_len;
    }
}




#endif     //  ifndef __elibs_charset_c

/* end of elibs_charset.c */
