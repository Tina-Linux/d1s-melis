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
/*  gxvopbd.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT opbd table validation (body).                         */
/*                                                                         */
/*  Copyright 2004, 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* gxvalid is derived from both gxlayout module and otvalid module.        */
/* Development of gxlayout is supported by the Information-technology      */
/* Promotion Agency(IPA), Japan.                                           */
/*                                                                         */
/***************************************************************************/
#include "gxvalid.h"
#include "gxvcommn.h"


/*************************************************************************/
/*                                                                       */
/* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
/* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
/* messages during execution.                                            */
/*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvopbd


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                      Data and Types                           *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

typedef struct  GXV_opbd_DataRec_
{
    FT_UShort  format;
    FT_UShort  valueOffset_min;

} GXV_opbd_DataRec, *GXV_opbd_Data;


#define GXV_OPBD_DATA( FIELD )  GXV_TABLE_DATA( opbd, FIELD )


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                      UTILITY FUNCTIONS                        *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

static void gxv_opbd_LookupValue_validate(FT_UShort            glyph,
        GXV_LookupValueDesc  value,
        GXV_Validator        valid)
{
    /* offset in LookupTable is measured from the head of opbd table */
    FT_Bytes   p     = valid->root->base + value.u;
    FT_Bytes   limit = valid->root->limit;
    FT_Short   delta_value;
    int        i;

    if (value.u < GXV_OPBD_DATA(valueOffset_min))
    {
        GXV_OPBD_DATA(valueOffset_min) = value.u;
    }

    for (i = 0; i < 4; i++)
    {
        GXV_LIMIT_CHECK(2);
        delta_value = FT_NEXT_SHORT(p);

        if (GXV_OPBD_DATA(format))        /* format 1, value is ctrl pt. */
        {
            if (delta_value == -1)
            {
                continue;
            }

            gxv_ctlPoint_validate(glyph, delta_value, valid);
        }
        else                              /* format 0, value is distance */
        {
            continue;
        }
    }
}


/*
  opbd ---------------------+
                            |
  +===============+         |
  | lookup header |         |
  +===============+         |
  | BinSrchHeader |         |
  +===============+         |
  | lastGlyph[0]  |         |
  +---------------+         |
  | firstGlyph[0] |         |  head of opbd sfnt table
  +---------------+         |             +
  | offset[0]     |    ->   |          offset            [byte]
  +===============+         |             +
  | lastGlyph[1]  |         | (glyphID - firstGlyph) * 4 * sizeof(FT_Short) [byte]
  +---------------+         |
  | firstGlyph[1] |         |
  +---------------+         |
  | offset[1]     |         |
  +===============+         |
                            |
   ....                     |
                            |
  48bit value array         |
  +===============+         |
  |     value     | <-------+
  |               |
  |               |
  |               |
  +---------------+
  .... */

static GXV_LookupValueDesc gxv_opbd_LookupFmt4_transit(FT_UShort            relative_gindex,
        GXV_LookupValueDesc  base_value,
        FT_Bytes             lookuptbl_limit,
        GXV_Validator        valid)
{
    GXV_LookupValueDesc  value;
    FT_UNUSED(lookuptbl_limit);
    FT_UNUSED(valid);
    /* XXX: check range? */
    value.u = (FT_UShort)(base_value.u +
                          relative_gindex * 4 * sizeof(FT_Short));
    return value;
}


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                         opbd TABLE                            *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

FT_LOCAL_DEF(void)
gxv_opbd_validate(FT_Bytes      table,
                  FT_Face       face,
                  FT_Validator  ftvalid)
{
    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;
    GXV_opbd_DataRec  opbdrec;
    GXV_opbd_Data     opbd  = &opbdrec;
    FT_Bytes          p     = table;
    FT_Bytes          limit = 0;
    FT_ULong  version;
    valid->root       = ftvalid;
    valid->table_data = opbd;
    valid->face       = face;
    FT_TRACE3(("validating `opbd' table\n"));
    GXV_INIT;
    GXV_OPBD_DATA(valueOffset_min) = 0xFFFFU;
    GXV_LIMIT_CHECK(4 + 2);
    version                 = FT_NEXT_ULONG(p);
    GXV_OPBD_DATA(format) = FT_NEXT_USHORT(p);
    /* only 0x00010000 is defined (1996) */
    GXV_TRACE(("(version=0x%08x)\n", version));

    if (0x00010000UL != version)
    {
        FT_INVALID_FORMAT;
    }

    /* only values 0 and 1 are defined (1996) */
    GXV_TRACE(("(format=0x%04x)\n", GXV_OPBD_DATA(format)));

    if (0x0001 < GXV_OPBD_DATA(format))
    {
        FT_INVALID_FORMAT;
    }

    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_opbd_LookupValue_validate;
    valid->lookupfmt4_trans = gxv_opbd_LookupFmt4_transit;
    gxv_LookupTable_validate(p, limit, valid);
    p += valid->subtable_length;

    if (p > table + GXV_OPBD_DATA(valueOffset_min))
    {
        GXV_TRACE((
                      "found overlap between LookupTable and opbd_value array\n"));
        FT_INVALID_OFFSET;
    }

    FT_TRACE4(("\n"));
}


/* END */
