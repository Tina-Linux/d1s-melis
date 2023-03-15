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
/*  t1objs.h                                                               */
/*                                                                         */
/*    Type 1 objects manager (specification).                              */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2006 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __T1OBJS_H__
#define __T1OBJS_H__


#include <ft2build.h>
#include FT_INTERNAL_OBJECTS_H
#include FT_CONFIG_CONFIG_H
#include FT_INTERNAL_TYPE1_TYPES_H


FT_BEGIN_HEADER


/* The following structures must be defined by the hinter */
typedef struct T1_Size_Hints_   T1_Size_Hints;
typedef struct T1_Glyph_Hints_  T1_Glyph_Hints;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    T1_Driver                                                          */
/*                                                                       */
/* <Description>                                                         */
/*    A handle to a Type 1 driver object.                                */
/*                                                                       */
typedef struct T1_DriverRec_   *T1_Driver;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    T1_Size                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A handle to a Type 1 size object.                                  */
/*                                                                       */
typedef struct T1_SizeRec_  *T1_Size;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    T1_GlyphSlot                                                       */
/*                                                                       */
/* <Description>                                                         */
/*    A handle to a Type 1 glyph slot object.                            */
/*                                                                       */
typedef struct T1_GlyphSlotRec_  *T1_GlyphSlot;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    T1_CharMap                                                         */
/*                                                                       */
/* <Description>                                                         */
/*    A handle to a Type 1 character mapping object.                     */
/*                                                                       */
/* <Note>                                                                */
/*    The Type 1 format doesn't use a charmap but an encoding table.     */
/*    The driver is responsible for making up charmap objects            */
/*    corresponding to these tables.                                     */
/*                                                                       */
typedef struct T1_CharMapRec_   *T1_CharMap;


/*************************************************************************/
/*                                                                       */
/*                  HERE BEGINS THE TYPE1 SPECIFIC STUFF                 */
/*                                                                       */
/*************************************************************************/


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    T1_SizeRec                                                         */
/*                                                                       */
/* <Description>                                                         */
/*    Type 1 size record.                                                */
/*                                                                       */
typedef struct  T1_SizeRec_
{
    FT_SizeRec  root;

} T1_SizeRec;


FT_LOCAL(void)
T1_Size_Done(T1_Size  size);

FT_LOCAL(FT_Error)
T1_Size_Request(T1_Size          size,
                FT_Size_Request  req);

FT_LOCAL(FT_Error)
T1_Size_Init(T1_Size  size);


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    T1_GlyphSlotRec                                                    */
/*                                                                       */
/* <Description>                                                         */
/*    Type 1 glyph slot record.                                          */
/*                                                                       */
typedef struct  T1_GlyphSlotRec_
{
    FT_GlyphSlotRec  root;

    FT_Bool          hint;
    FT_Bool          scaled;

    FT_Int           max_points;
    FT_Int           max_contours;

    FT_Fixed         x_scale;
    FT_Fixed         y_scale;

} T1_GlyphSlotRec;


FT_LOCAL(FT_Error)
T1_Face_Init(FT_Stream      stream,
             T1_Face        face,
             FT_Int         face_index,
             FT_Int         num_params,
             FT_Parameter  *params);

FT_LOCAL(void)
T1_Face_Done(T1_Face  face);

FT_LOCAL(FT_Error)
T1_GlyphSlot_Init(T1_GlyphSlot  slot);

FT_LOCAL(void)
T1_GlyphSlot_Done(T1_GlyphSlot  slot);

FT_LOCAL(FT_Error)
T1_Driver_Init(T1_Driver  driver);

FT_LOCAL(void)
T1_Driver_Done(T1_Driver  driver);


FT_END_HEADER

#endif /* __T1OBJS_H__ */


/* END */
