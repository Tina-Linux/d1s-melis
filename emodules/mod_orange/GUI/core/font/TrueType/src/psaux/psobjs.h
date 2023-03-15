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
/*  psobjs.h                                                               */
/*                                                                         */
/*    Auxiliary functions for PostScript fonts (specification).            */
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
#ifndef __PSOBJS_H__
#define __PSOBJS_H__


#include <ft2build.h>
#include FT_INTERNAL_POSTSCRIPT_AUX_H


FT_BEGIN_HEADER


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                             T1_TABLE                          *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/


FT_CALLBACK_TABLE
const PS_Table_FuncsRec    ps_table_funcs;

FT_CALLBACK_TABLE
const PS_Parser_FuncsRec   ps_parser_funcs;

FT_CALLBACK_TABLE
const T1_Builder_FuncsRec  t1_builder_funcs;


FT_LOCAL(FT_Error)
ps_table_new(PS_Table   table,
             FT_Int     count,
             FT_Memory  memory);

FT_LOCAL(FT_Error)
ps_table_add(PS_Table    table,
             FT_Int      idx,
             void       *object,
             FT_PtrDist  length);

FT_LOCAL(void)
ps_table_done(PS_Table  table);


FT_LOCAL(void)
ps_table_release(PS_Table  table);


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                            T1 PARSER                          *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/


FT_LOCAL(void)
ps_parser_skip_spaces(PS_Parser  parser);

FT_LOCAL(void)
ps_parser_skip_PS_token(PS_Parser  parser);

FT_LOCAL(void)
ps_parser_to_token(PS_Parser  parser,
                   T1_Token   token);

FT_LOCAL(void)
ps_parser_to_token_array(PS_Parser  parser,
                         T1_Token   tokens,
                         FT_UInt    max_tokens,
                         FT_Int    *pnum_tokens);

FT_LOCAL(FT_Error)
ps_parser_load_field(PS_Parser       parser,
                     const T1_Field  field,
                     void          **objects,
                     FT_UInt         max_objects,
                     FT_ULong       *pflags);

FT_LOCAL(FT_Error)
ps_parser_load_field_table(PS_Parser       parser,
                           const T1_Field  field,
                           void          **objects,
                           FT_UInt         max_objects,
                           FT_ULong       *pflags);

FT_LOCAL(FT_Long)
ps_parser_to_int(PS_Parser  parser);


FT_LOCAL(FT_Error)
ps_parser_to_bytes(PS_Parser  parser,
                   FT_Byte   *bytes,
                   FT_Long    max_bytes,
                   FT_Long   *pnum_bytes,
                   FT_Bool    delimiters);


FT_LOCAL(FT_Fixed)
ps_parser_to_fixed(PS_Parser  parser,
                   FT_Int     power_ten);


FT_LOCAL(FT_Int)
ps_parser_to_coord_array(PS_Parser  parser,
                         FT_Int     max_coords,
                         FT_Short  *coords);

FT_LOCAL(FT_Int)
ps_parser_to_fixed_array(PS_Parser  parser,
                         FT_Int     max_values,
                         FT_Fixed  *values,
                         FT_Int     power_ten);


FT_LOCAL(void)
ps_parser_init(PS_Parser  parser,
               FT_Byte   *base,
               FT_Byte   *limit,
               FT_Memory  memory);

FT_LOCAL(void)
ps_parser_done(PS_Parser  parser);


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                            T1 BUILDER                         *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

FT_LOCAL(void)
t1_builder_init(T1_Builder    builder,
                FT_Face       face,
                FT_Size       size,
                FT_GlyphSlot  glyph,
                FT_Bool       hinting);

FT_LOCAL(void)
t1_builder_done(T1_Builder  builder);

FT_LOCAL(FT_Error)
t1_builder_check_points(T1_Builder  builder,
                        FT_Int      count);

FT_LOCAL(void)
t1_builder_add_point(T1_Builder  builder,
                     FT_Pos      x,
                     FT_Pos      y,
                     FT_Byte     flag);

FT_LOCAL(FT_Error)
t1_builder_add_point1(T1_Builder  builder,
                      FT_Pos      x,
                      FT_Pos      y);

FT_LOCAL(FT_Error)
t1_builder_add_contour(T1_Builder  builder);


FT_LOCAL(FT_Error)
t1_builder_start_point(T1_Builder  builder,
                       FT_Pos      x,
                       FT_Pos      y);


FT_LOCAL(void)
t1_builder_close_contour(T1_Builder  builder);


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                            OTHER                              *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

FT_LOCAL(void)
t1_decrypt(FT_Byte   *buffer,
           FT_Offset  length,
           FT_UShort  seed);


FT_END_HEADER

#endif /* __PSOBJS_H__ */


/* END */
