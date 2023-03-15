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
/*  cffcmap.c                                                              */
/*                                                                         */
/*    CFF character mapping table (cmap) support (body).                   */
/*                                                                         */
/*  Copyright 2002, 2003, 2004, 2005, 2006 by                              */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#include "cffcmap.h"
#include "cffload.h"

#include "cfferrs.h"


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****           CFF STANDARD (AND EXPERT) ENCODING CMAPS            *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

FT_CALLBACK_DEF(FT_Error)
cff_cmap_encoding_init(CFF_CMapStd  cmap)
{
    TT_Face       face     = (TT_Face)FT_CMAP_FACE(cmap);
    CFF_Font      cff      = (CFF_Font)face->extra.data;
    CFF_Encoding  encoding = &cff->encoding;
    cmap->gids  = encoding->codes;
    return 0;
}


FT_CALLBACK_DEF(void)
cff_cmap_encoding_done(CFF_CMapStd  cmap)
{
    cmap->gids  = NULL;
}


FT_CALLBACK_DEF(FT_UInt)
cff_cmap_encoding_char_index(CFF_CMapStd  cmap,
                             FT_UInt32    char_code)
{
    FT_UInt  result = 0;

    if (char_code < 256)
    {
        result = cmap->gids[char_code];
    }

    return result;
}


FT_CALLBACK_DEF(FT_UInt)
cff_cmap_encoding_char_next(CFF_CMapStd   cmap,
                            FT_UInt32    *pchar_code)
{
    FT_UInt    result    = 0;
    FT_UInt32  char_code = *pchar_code;
    *pchar_code = 0;

    if (char_code < 255)
    {
        FT_UInt  code = (FT_UInt)(char_code + 1);

        for (;;)
        {
            if (code >= 256)
            {
                break;
            }

            result = cmap->gids[code];

            if (result != 0)
            {
                *pchar_code = code;
                break;
            }

            code++;
        }
    }

    return result;
}


FT_CALLBACK_TABLE_DEF const FT_CMap_ClassRec
cff_cmap_encoding_class_rec =
{
    sizeof(CFF_CMapStdRec),

    (FT_CMap_InitFunc)     cff_cmap_encoding_init,
    (FT_CMap_DoneFunc)     cff_cmap_encoding_done,
    (FT_CMap_CharIndexFunc)cff_cmap_encoding_char_index,
    (FT_CMap_CharNextFunc) cff_cmap_encoding_char_next
};


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****              CFF SYNTHETIC UNICODE ENCODING CMAP              *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

FT_CALLBACK_DEF(const char *)
cff_sid_to_glyph_name(TT_Face   face,
                      FT_UInt   idx)
{
    CFF_Font            cff     = (CFF_Font)face->extra.data;
    CFF_Charset         charset = &cff->charset;
    FT_Service_PsCMaps  psnames = (FT_Service_PsCMaps)cff->psnames;
    FT_UInt             sid     = charset->sids[idx];
    return cff_index_get_sid_string(&cff->string_index, sid, psnames);
}


FT_CALLBACK_DEF(void)
cff_sid_free_glyph_name(TT_Face      face,
                        const char  *gname)
{
    FT_Memory  memory = FT_FACE_MEMORY(face);
    FT_FREE(gname);
}


FT_CALLBACK_DEF(FT_Error)
cff_cmap_unicode_init(PS_Unicodes  unicodes)
{
    TT_Face             face    = (TT_Face)FT_CMAP_FACE(unicodes);
    FT_Memory           memory  = FT_FACE_MEMORY(face);
    CFF_Font            cff     = (CFF_Font)face->extra.data;
    CFF_Charset         charset = &cff->charset;
    FT_Service_PsCMaps  psnames = (FT_Service_PsCMaps)cff->psnames;

    /* can't build Unicode map for CID-keyed font */
    if (!charset->sids)
    {
        return CFF_Err_Invalid_Argument;
    }

    return psnames->unicodes_init(memory,
                                  unicodes,
                                  cff->num_glyphs,
                                  (PS_GetGlyphNameFunc)&cff_sid_to_glyph_name,
                                  (PS_FreeGlyphNameFunc)&cff_sid_free_glyph_name,
                                  (FT_Pointer)face);
}


FT_CALLBACK_DEF(void)
cff_cmap_unicode_done(PS_Unicodes  unicodes)
{
    FT_Face    face   = FT_CMAP_FACE(unicodes);
    FT_Memory  memory = FT_FACE_MEMORY(face);
    FT_FREE(unicodes->maps);
    unicodes->num_maps = 0;
}


FT_CALLBACK_DEF(FT_UInt)
cff_cmap_unicode_char_index(PS_Unicodes  unicodes,
                            FT_UInt32    char_code)
{
    TT_Face             face    = (TT_Face)FT_CMAP_FACE(unicodes);
    CFF_Font            cff     = (CFF_Font)face->extra.data;
    FT_Service_PsCMaps  psnames = (FT_Service_PsCMaps)cff->psnames;
    return psnames->unicodes_char_index(unicodes, char_code);
}


FT_CALLBACK_DEF(FT_UInt)
cff_cmap_unicode_char_next(PS_Unicodes  unicodes,
                           FT_UInt32   *pchar_code)
{
    TT_Face             face    = (TT_Face)FT_CMAP_FACE(unicodes);
    CFF_Font            cff     = (CFF_Font)face->extra.data;
    FT_Service_PsCMaps  psnames = (FT_Service_PsCMaps)cff->psnames;
    return psnames->unicodes_char_next(unicodes, pchar_code);
}


FT_CALLBACK_TABLE_DEF const FT_CMap_ClassRec
cff_cmap_unicode_class_rec =
{
    sizeof(PS_UnicodesRec),

    (FT_CMap_InitFunc)     cff_cmap_unicode_init,
    (FT_CMap_DoneFunc)     cff_cmap_unicode_done,
    (FT_CMap_CharIndexFunc)cff_cmap_unicode_char_index,
    (FT_CMap_CharNextFunc) cff_cmap_unicode_char_next
};


/* END */
