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
/*  ftnames.c                                                              */
/*                                                                         */
/*    Simple interface to access SFNT name tables (which are used          */
/*    to hold font names, copyright info, notices, etc.) (body).           */
/*                                                                         */
/*    This is _not_ used to retrieve glyph names!                          */
/*                                                                         */
/*  Copyright 1996-2001, 2002 by                                           */
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
#include FT_SFNT_NAMES_H
#include FT_INTERNAL_TRUETYPE_TYPES_H
#include FT_INTERNAL_STREAM_H


#ifdef TT_CONFIG_OPTION_SFNT_NAMES


/* documentation is in ftnames.h */

FT_EXPORT_DEF(FT_UInt)
FT_Get_Sfnt_Name_Count(FT_Face  face)
{
    return (face && FT_IS_SFNT(face)) ? ((TT_Face)face)->num_names : 0;
}


/* documentation is in ftnames.h */

FT_EXPORT_DEF(FT_Error)
FT_Get_Sfnt_Name(FT_Face       face,
                 FT_UInt       idx,
                 FT_SfntName  *aname)
{
    FT_Error  error = FT_Err_Invalid_Argument;

    if (aname && face && FT_IS_SFNT(face))
    {
        TT_Face  ttface = (TT_Face)face;

        if (idx < (FT_UInt)ttface->num_names)
        {
            TT_NameEntryRec  *entry = ttface->name_table.names + idx;

            /* load name on demand */
            if (entry->stringLength > 0 && entry->string == NULL)
            {
                FT_Memory  memory = face->memory;
                FT_Stream  stream = face->stream;

                if (FT_NEW_ARRAY(entry->string, entry->stringLength) ||
                    FT_STREAM_SEEK(entry->stringOffset)                ||
                    FT_STREAM_READ(entry->string, entry->stringLength))
                {
                    FT_FREE(entry->string);
                    entry->stringLength = 0;
                }
            }

            aname->platform_id = entry->platformID;
            aname->encoding_id = entry->encodingID;
            aname->language_id = entry->languageID;
            aname->name_id     = entry->nameID;
            aname->string      = (FT_Byte *)entry->string;
            aname->string_len  = entry->stringLength;
            error = FT_Err_Ok;
        }
    }

    return error;
}


#endif /* TT_CONFIG_OPTION_SFNT_NAMES */


/* END */
