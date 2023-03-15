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
/*  ftgasp.h                                                               */
/*                                                                         */
/*    Access of TrueType's `gasp' table (specification).                   */
/*                                                                         */
/*  Copyright 2007 by                                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef _FT_GASP_H_
#define _FT_GASP_H_

#include "ft2build.h"
#include FT_FREETYPE_H

/***************************************************************************
 *
 * @section:
 *   gasp_table
 *
 * @title:
 *   Gasp Table
 *
 * @abstract:
 *   Retrieving TrueType `gasp' table entries
 *
 * @description:
 *   The function @FT_Get_Gasp can be used to query a TrueType or OpenType
 *   font for specific entries in their `gasp' table, if any.  This is
 *   mainly useful when implementing native TrueType hinting with the
 *   bytecode interpreter to duplicate the Windows text rendering results.
 */

/*************************************************************************
 *
 * @enum:
 *   FT_GASP_XXX
 *
 * @description:
 *   A list of values and/or bit-flags returned by the @FT_Get_Gasp
 *   function.
 *
 * @values:
 *   FT_GASP_NO_TABLE ::
 *     This special value means that there is no GASP table in this face.
 *     It is up to the client to decide what to do.
 *
 *   FT_GASP_DO_GRIDFIT ::
 *     Grid-fitting and hinting should be performed at the specified ppem.
 *     This *really* means TrueType bytecode interpretation.
 *
 *   FT_GASP_DO_GRAY ::
 *     Anti-aliased rendering should be performed at the specified ppem.
 *
 *   FT_GASP_SYMMETRIC_SMOOTHING ::
 *     Smoothing along multiple axes must be used with ClearType.
 *
 *   FT_GASP_SYMMETRIC_GRIDFIT ::
 *     Grid-fitting must be used with ClearType's symmetric smoothing.
 *
 * @note:
 *   `ClearType' is Microsoft's implementation of LCD rendering, partly
 *   protected by patents.
 *
 * @since:
 *   2.3.0
 */
#define FT_GASP_NO_TABLE               -1
#define FT_GASP_DO_GRIDFIT           0x01
#define FT_GASP_DO_GRAY              0x02
#define FT_GASP_SYMMETRIC_SMOOTHING  0x08
#define FT_GASP_SYMMETRIC_GRIDFIT    0x10


/*************************************************************************
 *
 * @func:
 *   FT_Get_Gasp
 *
 * @description:
 *   Read the `gasp' table from a TrueType or OpenType font file and
 *   return the entry corresponding to a given character pixel size.
 *
 * @input:
 *   face :: The source face handle.
 *   ppem :: The vertical character pixel size.
 *
 * @return:
 *   Bit flags (see @FT_GASP_XXX), or @FT_GASP_NO_TABLE is there is no
 *   `gasp' table in the face.
 *
 * @since:
 *   2.3.0
 */
FT_EXPORT(FT_Int)
FT_Get_Gasp(FT_Face  face,
            FT_UInt  ppem);

/* */

#endif /* _FT_GASP_H_ */


/* END */
