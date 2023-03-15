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
/*  ttcmap.h                                                               */
/*                                                                         */
/*    TrueType character mapping table (cmap) support (specification).     */
/*                                                                         */
/*  Copyright 2002, 2003, 2004, 2005 by                                    */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __TTCMAP_H__
#define __TTCMAP_H__


#include "ft2build.h"
#include FT_INTERNAL_TRUETYPE_TYPES_H
#include FT_INTERNAL_VALIDATE_H
#include FT_SERVICE_TT_CMAP_H

FT_BEGIN_HEADER


#define TT_CMAP_FLAG_UNSORTED     1
#define TT_CMAP_FLAG_OVERLAPPING  2

typedef struct  TT_CMapRec_
{
    FT_CMapRec  cmap;
    FT_Byte    *data;           /* pointer to in-memory cmap table */
    FT_Int      flags;          /* for format 4 only               */

} TT_CMapRec, *TT_CMap;

typedef const struct TT_CMap_ClassRec_  *TT_CMap_Class;


typedef FT_Error
(*TT_CMap_ValidateFunc)(FT_Byte      *data,
                        FT_Validator  valid);

typedef struct  TT_CMap_ClassRec_
{
    FT_CMap_ClassRec      clazz;
    FT_UInt               format;
    TT_CMap_ValidateFunc  validate;
    TT_CMap_Info_GetFunc  get_cmap_info;

} TT_CMap_ClassRec;


typedef struct  TT_ValidatorRec_
{
    FT_ValidatorRec  validator;
    FT_UInt          num_glyphs;

} TT_ValidatorRec, *TT_Validator;


#define TT_VALIDATOR( x )          ((TT_Validator)( x ))
#define TT_VALID_GLYPH_COUNT( x )  TT_VALIDATOR( x )->num_glyphs


FT_LOCAL(FT_Error)
tt_face_build_cmaps(TT_Face  face);

/* used in tt-cmaps service */
FT_LOCAL(FT_Error)
tt_get_cmap_info(FT_CharMap    charmap,
                 TT_CMapInfo  *cmap_info);


FT_END_HEADER

#endif /* __TTCMAP_H__ */


/* END */
