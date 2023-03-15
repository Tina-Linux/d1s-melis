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
/*  t42parse.h                                                             */
/*                                                                         */
/*    Type 42 font parser (specification).                                 */
/*                                                                         */
/*  Copyright 2002, 2003 by Roberto Alameda.                               */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __T42PARSE_H__
#define __T42PARSE_H__


#include "t42objs.h"
#include FT_INTERNAL_POSTSCRIPT_AUX_H


FT_BEGIN_HEADER

typedef struct  T42_ParserRec_
{
    PS_ParserRec  root;
    FT_Stream     stream;

    FT_Byte      *base_dict;
    FT_Long       base_len;

    FT_Bool       in_memory;

} T42_ParserRec, *T42_Parser;


typedef struct  T42_Loader_
{
    T42_ParserRec  parser;          /* parser used to read the stream */

    FT_UInt        num_chars;       /* number of characters in encoding */
    PS_TableRec    encoding_table;  /* PS_Table used to store the       */
    /* encoding character names         */

    FT_UInt        num_glyphs;
    PS_TableRec    glyph_names;
    PS_TableRec    charstrings;
    PS_TableRec    swap_table;      /* For moving .notdef glyph to index 0. */

} T42_LoaderRec, *T42_Loader;


FT_LOCAL(FT_Error)
t42_parser_init(T42_Parser     parser,
                FT_Stream      stream,
                FT_Memory      memory,
                PSAux_Service  psaux);

FT_LOCAL(void)
t42_parser_done(T42_Parser  parser);


FT_LOCAL(FT_Error)
t42_parse_dict(T42_Face    face,
               T42_Loader  loader,
               FT_Byte    *base,
               FT_Long     size);


FT_LOCAL(void)
t42_loader_init(T42_Loader  loader,
                T42_Face    face);

FT_LOCAL(void)
t42_loader_done(T42_Loader  loader);


/* */

FT_END_HEADER


#endif /* __T42PARSE_H__ */


/* END */
