/*
************************************************************************************************************************
*                                                    Char Enc
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : elibs_charset.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.03
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
* Gary.Wang      2009.09.03       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __elibs_charset_h
#define  __elibs_charset_h

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
*Name        : eLIBs_charset_name2enm
*Prototype   : __s32  eLIBs_charset_name2enm( const char *charset_name )
*Arguments   : charset_name   input. the name of a charset
*Return      : the enum value of the charset. return EPDK_CHARSET_ENM_UNKNOWN, if fail.
*Description : get the enum value of the charset through its name
*Other       :
***************************************************************************************************/
extern __s32  eLIBs_charset_name2enm(const char *charset_name);

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
extern __s32  eLIBs_charset_enm2name(__s32 charset_enm, char *charset_name);

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
extern __u32  eLIBs_charset_convert_size(__s32 src_charset, __s32 dst_charset,
        const __u8 *src, __u32 src_size);

/***************************************************************************************************
*Name        : eLIBs_charset_convert
*Prototype   : __u32  eLIBs_charset_convert( __s32 src_charset, __s32 dst_charset,
                                             const __u8 *src, __u32 src_size,
                                             __u8 *dst, __u32 dst_size );
*Arguments   : src_charset    input. the charset of the source byte steam
*              dst_charset    input. the charset of the destination byte steam
*              src            input. the source byte stream
*              src_size       input. the size of the source byte stream
*              dst            output. a buffer to store destination byte stream
*              dst_size       input. the size of the destnination byte stream
*Return      : the buffer size occupied by the destination bytes stream converting from the
*              corresponding source byte stream
*Description : convert the source byte stream with one charset to a destination byte stream with
*              another charset
*Other       :
***************************************************************************************************/
extern __u32  eLIBs_charset_convert(__s32 src_charset, __s32 dst_charset,
                                    const __u8 *src, __u32 src_size,
                                    __u8 *dst, __u32 dst_size);


#ifdef __cplusplus
}
#endif

#endif     //  ifndef __elibs_charset_h

/* end of elibs_charset.h */
