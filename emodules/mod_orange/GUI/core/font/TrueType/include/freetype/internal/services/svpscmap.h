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
/*  svpscmap.h                                                             */
/*                                                                         */
/*    The FreeType PostScript charmap service (specification).             */
/*                                                                         */
/*  Copyright 2003, 2006 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __SVPSCMAP_H__
#define __SVPSCMAP_H__

#include FT_INTERNAL_OBJECTS_H


FT_BEGIN_HEADER


#define FT_SERVICE_ID_POSTSCRIPT_CMAPS  "postscript-cmaps"


/*
 *  Adobe glyph name to unicode value.
 */
typedef FT_UInt32
(*PS_Unicode_ValueFunc)(const char  *glyph_name);

/*
 *  Macintosh name id to glyph name.  NULL if invalid index.
 */
typedef const char *
(*PS_Macintosh_NameFunc)(FT_UInt  name_index);

/*
 *  Adobe standard string ID to glyph name.  NULL if invalid index.
 */
typedef const char *
(*PS_Adobe_Std_StringsFunc)(FT_UInt  string_index);


/*
 *  Simple unicode -> glyph index charmap built from font glyph names
 *  table.
 */
typedef struct  PS_UniMap_
{
    FT_UInt32  unicode;      /* bit 31 set: is glyph variant */
    FT_UInt    glyph_index;

} PS_UniMap;


typedef struct PS_UnicodesRec_  *PS_Unicodes;

typedef struct  PS_UnicodesRec_
{
    FT_CMapRec  cmap;
    FT_UInt     num_maps;
    PS_UniMap  *maps;

} PS_UnicodesRec;


/*
 *  A function which returns a glyph name for a given index.  Returns
 *  NULL if invalid index.
 */
typedef const char *
(*PS_GetGlyphNameFunc)(FT_Pointer  data,
                       FT_UInt     string_index);

/*
 *  A function used to release the glyph name returned by
 *  PS_GetGlyphNameFunc, when needed
 */
typedef void
(*PS_FreeGlyphNameFunc)(FT_Pointer  data,
                        const char  *name);

typedef FT_Error
(*PS_Unicodes_InitFunc)(FT_Memory             memory,
                        PS_Unicodes           unicodes,
                        FT_UInt               num_glyphs,
                        PS_GetGlyphNameFunc   get_glyph_name,
                        PS_FreeGlyphNameFunc  free_glyph_name,
                        FT_Pointer            glyph_data);

typedef FT_UInt
(*PS_Unicodes_CharIndexFunc)(PS_Unicodes  unicodes,
                             FT_UInt32    unicode);

typedef FT_ULong
(*PS_Unicodes_CharNextFunc)(PS_Unicodes  unicodes,
                            FT_UInt32   *unicode);


FT_DEFINE_SERVICE(PsCMaps)
{
    PS_Unicode_ValueFunc       unicode_value;
    PS_Unicodes_InitFunc       unicodes_init;
    PS_Unicodes_CharIndexFunc  unicodes_char_index;
    PS_Unicodes_CharNextFunc   unicodes_char_next;
    PS_Macintosh_NameFunc      macintosh_name;
    PS_Adobe_Std_StringsFunc   adobe_std_strings;
    const unsigned short      *adobe_std_encoding;
    const unsigned short      *adobe_expert_encoding;
};

/* */


FT_END_HEADER


#endif /* __SVPSCMAP_H__ */


/* END */
