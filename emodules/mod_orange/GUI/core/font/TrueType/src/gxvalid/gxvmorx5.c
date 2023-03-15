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
/*  gxvmorx5.c                                                             */
/*                                                                         */
/*    TrueTypeGX/AAT morx table validation                                 */
/*    body for type5 (Contextual Glyph Insertion) subtable.                */
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


/*
 * `morx' subtable type5 (Contextual Glyph Insertion)
 * has format of a StateTable with insertion-glyph-list
 * without name.  However, the 32bit offset from the head
 * of subtable to the i-g-l is given after `entryTable',
 * without variable name specification (the existence of
 * this offset to the table is different from mort type5).
 */


typedef struct  GXV_morx_subtable_type5_StateOptRec_
{
    FT_ULong  insertionGlyphList;
    FT_ULong  insertionGlyphList_length;

}  GXV_morx_subtable_type5_StateOptRec,
*GXV_morx_subtable_type5_StateOptRecData;


#define GXV_MORX_SUBTABLE_TYPE5_HEADER_SIZE \
    ( GXV_STATETABLE_HEADER_SIZE + 4 )


static void gxv_morx_subtable_type5_insertionGlyphList_load(FT_Bytes       table,
        FT_Bytes       limit,
        GXV_Validator  valid)
{
    FT_Bytes  p = table;
    GXV_morx_subtable_type5_StateOptRecData  optdata =
        (GXV_morx_subtable_type5_StateOptRecData)valid->xstatetable.optdata;
    GXV_LIMIT_CHECK(4);
    optdata->insertionGlyphList = FT_NEXT_ULONG(p);
}


static void gxv_morx_subtable_type5_subtable_setup(FT_ULong       table_size,
        FT_ULong       classTable,
        FT_ULong       stateArray,
        FT_ULong       entryTable,
        FT_ULong      *classTable_length_p,
        FT_ULong      *stateArray_length_p,
        FT_ULong      *entryTable_length_p,
        GXV_Validator  valid)
{
    FT_ULong   o[4];
    FT_ULong  *l[4];
    FT_ULong   buff[5];
    GXV_morx_subtable_type5_StateOptRecData  optdata =
        (GXV_morx_subtable_type5_StateOptRecData)valid->xstatetable.optdata;
    o[0] = classTable;
    o[1] = stateArray;
    o[2] = entryTable;
    o[3] = optdata->insertionGlyphList;
    l[0] = classTable_length_p;
    l[1] = stateArray_length_p;
    l[2] = entryTable_length_p;
    l[3] = &(optdata->insertionGlyphList_length);
    gxv_set_length_by_ulong_offset(o, l, buff, 4, table_size, valid);
}


static void gxv_morx_subtable_type5_InsertList_validate(FT_UShort      table_index,
        FT_UShort      count,
        FT_Bytes       table,
        FT_Bytes       limit,
        GXV_Validator  valid)
{
    FT_Bytes p = table + table_index * 2;

    while (p < table + count * 2 + table_index * 2)
    {
        FT_UShort  insert_glyphID;
        GXV_LIMIT_CHECK(2);
        insert_glyphID = FT_NEXT_USHORT(p);
        GXV_TRACE((" 0x%04x", insert_glyphID));
    }

    GXV_TRACE(("\n"));
}


static void gxv_morx_subtable_type5_entry_validate(
    FT_UShort                       state,
    FT_UShort                       flags,
    GXV_StateTable_GlyphOffsetDesc  glyphOffset,
    FT_Bytes                        table,
    FT_Bytes                        limit,
    GXV_Validator                   valid)
{
    FT_Bool    setMark;
    FT_Bool    dontAdvance;
    FT_Bool    currentIsKashidaLike;
    FT_Bool    markedIsKashidaLike;
    FT_Bool    currentInsertBefore;
    FT_Bool    markedInsertBefore;
    FT_Byte    currentInsertCount;
    FT_Byte    markedInsertCount;
    FT_Byte    currentInsertList;
    FT_UShort  markedInsertList;
    FT_UNUSED(state);
    setMark              = FT_BOOL((flags >> 15) & 1);
    dontAdvance          = FT_BOOL((flags >> 14) & 1);
    currentIsKashidaLike = FT_BOOL((flags >> 13) & 1);
    markedIsKashidaLike  = FT_BOOL((flags >> 12) & 1);
    currentInsertBefore  = FT_BOOL((flags >> 11) & 1);
    markedInsertBefore   = FT_BOOL((flags >> 10) & 1);
    currentInsertCount = (FT_Byte)((flags >> 5) & 0x1F);
    markedInsertCount  = (FT_Byte)(flags        & 0x001F);
    currentInsertList = (FT_Byte)(glyphOffset.ul >> 16);
    markedInsertList  = (FT_UShort)(glyphOffset.ul);

    if (currentInsertList && 0 != currentInsertCount)
        gxv_morx_subtable_type5_InsertList_validate(currentInsertList,
                currentInsertCount,
                table, limit,
                valid);

    if (markedInsertList && 0 != markedInsertCount)
        gxv_morx_subtable_type5_InsertList_validate(markedInsertList,
                markedInsertCount,
                table, limit,
                valid);
}


FT_LOCAL_DEF(void)
gxv_morx_subtable_type5_validate(FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid)
{
    FT_Bytes  p = table;
    GXV_morx_subtable_type5_StateOptRec      et_rec;
    GXV_morx_subtable_type5_StateOptRecData  et = &et_rec;
    GXV_NAME_ENTER("morx chain subtable type5 (Glyph Insertion)");
    GXV_LIMIT_CHECK(GXV_MORX_SUBTABLE_TYPE5_HEADER_SIZE);
    valid->xstatetable.optdata =
        et;
    valid->xstatetable.optdata_load_func =
        gxv_morx_subtable_type5_insertionGlyphList_load;
    valid->xstatetable.subtable_setup_func =
        gxv_morx_subtable_type5_subtable_setup;
    valid->xstatetable.entry_glyphoffset_fmt =
        GXV_GLYPHOFFSET_ULONG;
    valid->xstatetable.entry_validate_func =
        gxv_morx_subtable_type5_entry_validate;
    gxv_XStateTable_validate(p, limit, valid);
    GXV_EXIT;
}


/* END */
