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
/*  t1load.h                                                               */
/*                                                                         */
/*    Type 1 font loader (specification).                                  */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2004, 2006, 2007 by                         */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __T1LOAD_H__
#define __T1LOAD_H__


#include <ft2build.h>
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_POSTSCRIPT_AUX_H
#include FT_MULTIPLE_MASTERS_H

#include "t1parse.h"


FT_BEGIN_HEADER


typedef struct  T1_Loader_
{
    T1_ParserRec  parser;          /* parser used to read the stream */

    FT_Int        num_chars;       /* number of characters in encoding */
    PS_TableRec   encoding_table;  /* PS_Table used to store the       */
    /* encoding character names         */

    FT_Int        num_glyphs;
    PS_TableRec   glyph_names;
    PS_TableRec   charstrings;
    PS_TableRec   swap_table;      /* For moving .notdef glyph to index 0. */

    FT_Int        num_subrs;
    PS_TableRec   subrs;
    FT_Bool       fontdata;

    FT_UInt       keywords_encountered; /* T1_LOADER_ENCOUNTERED_XXX */

} T1_LoaderRec, *T1_Loader;


/* treatment of some keywords differs depending on whether */
/* they precede or follow certain other keywords           */

#define T1_PRIVATE                ( 1 << 0 )
#define T1_FONTDIR_AFTER_PRIVATE  ( 1 << 1 )


FT_LOCAL(FT_Error)
T1_Open_Face(T1_Face  face);

#ifndef T1_CONFIG_OPTION_NO_MM_SUPPORT

FT_LOCAL(FT_Error)
T1_Get_Multi_Master(T1_Face           face,
                    FT_Multi_Master  *master);

FT_LOCAL_DEF(FT_Error)
T1_Get_MM_Var(T1_Face      face,
              FT_MM_Var **master);

FT_LOCAL(FT_Error)
T1_Set_MM_Blend(T1_Face    face,
                FT_UInt    num_coords,
                FT_Fixed  *coords);

FT_LOCAL(FT_Error)
T1_Set_MM_Design(T1_Face   face,
                 FT_UInt   num_coords,
                 FT_Long  *coords);

FT_LOCAL_DEF(FT_Error)
T1_Set_Var_Design(T1_Face    face,
                  FT_UInt    num_coords,
                  FT_Fixed  *coords);

FT_LOCAL(void)
T1_Done_Blend(T1_Face  face);

#endif /* !T1_CONFIG_OPTION_NO_MM_SUPPORT */


FT_END_HEADER

#endif /* __T1LOAD_H__ */


/* END */
