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
/*  t1types.h                                                              */
/*                                                                         */
/*    Basic Type1/Type2 type definitions and interface (specification      */
/*    only).                                                               */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004, 2006 by                         */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __T1TYPES_H__
#define __T1TYPES_H__


#include "ft2build.h"
#include FT_TYPE1_TABLES_H
#include FT_INTERNAL_POSTSCRIPT_HINTS_H
#include FT_INTERNAL_SERVICE_H
#include FT_SERVICE_POSTSCRIPT_CMAPS_H


FT_BEGIN_HEADER


/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/***                                                                   ***/
/***                                                                   ***/
/***              REQUIRED TYPE1/TYPE2 TABLES DEFINITIONS              ***/
/***                                                                   ***/
/***                                                                   ***/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/


/*************************************************************************/
/*                                                                       */
/* <Struct>                                                              */
/*    T1_EncodingRec                                                     */
/*                                                                       */
/* <Description>                                                         */
/*    A structure modeling a custom encoding.                            */
/*                                                                       */
/* <Fields>                                                              */
/*    num_chars  :: The number of character codes in the encoding.       */
/*                  Usually 256.                                         */
/*                                                                       */
/*    code_first :: The lowest valid character code in the encoding.     */
/*                                                                       */
/*    code_last  :: The highest valid character code in the encoding.    */
/*                                                                       */
/*    char_index :: An array of corresponding glyph indices.             */
/*                                                                       */
/*    char_name  :: An array of corresponding glyph names.               */
/*                                                                       */
typedef struct  T1_EncodingRecRec_
{
    FT_Int       num_chars;
    FT_Int       code_first;
    FT_Int       code_last;

    FT_UShort   *char_index;
    FT_String  **char_name;

} T1_EncodingRec, *T1_Encoding;


typedef enum  T1_EncodingType_
{
    T1_ENCODING_TYPE_NONE = 0,
    T1_ENCODING_TYPE_ARRAY,
    T1_ENCODING_TYPE_STANDARD,
    T1_ENCODING_TYPE_ISOLATIN1,
    T1_ENCODING_TYPE_EXPERT

} T1_EncodingType;


typedef struct  T1_FontRec_
{
    PS_FontInfoRec   font_info;         /* font info dictionary */
    PS_PrivateRec    private_dict;      /* private dictionary   */
    FT_String       *font_name;         /* top-level dictionary */

    T1_EncodingType  encoding_type;
    T1_EncodingRec   encoding;

    FT_Byte         *subrs_block;
    FT_Byte         *charstrings_block;
    FT_Byte         *glyph_names_block;

    FT_Int           num_subrs;
    FT_Byte        **subrs;
    FT_PtrDist      *subrs_len;

    FT_Int           num_glyphs;
    FT_String      **glyph_names;       /* array of glyph names       */
    FT_Byte        **charstrings;       /* array of glyph charstrings */
    FT_PtrDist      *charstrings_len;

    FT_Byte          paint_type;
    FT_Byte          font_type;
    FT_Matrix        font_matrix;
    FT_Vector        font_offset;
    FT_BBox          font_bbox;
    FT_Long          font_id;

    FT_Fixed         stroke_width;

} T1_FontRec, *T1_Font;


typedef struct  CID_SubrsRec_
{
    FT_UInt    num_subrs;
    FT_Byte  **code;

} CID_SubrsRec, *CID_Subrs;


/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/***                                                                   ***/
/***                                                                   ***/
/***                AFM FONT INFORMATION STRUCTURES                    ***/
/***                                                                   ***/
/***                                                                   ***/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/

typedef struct  AFM_TrackKernRec_
{
    FT_Int    degree;
    FT_Fixed  min_ptsize;
    FT_Fixed  min_kern;
    FT_Fixed  max_ptsize;
    FT_Fixed  max_kern;

} AFM_TrackKernRec, *AFM_TrackKern;

typedef struct  AFM_KernPairRec_
{
    FT_Int  index1;
    FT_Int  index2;
    FT_Int  x;
    FT_Int  y;

} AFM_KernPairRec, *AFM_KernPair;

typedef struct  AFM_FontInfoRec_
{
    FT_Bool        IsCIDFont;
    FT_BBox        FontBBox;
    FT_Fixed       Ascender;
    FT_Fixed       Descender;
    AFM_TrackKern  TrackKerns;   /* free if non-NULL */
    FT_Int         NumTrackKern;
    AFM_KernPair   KernPairs;    /* free if non-NULL */
    FT_Int         NumKernPair;

} AFM_FontInfoRec, *AFM_FontInfo;


/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/***                                                                   ***/
/***                                                                   ***/
/***                ORIGINAL T1_FACE CLASS DEFINITION                  ***/
/***                                                                   ***/
/***                                                                   ***/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/


typedef struct T1_FaceRec_   *T1_Face;
typedef struct CID_FaceRec_  *CID_Face;


typedef struct  T1_FaceRec_
{
    FT_FaceRec      root;
    T1_FontRec      type1;
    const void     *psnames;
    const void     *psaux;
    const void     *afm_data;
    FT_CharMapRec   charmaprecs[2];
    FT_CharMap      charmaps[2];

#ifdef FT_CONFIG_OPTION_OLD_INTERNALS
    PS_Unicodes     unicode_map;
#endif

    /* support for Multiple Masters fonts */
    PS_Blend        blend;

    /* undocumented, optional: indices of subroutines that express      */
    /* the NormalizeDesignVector and the ConvertDesignVector procedure, */
    /* respectively, as Type 2 charstrings; -1 if keywords not present  */
    FT_Int           ndv_idx;
    FT_Int           cdv_idx;

    /* undocumented, optional: has the same meaning as len_buildchar */
    /* for Type 2 fonts; manipulated by othersubrs 19, 24, and 25    */
    FT_UInt          len_buildchar;
    FT_Int          *buildchar;

    /* since version 2.1 - interface to PostScript hinter */
    const void     *pshinter;

} T1_FaceRec;


typedef struct  CID_FaceRec_
{
    FT_FaceRec       root;
    void            *psnames;
    void            *psaux;
    CID_FaceInfoRec  cid;
    void            *afm_data;
    CID_Subrs        subrs;

    /* since version 2.1 - interface to PostScript hinter */
    void            *pshinter;

    /* since version 2.1.8, but was originally positioned after `afm_data' */
    FT_Byte         *binary_data; /* used if hex data has been converted */
    FT_Stream        cid_stream;

} CID_FaceRec;


FT_END_HEADER

#endif /* __T1TYPES_H__ */


/* END */
