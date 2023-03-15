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
/*  pshrec.h                                                               */
/*                                                                         */
/*    Postscript (Type1/Type2) hints recorder (specification).             */
/*                                                                         */
/*  Copyright 2001, 2002, 2003, 2006 by                                    */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
/**************************************************************************/
/*                                                                        */
/*  The functions defined here are called from the Type 1, CID and CFF    */
/*  font drivers to record the hints of a given character/glyph.          */
/*                                                                        */
/*  The hints are recorded in a unified format, and are later processed   */
/*  by the `optimizer' and `fitter' to adjust the outlines to the pixel   */
/*  grid.                                                                 */
/*                                                                        */
/**************************************************************************/


#ifndef __PSHREC_H__
#define __PSHREC_H__


#include <ft2build.h>
#include FT_INTERNAL_POSTSCRIPT_HINTS_H
#include "pshglob.h"


FT_BEGIN_HEADER


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                 GLYPH HINTS RECORDER INTERNALS                *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

/* handle to hint record */
typedef struct PS_HintRec_  *PS_Hint;

/* hint types */
typedef enum
{
    PS_HINT_TYPE_1 = 1,
    PS_HINT_TYPE_2 = 2

} PS_Hint_Type;


/* hint flags */
typedef enum
{
    PS_HINT_FLAG_GHOST  = 1,
    PS_HINT_FLAG_BOTTOM = 2

} PS_Hint_Flags;


/* hint descriptor */
typedef struct  PS_HintRec_
{
    FT_Int   pos;
    FT_Int   len;
    FT_UInt  flags;

} PS_HintRec;


#define ps_hint_is_active( x )  ( (x)->flags & PS_HINT_FLAG_ACTIVE )
#define ps_hint_is_ghost( x )   ( (x)->flags & PS_HINT_FLAG_GHOST  )
#define ps_hint_is_bottom( x )  ( (x)->flags & PS_HINT_FLAG_BOTTOM )


/* hints table descriptor */
typedef struct  PS_Hint_TableRec_
{
    FT_UInt  num_hints;
    FT_UInt  max_hints;
    PS_Hint  hints;

} PS_Hint_TableRec, *PS_Hint_Table;


/* hint and counter mask descriptor */
typedef struct  PS_MaskRec_
{
    FT_UInt   num_bits;
    FT_UInt   max_bits;
    FT_Byte  *bytes;
    FT_UInt   end_point;

} PS_MaskRec, *PS_Mask;


/* masks and counters table descriptor */
typedef struct  PS_Mask_TableRec_
{
    FT_UInt  num_masks;
    FT_UInt  max_masks;
    PS_Mask  masks;

} PS_Mask_TableRec, *PS_Mask_Table;


/* dimension-specific hints descriptor */
typedef struct  PS_DimensionRec_
{
    PS_Hint_TableRec  hints;
    PS_Mask_TableRec  masks;
    PS_Mask_TableRec  counters;

} PS_DimensionRec, *PS_Dimension;


/* glyph hints descriptor                                */
/* dimension 0 => X coordinates + vertical hints/stems   */
/* dimension 1 => Y coordinates + horizontal hints/stems */
typedef struct  PS_HintsRec_
{
    FT_Memory        memory;
    FT_Error         error;
    FT_UInt32        magic;
    PS_Hint_Type     hint_type;
    PS_DimensionRec  dimension[2];

} PS_HintsRec, *PS_Hints;

/* */

/* initialize hints recorder */
FT_LOCAL(FT_Error)
ps_hints_init(PS_Hints   hints,
              FT_Memory  memory);

/* finalize hints recorder */
FT_LOCAL(void)
ps_hints_done(PS_Hints  hints);

/* initialize Type1 hints recorder interface */
FT_LOCAL(void)
t1_hints_funcs_init(T1_Hints_FuncsRec  *funcs);

/* initialize Type2 hints recorder interface */
FT_LOCAL(void)
t2_hints_funcs_init(T2_Hints_FuncsRec  *funcs);


#ifdef DEBUG_HINTER
extern PS_Hints  ps_debug_hints;
extern  int      ps_debug_no_horz_hints;
extern  int      ps_debug_no_vert_hints;
#endif

/* */


FT_END_HEADER


#endif /* __PS_HINTER_RECORD_H__ */


/* END */
