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
/*  aflatin.h                                                              */
/*                                                                         */
/*    Auto-fitter hinting routines for latin script (specification).       */
/*                                                                         */
/*  Copyright 2003, 2004, 2005, 2006, 2007 by                              */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __AFLATIN_H__
#define __AFLATIN_H__

#include "afhints.h"


FT_BEGIN_HEADER


/* the latin-specific script class */

FT_CALLBACK_TABLE const AF_ScriptClassRec
af_latin_script_class;


/* constants are given with units_per_em == 2048 in mind */
#define AF_LATIN_CONSTANT( metrics, c ) \
    ( ( (c) * (FT_Long)( (AF_LatinMetrics)(metrics) )->units_per_em ) / 2048 )


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****            L A T I N   G L O B A L   M E T R I C S            *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/


/*
 *  The following declarations could be embedded in the file `aflatin.c';
 *  they have been made semi-public to allow alternate script hinters to
 *  re-use some of them.
 */


/* Latin (global) metrics management */

enum
{
    AF_LATIN_BLUE_CAPITAL_TOP,
    AF_LATIN_BLUE_CAPITAL_BOTTOM,
    AF_LATIN_BLUE_SMALL_F_TOP,
    AF_LATIN_BLUE_SMALL_TOP,
    AF_LATIN_BLUE_SMALL_BOTTOM,
    AF_LATIN_BLUE_SMALL_MINOR,

    AF_LATIN_BLUE_MAX
};


#define AF_LATIN_IS_TOP_BLUE( b )  ( (b) == AF_LATIN_BLUE_CAPITAL_TOP || \
                                     (b) == AF_LATIN_BLUE_SMALL_F_TOP || \
                                     (b) == AF_LATIN_BLUE_SMALL_TOP   )

#define AF_LATIN_MAX_WIDTHS  16
#define AF_LATIN_MAX_BLUES   AF_LATIN_BLUE_MAX


enum
{
    AF_LATIN_BLUE_ACTIVE     = 1 << 0,
    AF_LATIN_BLUE_TOP        = 1 << 1,
    AF_LATIN_BLUE_ADJUSTMENT = 1 << 2,  /* used for scale adjustment */
    /* optimization              */
    AF_LATIN_BLUE_FLAG_MAX
};


typedef struct  AF_LatinBlueRec_
{
    AF_WidthRec  ref;
    AF_WidthRec  shoot;
    FT_UInt      flags;

} AF_LatinBlueRec, *AF_LatinBlue;


typedef struct  AF_LatinAxisRec_
{
    FT_Fixed         scale;
    FT_Pos           delta;

    FT_UInt          width_count;
    AF_WidthRec      widths[AF_LATIN_MAX_WIDTHS];
    FT_Pos           edge_distance_threshold;
    FT_Pos           standard_width;
    FT_Bool          extra_light;

    /* ignored for horizontal metrics */
    FT_Bool          control_overshoot;
    FT_UInt          blue_count;
    AF_LatinBlueRec  blues[AF_LATIN_BLUE_MAX];

    FT_Fixed         org_scale;
    FT_Pos           org_delta;

} AF_LatinAxisRec, *AF_LatinAxis;


typedef struct  AF_LatinMetricsRec_
{
    AF_ScriptMetricsRec  root;
    FT_UInt              units_per_em;
    AF_LatinAxisRec      axis[AF_DIMENSION_MAX];

} AF_LatinMetricsRec, *AF_LatinMetrics;


FT_LOCAL(FT_Error)
af_latin_metrics_init(AF_LatinMetrics  metrics,
                      FT_Face          face);

FT_LOCAL(void)
af_latin_metrics_scale(AF_LatinMetrics  metrics,
                       AF_Scaler        scaler);

FT_LOCAL(void)
af_latin_metrics_init_widths(AF_LatinMetrics  metrics,
                             FT_Face          face,
                             FT_ULong         charcode);


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****           L A T I N   G L Y P H   A N A L Y S I S             *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

enum
{
    AF_LATIN_HINTS_HORZ_SNAP   = 1 << 0, /* enable stem width snapping  */
    AF_LATIN_HINTS_VERT_SNAP   = 1 << 1, /* enable stem height snapping */
    AF_LATIN_HINTS_STEM_ADJUST = 1 << 2, /* enable stem width/height    */
    /* adjustment                  */
    AF_LATIN_HINTS_MONO        = 1 << 3  /* indicate monochrome         */
                                 /* rendering                   */
};


#define AF_LATIN_HINTS_DO_HORZ_SNAP( h )             \
    AF_HINTS_TEST_OTHER( h, AF_LATIN_HINTS_HORZ_SNAP )

#define AF_LATIN_HINTS_DO_VERT_SNAP( h )             \
    AF_HINTS_TEST_OTHER( h, AF_LATIN_HINTS_VERT_SNAP )

#define AF_LATIN_HINTS_DO_STEM_ADJUST( h )             \
    AF_HINTS_TEST_OTHER( h, AF_LATIN_HINTS_STEM_ADJUST )

#define AF_LATIN_HINTS_DO_MONO( h )             \
    AF_HINTS_TEST_OTHER( h, AF_LATIN_HINTS_MONO )


/*
 *  This shouldn't normally be exported.  However, other scripts might
 *  like to use this function as-is.
 */
FT_LOCAL(FT_Error)
af_latin_hints_compute_segments(AF_GlyphHints  hints,
                                AF_Dimension   dim);

/*
 *  This shouldn't normally be exported.  However, other scripts might
 *  want to use this function as-is.
 */
FT_LOCAL(void)
af_latin_hints_link_segments(AF_GlyphHints  hints,
                             AF_Dimension   dim);

/*
 *  This shouldn't normally be exported.  However, other scripts might
 *  want to use this function as-is.
 */
FT_LOCAL(FT_Error)
af_latin_hints_compute_edges(AF_GlyphHints  hints,
                             AF_Dimension   dim);

FT_LOCAL(FT_Error)
af_latin_hints_detect_features(AF_GlyphHints  hints,
                               AF_Dimension   dim);

/* */

FT_END_HEADER

#endif /* __AFLATIN_H__ */


/* END */
