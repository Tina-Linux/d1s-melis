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
/*  gxvfeat.h                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT feat table validation (specification).                */
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
#ifndef __GXVFEAT_H__
#define __GXVFEAT_H__


#include "gxvalid.h"
#include "gxvcommn.h"


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                Registry predefined by Apple                   *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

/* TODO: More compact format */
typedef struct  GXV_Feature_RegistryRec_
{
    FT_Bool  existence;
    FT_Bool  apple_reserved;
    FT_Bool  exclusive;
    FT_Byte  nSettings;

} GX_Feature_RegistryRec;


#define gxv_feat_registry_length                  \
    ( sizeof ( gxv_feat_registry ) /        \
      sizeof ( GX_Feature_RegistryRec ) )


static GX_Feature_RegistryRec  gxv_feat_registry[] =
{
    /* Generated from gxvfgen.c */
    {1, 0, 0,  1},   /* All Typographic Features */
    {1, 0, 0,  8},   /* Ligatures */
    {1, 0, 1,  3},   /* Cursive Connection */
    {1, 0, 1,  6},   /* Letter Case */
    {1, 0, 0,  1},   /* Vertical Substitution */
    {1, 0, 0,  1},   /* Linguistic Rearrangement */
    {1, 0, 1,  2},   /* Number Spacing */
    {1, 1, 0,  0},   /* Apple Reserved 1 */
    {1, 0, 0,  5},   /* Smart Swashes */
    {1, 0, 1,  3},   /* Diacritics */
    {1, 0, 1,  4},   /* Vertical Position */
    {1, 0, 1,  3},   /* Fractions */
    {1, 1, 0,  0},   /* Apple Reserved 2 */
    {1, 0, 0,  1},   /* Overlapping Characters */
    {1, 0, 0,  6},   /* Typographic Extras */
    {1, 0, 0,  5},   /* Mathematical Extras */
    {1, 0, 1,  7},   /* Ornament Sets */
    {1, 0, 1,  1},   /* Character Alternatives */
    {1, 0, 1,  5},   /* Design Complexity */
    {1, 0, 1,  6},   /* Style Options */
    {1, 0, 1, 11},   /* Character Shape */
    {1, 0, 1,  2},   /* Number Case */
    {1, 0, 1,  4},   /* Text Spacing */
    {1, 0, 1, 10},   /* Transliteration */
    {1, 0, 1,  9},   /* Annotation */
    {1, 0, 1,  2},   /* Kana Spacing */
    {1, 0, 1,  2},   /* Ideographic Spacing */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {0, 0, 0,  0},   /* __EMPTY__ */
    {1, 0, 1,  4},   /* Text Spacing */
    {1, 0, 1,  2},   /* Kana Spacing */
    {1, 0, 1,  2},   /* Ideographic Spacing */
    {1, 0, 1,  4},   /* CJK Roman Spacing */
};


#endif /* __GXVFEAT_H__ */


/* END */
