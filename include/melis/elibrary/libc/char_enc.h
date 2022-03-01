/*
************************************************************************************************************************
*                                                    Char Enc
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : char_enc.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.04.27
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.04.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __char_enc_h
#define  __char_enc_h
#include <typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef  enum
{
    EPDK_CHAR_ENCODING_GB2312 = 0,
    EPDK_CHAR_ENCODING_UTF8,
    EPDK_CHAR_ENCODING_UTF16BE,
    EPDK_CHAR_ENCODING_UTF16LE,
    EPDK_CHAR_ENCODING_BIG5,
    EPDK_CHAR_ENCODING_GBK,

    EPDK_CHAR_ENCODING_UNKNOWN = -1     // always the last one
}
__epdk_char_encoding_e;



/***************************************************************************************************
*Name        : eLIBs_GB2312_to_Unicode
*Prototype   : __u16  eLIBs_GB2312_to_Unicode( __u16 gb2312 )
*Arguments   : gb2312         input. gb2312 code
*Return      : 0xFFFF         converting fail
*              other value    unicode corresponding to the gb2312 code
*Description : converting gb2312 code to unicode
*Other       :
***************************************************************************************************/
extern __u16  eLIBs_GB2312_to_Unicode(__u16 gb2312);



/***************************************************************************************************
*Name        : eLIBs_GBK_to_Unicode
*Prototype   : __u16  eLIBs_GBK_to_Unicode( __u16 gbk )
*Arguments   : gbk            input. gbk code
*Return      : 0xFFFF         converting fail
*              other value    unicode corresponding to the gbk code
*Description : converting gbk code to unicode
*Other       :
***************************************************************************************************/
extern __u16  eLIBs_GBK_to_Unicode(__u16 gbk);



/***************************************************************************************************
*Name        : eLIBs_BIG5_to_Unicode
*Prototype   : __u16  eLIBs_BIG5_to_Unicode( __u16 big5 )
*Arguments   : big5            input. big5 code
*Return      : 0xFFFF         converting fail
*              other value    unicode corresponding to the big5 code
*Description : converting big5 code to unicode
*Other       :
***************************************************************************************************/
extern __u16  eLIBs_BIG5_to_Unicode(__u16 big5);



/***************************************************************************************************
*Name        : eLIBs_unicode_to_UTF8_size
*Prototype   : __u32  eLIBs_unicode_to_UTF8_size( __u16 unicode )
*Arguments   : unicode        input. unicode
*Return      : the buffer size occupied by the UTF8 encoding converting from the corresponding unicode
*Description : get the buffer size occupied by the UTF8 encoding converting from the corresponding
*              unicode
*Other       :
***************************************************************************************************/
extern __u32  eLIBs_unicode_to_UTF8_size(__u16 unicode);



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
extern __u32  eLIBs_unicode_to_UTF8(__u16 unicode, char *str, __u32 size);



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
extern __u32  eLIBs_encoding_stream_convert_size(__s32 src_encoding, __s32 dst_encoding,
        const char *src, __u32 src_size);



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
extern __u32  eLIBs_encoding_stream_convert(__s32 src_encoding, __s32 dst_encoding,
        const char *src, __u32 src_size,
        char *dst, __u32 dst_size);


#ifdef __cplusplus
}
#endif

#endif     //  ifndef __char_enc_h

/* end of char_enc.h */
