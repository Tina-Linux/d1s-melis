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
/*                                                                         */
/*  cidparse.h                                                             */
/*                                                                         */
/*    CID-keyed Type1 parser (specification).                              */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004 by                               */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __CIDPARSE_H__
#define __CIDPARSE_H__


#include <ft2build.h>
#include FT_INTERNAL_TYPE1_TYPES_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_POSTSCRIPT_AUX_H


FT_BEGIN_HEADER


/*************************************************************************/
/*                                                                       */
/* <Struct>                                                              */
/*    CID_Parser                                                         */
/*                                                                       */
/* <Description>                                                         */
/*    A CID_Parser is an object used to parse a Type 1 fonts very        */
/*    quickly.                                                           */
/*                                                                       */
/* <Fields>                                                              */
/*    root           :: The root PS_ParserRec fields.                    */
/*                                                                       */
/*    stream         :: The current input stream.                        */
/*                                                                       */
/*    postscript     :: A pointer to the data to be parsed.              */
/*                                                                       */
/*    postscript_len :: The length of the data to be parsed.             */
/*                                                                       */
/*    data_offset    :: The start position of the binary data (i.e., the */
/*                      end of the data to be parsed.                    */
/*                                                                       */
/*    binary_length  :: The length of the data after the `StartData'     */
/*                      command if the data format is hexadecimal.       */
/*                                                                       */
/*    cid            :: A structure which holds the information about    */
/*                      the current font.                                */
/*                                                                       */
/*    num_dict       :: The number of font dictionaries.                 */
/*                                                                       */
typedef struct  CID_Parser_
{
    PS_ParserRec  root;
    FT_Stream     stream;

    FT_Byte      *postscript;
    FT_Long       postscript_len;

    FT_ULong      data_offset;

    FT_Long       binary_length;

    CID_FaceInfo  cid;
    FT_Int        num_dict;

} CID_Parser;


FT_LOCAL(FT_Error)
cid_parser_new(CID_Parser    *parser,
               FT_Stream      stream,
               FT_Memory      memory,
               PSAux_Service  psaux);

FT_LOCAL(void)
cid_parser_done(CID_Parser  *parser);


/*************************************************************************/
/*                                                                       */
/*                            PARSING ROUTINES                           */
/*                                                                       */
/*************************************************************************/

#define cid_parser_skip_spaces( p ) \
    (p)->root.funcs.skip_spaces( &(p)->root )
#define cid_parser_skip_PS_token( p ) \
    (p)->root.funcs.skip_PS_token( &(p)->root )

#define cid_parser_to_int( p )        (p)->root.funcs.to_int( &(p)->root )
#define cid_parser_to_fixed( p, t )   (p)->root.funcs.to_fixed( &(p)->root, t )

#define cid_parser_to_coord_array( p, m, c )                          \
    (p)->root.funcs.to_coord_array( &(p)->root, m, c )
#define cid_parser_to_fixed_array( p, m, f, t )                          \
    (p)->root.funcs.to_fixed_array( &(p)->root, m, f, t )
#define cid_parser_to_token( p, t )                         \
    (p)->root.funcs.to_token( &(p)->root, t )
#define cid_parser_to_token_array( p, t, m, c )                          \
    (p)->root.funcs.to_token_array( &(p)->root, t, m, c )

#define cid_parser_load_field( p, f, o )                              \
    (p)->root.funcs.load_field( &(p)->root, f, o, 0, 0 )
#define cid_parser_load_field_table( p, f, o )                              \
    (p)->root.funcs.load_field_table( &(p)->root, f, o, 0, 0 )


FT_END_HEADER

#endif /* __CIDPARSE_H__ */


/* END */
