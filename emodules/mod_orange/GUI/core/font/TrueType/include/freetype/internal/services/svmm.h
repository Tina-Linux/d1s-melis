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
/*  svmm.h                                                                 */
/*                                                                         */
/*    The FreeType Multiple Masters and GX var services (specification).   */
/*                                                                         */
/*  Copyright 2003, 2004 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __SVMM_H__
#define __SVMM_H__

#include FT_INTERNAL_SERVICE_H


FT_BEGIN_HEADER


/*
 *  A service used to manage multiple-masters data in a given face.
 *
 *  See the related APIs in `ftmm.h' (FT_MULTIPLE_MASTERS_H).
 *
 */

#define FT_SERVICE_ID_MULTI_MASTERS  "multi-masters"


typedef FT_Error
(*FT_Get_MM_Func)(FT_Face           face,
                  FT_Multi_Master  *master);

typedef FT_Error
(*FT_Get_MM_Var_Func)(FT_Face      face,
                      FT_MM_Var **master);

typedef FT_Error
(*FT_Set_MM_Design_Func)(FT_Face   face,
                         FT_UInt   num_coords,
                         FT_Long  *coords);

typedef FT_Error
(*FT_Set_Var_Design_Func)(FT_Face    face,
                          FT_UInt    num_coords,
                          FT_Fixed  *coords);

typedef FT_Error
(*FT_Set_MM_Blend_Func)(FT_Face   face,
                        FT_UInt   num_coords,
                        FT_Long  *coords);


FT_DEFINE_SERVICE(MultiMasters)
{
    FT_Get_MM_Func          get_mm;
    FT_Set_MM_Design_Func   set_mm_design;
    FT_Set_MM_Blend_Func    set_mm_blend;
    FT_Get_MM_Var_Func      get_mm_var;
    FT_Set_Var_Design_Func  set_var_design;
};

/* */


FT_END_HEADER

#endif /* __SVMM_H__ */


/* END */
