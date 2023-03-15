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
/*  pfrcmap.c                                                              */
/*                                                                         */
/*    FreeType PFR cmap handling (body).                                   */
/*                                                                         */
/*  Copyright 2002, 2007 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#include "pfrcmap.h"
#include "pfrobjs.h"
#include FT_INTERNAL_DEBUG_H


FT_CALLBACK_DEF(FT_Error)
pfr_cmap_init(PFR_CMap  cmap)
{
    FT_Error  error = PFR_Err_Ok;
    PFR_Face  face  = (PFR_Face)FT_CMAP_FACE(cmap);
    cmap->num_chars = face->phy_font.num_chars;
    cmap->chars     = face->phy_font.chars;
    /* just for safety, check that the character entries are correctly */
    /* sorted in increasing character code order                       */
    {
        FT_UInt  n;

        for (n = 1; n < cmap->num_chars; n++)
        {
            if (cmap->chars[n - 1].char_code >= cmap->chars[n].char_code)
            {
                error = PFR_Err_Invalid_Table;
                goto Exit;
            }
        }
    }
Exit:
    return error;
}


FT_CALLBACK_DEF(void)
pfr_cmap_done(PFR_CMap  cmap)
{
    cmap->chars     = NULL;
    cmap->num_chars = 0;
}


FT_CALLBACK_DEF(FT_UInt)
pfr_cmap_char_index(PFR_CMap   cmap,
                    FT_UInt32  char_code)
{
    FT_UInt   min = 0;
    FT_UInt   max = cmap->num_chars;
    FT_UInt   mid;
    PFR_Char  gchar;

    while (min < max)
    {
        mid   = min + (max - min) / 2;
        gchar = cmap->chars + mid;

        if (gchar->char_code == char_code)
        {
            return mid + 1;
        }

        if (gchar->char_code < char_code)
        {
            min = mid + 1;
        }
        else
        {
            max = mid;
        }
    }

    return 0;
}


FT_CALLBACK_DEF(FT_UInt)
pfr_cmap_char_next(PFR_CMap    cmap,
                   FT_UInt32  *pchar_code)
{
    FT_UInt    result    = 0;
    FT_UInt32  char_code = *pchar_code + 1;
Restart:
    {
        FT_UInt   min = 0;
        FT_UInt   max = cmap->num_chars;
        FT_UInt   mid;
        PFR_Char  gchar;

        while (min < max)
        {
            mid   = min + ((max - min) >> 1);
            gchar = cmap->chars + mid;

            if (gchar->char_code == char_code)
            {
                result = mid;

                if (result != 0)
                {
                    result++;
                    goto Exit;
                }

                char_code++;
                goto Restart;
            }

            if (gchar->char_code < char_code)
            {
                min = mid + 1;
            }
            else
            {
                max = mid;
            }
        }

        /* we didn't find it, but we have a pair just above it */
        char_code = 0;

        if (min < cmap->num_chars)
        {
            gchar  = cmap->chars + min;
            result = min;

            if (result != 0)
            {
                result++;
                char_code = gchar->char_code;
            }
        }
    }
Exit:
    *pchar_code = char_code;
    return result;
}


FT_CALLBACK_TABLE_DEF const FT_CMap_ClassRec
pfr_cmap_class_rec =
{
    sizeof(PFR_CMapRec),

    (FT_CMap_InitFunc)     pfr_cmap_init,
    (FT_CMap_DoneFunc)     pfr_cmap_done,
    (FT_CMap_CharIndexFunc)pfr_cmap_char_index,
    (FT_CMap_CharNextFunc) pfr_cmap_char_next
};


/* END */
