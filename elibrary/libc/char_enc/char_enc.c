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
#ifndef  __char_enc_c
#define  __char_enc_c
#include <typedef.h>
#include <kapi.h>
#include <mod_charset.h>
#include <elibs_charset.h>

#undef  min
#define min( x, y )                     ( (x) < (y) ? (x) : (y) )

#undef  max
#define max( x, y )                     ( (x) > (y) ? (x) : (y) )

/***************************************************************************************************
*Name        : eLIBs_GBK_to_Unicode
*Prototype   : __u16  eLIBs_GBK_to_Unicode( __u16 gbk )
*Arguments   : gbk            input. gbk code
*Return      : 0xFFFF         converting fail
*              other value    unicode corresponding to the gbk code
*Description : converting gbk code to unicode
*Other       :
***************************************************************************************************/
__u16  eLIBs_GBK_to_Unicode(__u16 gbk)
{
    if (gbk < 0x80)
    {
        return gbk;
    }
    else
    {
        __s32 len;
        __u16 uni;
        __u8  stream[4];
        stream[0] = (gbk & 0xff00) >> 8;
        stream[1] = gbk & 0xff;
        len = esCHS_Char2Uni(EPDK_CHARSET_ENM_GBK, stream, 4, &uni);

        if (len <= 0)
        {
            return (__u16)0xffff;
        }
        else
        {
            return uni;
        }
    }
}



/***************************************************************************************************
*Name        : eLIBs_GB2312_to_Unicode
*Prototype   : __u16  eLIBs_GB2312_to_Unicode( __u16 gb2312 )
*Arguments   : gb2312         input. gb2312 code
*Return      : 0xFFFF         converting fail
*              other value    unicode corresponding to the gb2312 code
*Description : converting gb2312 code to unicode
*Other       :
***************************************************************************************************/
__u16  eLIBs_GB2312_to_Unicode(__u16 gb2312)
{
    return eLIBs_GBK_to_Unicode(gb2312);
}


/***************************************************************************************************
*Name        : eLIBs_BIG5_to_Unicode
*Prototype   : __u16  eLIBs_BIG5_to_Unicode( __u16 big5 )
*Arguments   : big5            input. big5 code
*Return      : 0xFFFF         converting fail
*              other value    unicode corresponding to the big5 code
*Description : converting big5 code to unicode
*Other       :
***************************************************************************************************/
__u16  eLIBs_BIG5_to_Unicode(__u16 big5)
{
    if (big5 < 0x80)
    {
        return big5;
    }
    else
    {
        __s32 len;
        __u16 uni;
        __u8  stream[4];
        stream[0] = (big5 & 0xff00) >> 8;
        stream[1] = big5 & 0xff;
        len = esCHS_Char2Uni(EPDK_CHARSET_ENM_BIG5, stream, 4, &uni);

        if (len <= 0)
        {
            return (__u16)0xffff;
        }
        else
        {
            return uni;
        }
    }
}


/***************************************************************************************************
*Name        : eLIBs_unicode_to_UTF8_size
*Prototype   : __u32  eLIBs_unicode_to_UTF8_size( __u16 unicode )
*Arguments   : unicode        input. unicode
*Return      : the buffer size occupied by the UTF8 encoding converting from the corresponding unicode
*Description : get the buffer size occupied by the UTF8 encoding converting from the corresponding
*              unicode
*Other       :
***************************************************************************************************/
__u32  eLIBs_unicode_to_UTF8_size(__u16 unicode)
{
    __u32  len;

    if (unicode < 0x0080)
    {
        len = 1;
    }
    else if (unicode < 0x0800)
    {
        len = 2;
    }
    else
    {
        len = 3;
    }

    return len;
}



/***************************************************************************************************
*Name        : eLIBs_unicode_to_UTF8
*Prototype   : __u32  eLIBs_unicode_to_UTF8( __u16 unicode, char *str, __u32 size )
*Arguments   : unicode        input. unicode
*              str            output. a buffer to store the UTF8 encoding
*              size           input. the buffer size.
*Return      : the buffer size occupied by the UTF8 encoding converting from the corresponding unicode
*Description : converting a unicode to UTF8 encoding
*Other       :
***************************************************************************************************/
__u32  eLIBs_unicode_to_UTF8(__u16 unicode, char *str, __u32 size)
{
    __u32  len;

    if (str == NULL || size == 0)
    {
        return 0;
    }

    len = esCHS_Uni2Char(EPDK_CHARSET_ENM_UTF8, unicode, (__u8 *)str, size);

    if (len <= 0)
    {
        return 0;
    }
    else
    {
        return len;
    }
}




/***************************************************************************************************
*Name        : eLIBs_encoding_stream_convert_size
*Prototype   : __u32  eLIBs_encoding_stream_convert_size( __s32 src_encoding, __s32 dst_encoding,
                                                          const char *src, __u32 src_size );
*Arguments   : src_encoding   input. the encoding of the source byte steam
*              dst_encoding   input. the encoding of the destination byte steam
*              src            input. the source byte stream
*              src_size       input. the size of the source byte stream
*Return      : the buffer size occupied by the destination bytes stream converting from the
*              corresponding source byte stream
*Description : get the buffer size occupied by the destination bytes stream converting from the
*              corresponding source byte stream.
*Other       :
***************************************************************************************************/
__u32  eLIBs_encoding_stream_convert_size(__s32 src_encoding, __s32 dst_encoding,
        const char *src, __u32 src_size)
{
    return eLIBs_charset_convert_size(src_encoding, dst_encoding, (const __u8 *)src, src_size);
}



/***************************************************************************************************
*Name        : eLIBs_encoding_stream_convert
*Prototype   : __u32  eLIBs_encoding_stream_convert( __s32 src_encoding, __s32 dst_encoding,
                                                     const char *src, __u32 src_size,
                                                     char *dst, __u32 dst_size );
*Arguments   : src_encoding   input. the encoding of the source byte steam
*              dst_encoding   input. the encoding of the destination byte steam
*              src            input. the source byte stream
*              src_size       input. the size of the source byte stream
*              dst            output. a buffer to store destination encoding
*              dst_size       input. the size of the destnination byte stream
*Return      : the buffer size occupied by the destination bytes stream converting from the
*              corresponding source byte stream
*Description : convert the source byte stream with one encoding to a destination byte stream with
*              another encoding
*Other       :
***************************************************************************************************/
__u32  eLIBs_encoding_stream_convert(__s32 src_encoding, __s32 dst_encoding,
                                     const char *src, __u32 src_size,
                                     char *dst, __u32 dst_size)
{
    return eLIBs_charset_convert(src_encoding, dst_encoding,
                                 (const __u8 *)src, src_size,
                                 (__u8 *)dst, dst_size);
}



#endif     //  ifndef __char_enc_c

/* end of char_enc.c */
