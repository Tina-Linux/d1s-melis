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
/*  pfrobjs.h                                                              */
/*                                                                         */
/*    FreeType PFR object methods (specification).                         */
/*                                                                         */
/*  Copyright 2002, 2003, 2004 by                                          */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __PFROBJS_H__
#define __PFROBJS_H__

#include "pfrtypes.h"


FT_BEGIN_HEADER

typedef struct PFR_FaceRec_  *PFR_Face;

typedef struct PFR_SizeRec_  *PFR_Size;

typedef struct PFR_SlotRec_  *PFR_Slot;


typedef struct  PFR_FaceRec_
{
    FT_FaceRec      root;
    PFR_HeaderRec   header;
    PFR_LogFontRec  log_font;
    PFR_PhyFontRec  phy_font;

} PFR_FaceRec;


typedef struct  PFR_SizeRec_
{
    FT_SizeRec  root;

} PFR_SizeRec;


typedef struct  PFR_SlotRec_
{
    FT_GlyphSlotRec  root;
    PFR_GlyphRec     glyph;

} PFR_SlotRec;


FT_LOCAL(FT_Error)
pfr_face_init(FT_Stream      stream,
              FT_Face        face,           /* PFR_Face */
              FT_Int         face_index,
              FT_Int         num_params,
              FT_Parameter  *params);

FT_LOCAL(void)
pfr_face_done(FT_Face  face);                 /* PFR_Face */


FT_LOCAL(FT_Error)
pfr_face_get_kerning(FT_Face     face,        /* PFR_Face */
                     FT_UInt     glyph1,
                     FT_UInt     glyph2,
                     FT_Vector  *kerning);


FT_LOCAL(FT_Error)
pfr_slot_init(FT_GlyphSlot  slot);            /* PFR_Slot */

FT_LOCAL(void)
pfr_slot_done(FT_GlyphSlot  slot);            /* PFR_Slot */


FT_LOCAL(FT_Error)
pfr_slot_load(FT_GlyphSlot  slot,             /* PFR_Slot */
              FT_Size       size,            /* PFR_Size */
              FT_UInt       gindex,
              FT_Int32      load_flags);


FT_END_HEADER

#endif /* __PFROBJS_H__ */


/* END */
