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
/*  ftsynth.c                                                              */
/*                                                                         */
/*    FreeType synthesizing code for emboldening and slanting (body).      */
/*                                                                         */
/*  Copyright 2000-2001, 2002, 2003, 2004, 2005, 2006 by                   */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#include "ft2build.h"
#include FT_SYNTHESIS_H
#include FT_INTERNAL_OBJECTS_H
#include FT_OUTLINE_H
#include FT_BITMAP_H


/*************************************************************************/
/*************************************************************************/
/****                                                                 ****/
/****   EXPERIMENTAL OBLIQUING SUPPORT                                ****/
/****                                                                 ****/
/*************************************************************************/
/*************************************************************************/

/* documentation is in ftsynth.h */

FT_EXPORT_DEF(void)
FT_GlyphSlot_Oblique(FT_GlyphSlot  slot)
{
    FT_Matrix    transform;
    FT_Outline  *outline = &slot->outline;

    /* only oblique outline glyphs */
    if (slot->format != FT_GLYPH_FORMAT_OUTLINE)
    {
        return;
    }

    /* we don't touch the advance width */
    /* For italic, simply apply a shear transform, with an angle */
    /* of about 12 degrees.                                      */
    transform.xx = 0x10000L;
    transform.yx = 0x00000L;
    transform.xy = 0x06000L;
    transform.yy = 0x10000L;
    FT_Outline_Transform(outline, &transform);
}


/*************************************************************************/
/*************************************************************************/
/****                                                                 ****/
/****   EXPERIMENTAL EMBOLDENING/OUTLINING SUPPORT                    ****/
/****                                                                 ****/
/*************************************************************************/
/*************************************************************************/


FT_EXPORT_DEF(FT_Error)
FT_GlyphSlot_Own_Bitmap(FT_GlyphSlot  slot)
{
    if (slot && slot->format == FT_GLYPH_FORMAT_BITMAP   &&
        !(slot->internal->flags & FT_GLYPH_OWN_BITMAP))
    {
        FT_Bitmap  bitmap;
        FT_Error   error;
        FT_Bitmap_New(&bitmap);
        error = FT_Bitmap_Copy(slot->library, &slot->bitmap, &bitmap);

        if (error)
        {
            return error;
        }

        slot->bitmap = bitmap;
        slot->internal->flags |= FT_GLYPH_OWN_BITMAP;
    }

    return FT_Err_Ok;
}


/* documentation is in ftsynth.h */

FT_EXPORT_DEF(void)
FT_GlyphSlot_Embolden(FT_GlyphSlot  slot)
{
    FT_Library  library = slot->library;
    FT_Face     face    = FT_SLOT_FACE(slot);
    FT_Error    error;
    FT_Pos      xstr, ystr;

    if (slot->format != FT_GLYPH_FORMAT_OUTLINE &&
        slot->format != FT_GLYPH_FORMAT_BITMAP)
    {
        return;
    }

    /* some reasonable strength */
    xstr = FT_MulFix(face->units_per_EM,
                     face->size->metrics.y_scale) / 35;
    ystr = xstr;

    if (slot->format == FT_GLYPH_FORMAT_OUTLINE)
    {
        error = FT_Outline_Embolden(&slot->outline, xstr);
        /* ignore error */
        /* this is more than enough for most glyphs; if you need accurate */
        /* values, you have to call FT_Outline_Get_CBox                   */
        xstr = xstr * 2;
        ystr = xstr;
    }
    else if (slot->format == FT_GLYPH_FORMAT_BITMAP)
    {
        xstr = FT_PIX_FLOOR(xstr);

        if (xstr == 0)
        {
            xstr = 1 << 6;
        }

        ystr = FT_PIX_FLOOR(ystr);
        error = FT_GlyphSlot_Own_Bitmap(slot);

        if (error)
        {
            return;
        }

        error = FT_Bitmap_Embolden(library, &slot->bitmap, xstr, ystr);

        if (error)
        {
            return;
        }
    }

    if (slot->advance.x)
    {
        slot->advance.x += xstr;
    }

    if (slot->advance.y)
    {
        slot->advance.y += ystr;
    }

    slot->metrics.width        += xstr;
    slot->metrics.height       += ystr;
    slot->metrics.horiBearingY += ystr;
    slot->metrics.horiAdvance  += xstr;
    slot->metrics.vertBearingX -= xstr / 2;
    slot->metrics.vertBearingY += ystr;
    slot->metrics.vertAdvance  += ystr;

    if (slot->format == FT_GLYPH_FORMAT_BITMAP)
    {
        slot->bitmap_top += ystr >> 6;
    }
}


/* END */
