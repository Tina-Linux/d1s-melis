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
/*  gxvlcar.c                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT lcar table validation (body).                         */
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
#define FT_COMPONENT  trace_gxvlcar


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                      Data and Types                           *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

typedef struct  GXV_lcar_DataRec_
{
    FT_UShort  format;

} GXV_lcar_DataRec, *GXV_lcar_Data;


#define GXV_LCAR_DATA( FIELD )  GXV_TABLE_DATA( lcar, FIELD )


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                      UTILITY FUNCTIONS                        *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

static void gxv_lcar_partial_validate(FT_UShort      partial,
                                      FT_UShort      glyph,
                                      GXV_Validator  valid)
{
    GXV_NAME_ENTER("partial");

    if (GXV_LCAR_DATA(format) != 1)
    {
        goto Exit;
    }

    gxv_ctlPoint_validate(glyph, partial, valid);
Exit:
    GXV_EXIT;
}


static void gxv_lcar_LookupValue_validate(FT_UShort            glyph,
        GXV_LookupValueDesc  value,
        GXV_Validator        valid)
{
    FT_Bytes   p     = valid->root->base + value.u;
    FT_Bytes   limit = valid->root->limit;
    FT_UShort  count;
    FT_Short   partial;
    FT_UShort  i;
    GXV_NAME_ENTER("element in lookupTable");
    GXV_LIMIT_CHECK(2);
    count = FT_NEXT_USHORT(p);
    GXV_LIMIT_CHECK(2 * count);

    for (i = 0; i < count; i++)
    {
        partial = FT_NEXT_SHORT(p);
        gxv_lcar_partial_validate(partial, glyph, valid);
    }

    GXV_EXIT;
}


/*
  +------ lcar --------------------+
  |                                |
  |      +===============+         |
  |      | looup header  |         |
  |      +===============+         |
  |      | BinSrchHeader |         |
  |      +===============+         |
  |      | lastGlyph[0]  |         |
  |      +---------------+         |
  |      | firstGlyph[0] |         |  head of lcar sfnt table
  |      +---------------+         |             +
  |      | offset[0]     |    ->   |          offset            [byte]
  |      +===============+         |             +
  |      | lastGlyph[1]  |         | (glyphID - firstGlyph) * 2 [byte]
  |      +---------------+         |
  |      | firstGlyph[1] |         |
  |      +---------------+         |
  |      | offset[1]     |         |
  |      +===============+         |
  |                                |
  |       ....                     |
  |                                |
  |      16bit value array         |
  |      +===============+         |
  +------|     value     | <-------+
  |       ....
  |
  |
  |
  |
  |
  +---->  lcar values...handled by lcar callback function
*/

static GXV_LookupValueDesc gxv_lcar_LookupFmt4_transit(FT_UShort            relative_gindex,
        GXV_LookupValueDesc  base_value,
        FT_Bytes             lookuptbl_limit,
        GXV_Validator        valid)
{
    FT_Bytes             p;
    FT_Bytes             limit;
    FT_UShort            offset;
    GXV_LookupValueDesc  value;
    FT_UNUSED(lookuptbl_limit);
    /* XXX: check range? */
    offset = (FT_UShort)(base_value.u +
                         relative_gindex * sizeof(FT_UShort));
    p      = valid->root->base + offset;
    limit  = valid->root->limit;
    GXV_LIMIT_CHECK(2);
    value.u = FT_NEXT_USHORT(p);
    return value;
}


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                          lcar TABLE                           *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

FT_LOCAL_DEF(void)
gxv_lcar_validate(FT_Bytes      table,
                  FT_Face       face,
                  FT_Validator  ftvalid)
{
    FT_Bytes          p     = table;
    FT_Bytes          limit = 0;
    GXV_ValidatorRec  validrec;
    GXV_Validator     valid = &validrec;
    GXV_lcar_DataRec  lcarrec;
    GXV_lcar_Data     lcar = &lcarrec;
    FT_Fixed          version;
    valid->root       = ftvalid;
    valid->table_data = lcar;
    valid->face       = face;
    FT_TRACE3(("validating `lcar' table\n"));
    GXV_INIT;
    GXV_LIMIT_CHECK(4 + 2);
    version = FT_NEXT_ULONG(p);
    GXV_LCAR_DATA(format) = FT_NEXT_USHORT(p);

    if (version != 0x00010000UL)
    {
        FT_INVALID_FORMAT;
    }

    if (GXV_LCAR_DATA(format) > 1)
    {
        FT_INVALID_FORMAT;
    }

    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_lcar_LookupValue_validate;
    valid->lookupfmt4_trans = gxv_lcar_LookupFmt4_transit;
    gxv_LookupTable_validate(p, limit, valid);
    FT_TRACE4(("\n"));
}


/* END */
