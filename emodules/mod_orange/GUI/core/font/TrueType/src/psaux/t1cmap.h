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
/*  t1cmap.h                                                               */
/*                                                                         */
/*    Type 1 character map support (specification).                        */
/*                                                                         */
/*  Copyright 2002, 2003, 2006 by                                          */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __T1CMAP_H__
#define __T1CMAP_H__

#include <ft2build.h>
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_TYPE1_TYPES_H

FT_BEGIN_HEADER


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****          TYPE1 STANDARD (AND EXPERT) ENCODING CMAPS           *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

/* standard (and expert) encoding cmaps */
typedef struct T1_CMapStdRec_  *T1_CMapStd;

typedef struct  T1_CMapStdRec_
{
    FT_CMapRec                cmap;

    const FT_UShort          *code_to_sid;
    PS_Adobe_Std_StringsFunc  sid_to_string;

    FT_UInt                   num_glyphs;
    const char *const        *glyph_names;

} T1_CMapStdRec;


FT_CALLBACK_TABLE const FT_CMap_ClassRec
t1_cmap_standard_class_rec;

FT_CALLBACK_TABLE const FT_CMap_ClassRec
t1_cmap_expert_class_rec;


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                  TYPE1 CUSTOM ENCODING CMAP                   *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

typedef struct T1_CMapCustomRec_  *T1_CMapCustom;

typedef struct  T1_CMapCustomRec_
{
    FT_CMapRec  cmap;
    FT_UInt     first;
    FT_UInt     count;
    FT_UShort  *indices;

} T1_CMapCustomRec;


FT_CALLBACK_TABLE const FT_CMap_ClassRec
t1_cmap_custom_class_rec;


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****             TYPE1 SYNTHETIC UNICODE ENCODING CMAP             *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

/* unicode (synthetic) cmaps */

FT_CALLBACK_TABLE const FT_CMap_ClassRec
t1_cmap_unicode_class_rec;

/* */


FT_END_HEADER

#endif /* __T1CMAP_H__ */


/* END */
