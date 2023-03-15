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
/*  t1parse.h                                                              */
/*                                                                         */
/*    Type 1 parser (specification).                                       */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __T1PARSE_H__
#define __T1PARSE_H__


#include <ft2build.h>
#include FT_INTERNAL_TYPE1_TYPES_H
#include FT_INTERNAL_STREAM_H


FT_BEGIN_HEADER


/*************************************************************************/
/*                                                                       */
/* <Struct>                                                              */
/*    T1_ParserRec                                                       */
/*                                                                       */
/* <Description>                                                         */
/*    A PS_ParserRec is an object used to parse a Type 1 fonts very      */
/*    quickly.                                                           */
/*                                                                       */
/* <Fields>                                                              */
/*    root         :: The root parser.                                   */
/*                                                                       */
/*    stream       :: The current input stream.                          */
/*                                                                       */
/*    base_dict    :: A pointer to the top-level dictionary.             */
/*                                                                       */
/*    base_len     :: The length in bytes of the top dictionary.         */
/*                                                                       */
/*    private_dict :: A pointer to the private dictionary.               */
/*                                                                       */
/*    private_len  :: The length in bytes of the private dictionary.     */
/*                                                                       */
/*    in_pfb       :: A boolean.  Indicates that we are handling a PFB   */
/*                    file.                                              */
/*                                                                       */
/*    in_memory    :: A boolean.  Indicates a memory-based stream.       */
/*                                                                       */
/*    single_block :: A boolean.  Indicates that the private dictionary  */
/*                    is stored in lieu of the base dictionary.          */
/*                                                                       */
typedef struct  T1_ParserRec_
{
    PS_ParserRec  root;
    FT_Stream     stream;

    FT_Byte      *base_dict;
    FT_Long       base_len;

    FT_Byte      *private_dict;
    FT_Long       private_len;

    FT_Bool       in_pfb;
    FT_Bool       in_memory;
    FT_Bool       single_block;

} T1_ParserRec, *T1_Parser;


#define T1_Add_Table( p, i, o, l )  (p)->funcs.add( (p), i, o, l )
#define T1_Done_Table( p )          \
    do                        \
    {                         \
        if ( (p)->funcs.done )  \
            (p)->funcs.done( p ); \
    } while ( 0 )
#define T1_Release_Table( p )          \
    do                           \
    {                            \
        if ( (p)->funcs.release )  \
            (p)->funcs.release( p ); \
    } while ( 0 )


#define T1_Skip_Spaces( p )    (p)->root.funcs.skip_spaces( &(p)->root )
#define T1_Skip_PS_Token( p )  (p)->root.funcs.skip_PS_token( &(p)->root )

#define T1_ToInt( p )       (p)->root.funcs.to_int( &(p)->root )
#define T1_ToFixed( p, t )  (p)->root.funcs.to_fixed( &(p)->root, t )

#define T1_ToCoordArray( p, m, c )                           \
    (p)->root.funcs.to_coord_array( &(p)->root, m, c )
#define T1_ToFixedArray( p, m, f, t )                           \
    (p)->root.funcs.to_fixed_array( &(p)->root, m, f, t )
#define T1_ToToken( p, t )                          \
    (p)->root.funcs.to_token( &(p)->root, t )
#define T1_ToTokenArray( p, t, m, c )                           \
    (p)->root.funcs.to_token_array( &(p)->root, t, m, c )

#define T1_Load_Field( p, f, o, m, pf )                         \
    (p)->root.funcs.load_field( &(p)->root, f, o, m, pf )

#define T1_Load_Field_Table( p, f, o, m, pf )                         \
    (p)->root.funcs.load_field_table( &(p)->root, f, o, m, pf )


FT_LOCAL(FT_Error)
T1_New_Parser(T1_Parser      parser,
              FT_Stream      stream,
              FT_Memory      memory,
              PSAux_Service  psaux);

FT_LOCAL(FT_Error)
T1_Get_Private_Dict(T1_Parser      parser,
                    PSAux_Service  psaux);

FT_LOCAL(void)
T1_Finalize_Parser(T1_Parser  parser);


FT_END_HEADER

#endif /* __T1PARSE_H__ */


/* END */
