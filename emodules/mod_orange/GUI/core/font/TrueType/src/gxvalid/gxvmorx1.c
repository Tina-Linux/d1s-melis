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
/*  gxvmorx1.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT morx table validation                                 */
/*    body for type1 (Contextual Substitution) subtable.                   */
/*                                                                         */
/*  Copyright 2005, 2007 by suzuki toshiya, Masatake YAMATO, Red Hat K.K., */
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
#include "gxvmorx.h"


/*************************************************************************/
/*                                                                       */
/* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
/* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
/* messages during execution.                                            */
/*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmorx


typedef struct  GXV_morx_subtable_type1_StateOptRec_
{
    FT_ULong   substitutionTable;
    FT_ULong   substitutionTable_length;
    FT_UShort  substitutionTable_num_lookupTables;

}  GXV_morx_subtable_type1_StateOptRec,
*GXV_morx_subtable_type1_StateOptRecData;


#define GXV_MORX_SUBTABLE_TYPE1_HEADER_SIZE \
    ( GXV_STATETABLE_HEADER_SIZE + 2 )


static void gxv_morx_subtable_type1_substitutionTable_load(FT_Bytes       table,
        FT_Bytes       limit,
        GXV_Validator  valid)
{
    FT_Bytes  p = table;
    GXV_morx_subtable_type1_StateOptRecData  optdata =
        (GXV_morx_subtable_type1_StateOptRecData)valid->xstatetable.optdata;
    GXV_LIMIT_CHECK(2);
    optdata->substitutionTable = FT_NEXT_USHORT(p);
}


static void gxv_morx_subtable_type1_subtable_setup(FT_ULong       table_size,
        FT_ULong       classTable,
        FT_ULong       stateArray,
        FT_ULong       entryTable,
        FT_ULong      *classTable_length_p,
        FT_ULong      *stateArray_length_p,
        FT_ULong      *entryTable_length_p,
        GXV_Validator  valid)
{
    FT_ULong  o[4];
    FT_ULong  *l[4];
    FT_ULong  buff[5];
    GXV_morx_subtable_type1_StateOptRecData  optdata =
        (GXV_morx_subtable_type1_StateOptRecData)valid->xstatetable.optdata;
    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    o[3] = optdata->substitutionTable;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;
    l[3] = &(optdata->substitutionTable_length);
    gxv_set_length_by_ulong_offset(o, l, buff, 4, table_size, valid);
}


static void gxv_morx_subtable_type1_entry_validate(
    FT_UShort                       state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetDesc  glyphOffset,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid)
{
    FT_UShort  setMark;
    FT_UShort  dontAdvance;
    FT_UShort  reserved;
    FT_Short   markIndex;
    FT_Short   currentIndex;
    GXV_morx_subtable_type1_StateOptRecData  optdata =
        (GXV_morx_subtable_type1_StateOptRecData)valid->xstatetable.optdata;
    FT_UNUSED(state);
    FT_UNUSED(table);
    FT_UNUSED(limit);
    setMark      = (FT_UShort)((flags >> 15) & 1);
    dontAdvance  = (FT_UShort)((flags >> 14) & 1);
    reserved = (FT_UShort)(flags & 0x3FFF);
    markIndex    = (FT_Short)(glyphOffset.ul >> 16);
    currentIndex = (FT_Short)(glyphOffset.ul);
    GXV_TRACE((" setMark=%01d dontAdvance=%01d\n",
               setMark, dontAdvance));

    if (0 < reserved)
    {
        GXV_TRACE((" non-zero bits found in reserved range\n"));

        if (valid->root->level >= FT_VALIDATE_PARANOID)
        {
            FT_INVALID_DATA;
        }
    }

    GXV_TRACE(("markIndex = %d, currentIndex = %d\n",
               markIndex, currentIndex));

    if (optdata->substitutionTable_num_lookupTables < markIndex + 1)
        optdata->substitutionTable_num_lookupTables =
            (FT_Short)(markIndex + 1);

    if (optdata->substitutionTable_num_lookupTables < currentIndex + 1)
        optdata->substitutionTable_num_lookupTables =
            (FT_Short)(currentIndex + 1);
}


static void gxv_morx_subtable_type1_LookupValue_validate(FT_UShort            glyph,
        GXV_LookupValueDesc  value,
        GXV_Validator        valid)
{
    FT_UNUSED(glyph);   /* for the non-debugging case */
    GXV_TRACE(("morx subtable type1 subst.: %d -> %d\n", glyph, value.u));

    if (value.u > valid->face->num_glyphs)
    {
        FT_INVALID_GLYPH_ID;
    }
}


static GXV_LookupValueDesc gxv_morx_subtable_type1_LookupFmt4_transit(
    FT_UShort            relative_gindex,
    GXV_LookupValueDesc  base_value,
    FT_Bytes             lookuptbl_limit,
    GXV_Validator        valid)
{
    FT_Bytes             p;
    FT_Bytes             limit;
    FT_UShort            offset;
    GXV_LookupValueDesc  value;
    /* XXX: check range? */
    offset = (FT_UShort)(base_value.u +
                         relative_gindex * sizeof(FT_UShort));
    p     = valid->lookuptbl_head + offset;
    limit = lookuptbl_limit;
    GXV_LIMIT_CHECK(2);
    value.u = FT_NEXT_USHORT(p);
    return value;
}


/*
 * TODO: length should be limit?
 **/
static void gxv_morx_subtable_type1_substitutionTable_validate(FT_Bytes       table,
        FT_Bytes       limit,
        GXV_Validator  valid)
{
    FT_Bytes   p = table;
    FT_UShort  i;
    GXV_morx_subtable_type1_StateOptRecData  optdata =
        (GXV_morx_subtable_type1_StateOptRecData)valid->xstatetable.optdata;
    /* TODO: calculate offset/length for each lookupTables */
    valid->lookupval_sign   = GXV_LOOKUPVALUE_UNSIGNED;
    valid->lookupval_func   = gxv_morx_subtable_type1_LookupValue_validate;
    valid->lookupfmt4_trans = gxv_morx_subtable_type1_LookupFmt4_transit;

    for (i = 0; i < optdata->substitutionTable_num_lookupTables; i++)
    {
        FT_ULong  offset;
        GXV_LIMIT_CHECK(4);
        offset = FT_NEXT_ULONG(p);
        gxv_LookupTable_validate(table + offset, limit, valid);
    }

    /* TODO: overlapping of lookupTables in substitutionTable */
}


/*
 * subtable for Contextual glyph substitution is a modified StateTable.
 * In addition to classTable, stateArray, entryTable, the field
 * `substitutionTable' is added.
 */
FT_LOCAL_DEF(void)
gxv_morx_subtable_type1_validate(FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid)
{
    FT_Bytes  p = table;
    GXV_morx_subtable_type1_StateOptRec  st_rec;
    GXV_NAME_ENTER("morx chain subtable type1 (Contextual Glyph Subst)");
    GXV_LIMIT_CHECK(GXV_MORX_SUBTABLE_TYPE1_HEADER_SIZE);
    st_rec.substitutionTable_num_lookupTables = 0;
    valid->xstatetable.optdata =
        &st_rec;
    valid->xstatetable.optdata_load_func =
        gxv_morx_subtable_type1_substitutionTable_load;
    valid->xstatetable.subtable_setup_func =
        gxv_morx_subtable_type1_subtable_setup;
    valid->xstatetable.entry_glyphoffset_fmt =
        GXV_GLYPHOFFSET_ULONG;
    valid->xstatetable.entry_validate_func =
        gxv_morx_subtable_type1_entry_validate;
    gxv_XStateTable_validate(p, limit, valid);
    gxv_morx_subtable_type1_substitutionTable_validate(
        table + st_rec.substitutionTable,
        table + st_rec.substitutionTable + st_rec.substitutionTable_length,
        valid);
    GXV_EXIT;
}


/* END */
