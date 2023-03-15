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
/*  ftpfr.h                                                                */
/*                                                                         */
/*    FreeType API for accessing PFR-specific data (specification only).   */
/*                                                                         */
/*  Copyright 2002, 2003, 2004, 2006 by                                    */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __FTPFR_H__
#define __FTPFR_H__

#include "ft2build.h"
#include FT_FREETYPE_H

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif


FT_BEGIN_HEADER


/*************************************************************************/
/*                                                                       */
/* <Section>                                                             */
/*    pfr_fonts                                                          */
/*                                                                       */
/* <Title>                                                               */
/*    PFR Fonts                                                          */
/*                                                                       */
/* <Abstract>                                                            */
/*    PFR/TrueDoc specific API.                                          */
/*                                                                       */
/* <Description>                                                         */
/*    This section contains the declaration of PFR-specific functions.   */
/*                                                                       */
/*************************************************************************/


/**********************************************************************
 *
 * @function:
 *    FT_Get_PFR_Metrics
 *
 * @description:
 *    Return the outline and metrics resolutions of a given PFR face.
 *
 * @input:
 *    face :: Handle to the input face.  It can be a non-PFR face.
 *
 * @output:
 *    aoutline_resolution ::
 *      Outline resolution.  This is equivalent to `face->units_per_EM'.
 *      Optional (parameter can be NULL).
 *
 *    ametrics_resolution ::
 *      Metrics resolution.  This is equivalent to `outline_resolution'
 *      for non-PFR fonts.  Optional (parameter can be NULL).
 *
 *    ametrics_x_scale ::
 *      A 16.16 fixed-point number used to scale distance expressed
 *      in metrics units to device sub-pixels.  This is equivalent to
 *      `face->size->x_scale', but for metrics only.  Optional (parameter
 *      can be NULL)
 *
 *    ametrics_y_scale ::
 *      Same as `ametrics_x_scale' but for the vertical direction.
 *      optional (parameter can be NULL)
 *
 * @return:
 *    FreeType error code.  0 means success.
 *
 * @note:
 *   If the input face is not a PFR, this function will return an error.
 *   However, in all cases, it will return valid values.
 */
FT_EXPORT(FT_Error)
FT_Get_PFR_Metrics(FT_Face    face,
                   FT_UInt   *aoutline_resolution,
                   FT_UInt   *ametrics_resolution,
                   FT_Fixed  *ametrics_x_scale,
                   FT_Fixed  *ametrics_y_scale);


/**********************************************************************
 *
 * @function:
 *    FT_Get_PFR_Kerning
 *
 * @description:
 *    Return the kerning pair corresponding to two glyphs in a PFR face.
 *    The distance is expressed in metrics units, unlike the result of
 *    @FT_Get_Kerning.
 *
 * @input:
 *    face  :: A handle to the input face.
 *
 *    left  :: Index of the left glyph.
 *
 *    right :: Index of the right glyph.
 *
 * @output:
 *    avector :: A kerning vector.
 *
 * @return:
 *    FreeType error code.  0 means success.
 *
 * @note:
 *    This function always return distances in original PFR metrics
 *    units.  This is unlike @FT_Get_Kerning with the @FT_KERNING_UNSCALED
 *    mode, which always returns distances converted to outline units.
 *
 *    You can use the value of the `x_scale' and `y_scale' parameters
 *    returned by @FT_Get_PFR_Metrics to scale these to device sub-pixels.
 */
FT_EXPORT(FT_Error)
FT_Get_PFR_Kerning(FT_Face     face,
                   FT_UInt     left,
                   FT_UInt     right,
                   FT_Vector  *avector);


/**********************************************************************
 *
 * @function:
 *    FT_Get_PFR_Advance
 *
 * @description:
 *    Return a given glyph advance, expressed in original metrics units,
 *    from a PFR font.
 *
 * @input:
 *    face   :: A handle to the input face.
 *
 *    gindex :: The glyph index.
 *
 * @output:
 *    aadvance :: The glyph advance in metrics units.
 *
 * @return:
 *    FreeType error code.  0 means success.
 *
 * @note:
 *    You can use the `x_scale' or `y_scale' results of @FT_Get_PFR_Metrics
 *    to convert the advance to device sub-pixels (i.e., 1/64th of pixels).
 */
FT_EXPORT(FT_Error)
FT_Get_PFR_Advance(FT_Face   face,
                   FT_UInt   gindex,
                   FT_Pos   *aadvance);

/* */


FT_END_HEADER

#endif /* __FTPFR_H__ */


/* END */
