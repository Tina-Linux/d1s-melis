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
/*  gxvmort.h                                                              */
/*                                                                         */
/*    TrueTypeGX/AAT common definition for mort table (specification).     */
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
#ifndef __GXVMORT_H__
#define __GXVMORT_H__

#include "gxvalid.h"
#include "gxvcommn.h"

#include FT_SFNT_NAMES_H


typedef struct  GXV_mort_featureRec_
{
    FT_UShort  featureType;
    FT_UShort  featureSetting;
    FT_ULong   enableFlags;
    FT_ULong   disableFlags;

} GXV_mort_featureRec, *GXV_mort_feature;

#define GXV_MORT_FEATURE_OFF  {0, 1, 0x00000000UL, 0x00000000UL}

#define IS_GXV_MORT_FEATURE_OFF( f )              \
    ( (f).featureType    == 0            || \
      (f).featureSetting == 1            || \
      (f).enableFlags    == 0x00000000UL || \
      (f).disableFlags   == 0x00000000UL )


FT_LOCAL(void)
gxv_mort_featurearray_validate(FT_Bytes       table,
                               FT_Bytes       limit,
                               FT_UInt        nFeatureFlags,
                               GXV_Validator  valid);

FT_LOCAL(void)
gxv_mort_coverage_validate(FT_UShort      coverage,
                           GXV_Validator  valid);

FT_LOCAL(void)
gxv_mort_subtable_type0_validate(FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid);

FT_LOCAL(void)
gxv_mort_subtable_type1_validate(FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid);

FT_LOCAL(void)
gxv_mort_subtable_type2_validate(FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid);

FT_LOCAL(void)
gxv_mort_subtable_type4_validate(FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid);

FT_LOCAL(void)
gxv_mort_subtable_type5_validate(FT_Bytes       table,
                                 FT_Bytes       limit,
                                 GXV_Validator  valid);


#endif /* __GXVMORT_H__ */


/* END */
