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
/*  t42objs.h                                                              */
/*                                                                         */
/*    Type 42 objects manager (specification).                             */
/*                                                                         */
/*  Copyright 2002, 2003, 2006, 2007 by Roberto Alameda.                   */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __T42OBJS_H__
#define __T42OBJS_H__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPE1_TABLES_H
#include FT_INTERNAL_TYPE1_TYPES_H
#include "t42types.h"
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_DRIVER_H
#include FT_SERVICE_POSTSCRIPT_CMAPS_H
#include FT_INTERNAL_POSTSCRIPT_HINTS_H


FT_BEGIN_HEADER


/* Type42 size */
typedef struct  T42_SizeRec_
{
    FT_SizeRec  root;
    FT_Size     ttsize;

} T42_SizeRec, *T42_Size;


/* Type42 slot */
typedef struct  T42_GlyphSlotRec_
{
    FT_GlyphSlotRec  root;
    FT_GlyphSlot     ttslot;

} T42_GlyphSlotRec, *T42_GlyphSlot;


/* Type 42 driver */
typedef struct  T42_DriverRec_
{
    FT_DriverRec     root;
    FT_Driver_Class  ttclazz;
    void            *extension_component;

} T42_DriverRec, *T42_Driver;


/* */


FT_LOCAL(FT_Error)
T42_Face_Init(FT_Stream      stream,
              T42_Face       face,
              FT_Int         face_index,
              FT_Int         num_params,
              FT_Parameter  *params);


FT_LOCAL(void)
T42_Face_Done(T42_Face  face);


FT_LOCAL(FT_Error)
T42_Size_Init(T42_Size  size);


FT_LOCAL(FT_Error)
T42_Size_Request(T42_Size         size,
                 FT_Size_Request  req);


FT_LOCAL(FT_Error)
T42_Size_Select(T42_Size  size,
                FT_ULong  strike_index);


FT_LOCAL(void)
T42_Size_Done(T42_Size  size);


FT_LOCAL(FT_Error)
T42_GlyphSlot_Init(T42_GlyphSlot  slot);


FT_LOCAL(FT_Error)
T42_GlyphSlot_Load(FT_GlyphSlot  glyph,
                   FT_Size       size,
                   FT_UInt       glyph_index,
                   FT_Int32      load_flags);

FT_LOCAL(void)
T42_GlyphSlot_Done(T42_GlyphSlot slot);


FT_LOCAL(FT_Error)
T42_Driver_Init(T42_Driver  driver);

FT_LOCAL(void)
T42_Driver_Done(T42_Driver  driver);

/* */

FT_END_HEADER


#endif /* __T42OBJS_H__ */


/* END */
