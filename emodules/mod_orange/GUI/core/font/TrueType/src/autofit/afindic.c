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
/*  afindic.c                                                              */
/*                                                                         */
/*    Auto-fitter hinting routines for Indic scripts (body).               */
/*                                                                         */
/*  Copyright 2007 by                                                      */
/*  Rahul Bhalerao <rahul.bhalerao@redhat.com>, <b.rahul.pm@gmail.com>.    */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#include "aftypes.h"
#include "aflatin.h"


#ifdef AF_CONFIG_OPTION_INDIC

#include "afindic.h"
#include "aferrors.h"
#include "afcjk.h"


#ifdef AF_USE_WARPER
#include "afwarp.h"
#endif


static FT_Error af_indic_metrics_init(AF_LatinMetrics  metrics,
                                      FT_Face          face)
{
    /* use CJK routines */
    return af_cjk_metrics_init(metrics, face);
}


static void af_indic_metrics_scale(AF_LatinMetrics  metrics,
                                   AF_Scaler        scaler)
{
    /* use CJK routines */
    af_cjk_metrics_scale(metrics, scaler);
}


static FT_Error af_indic_hints_init(AF_GlyphHints    hints,
                                    AF_LatinMetrics  metrics)
{
    /* use CJK routines */
    return af_cjk_hints_init(hints, metrics);
}


static FT_Error af_indic_hints_apply(AF_GlyphHints    hints,
                                     FT_Outline      *outline,
                                     AF_LatinMetrics  metrics)
{
    /* use CJK routines */
    return af_cjk_hints_apply(hints, outline, metrics);
}


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                I N D I C   S C R I P T   C L A S S            *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/


static const AF_Script_UniRangeRec  af_indic_uniranges[] =
{
#if 0
    { 0x0100,  0xFFFF },  /* why this? */
#endif
    { 0x0900, 0x0DFF},    /* Indic Range */
    { 0,       0 }
};


FT_CALLBACK_TABLE_DEF const AF_ScriptClassRec
af_indic_script_class =
{
    AF_SCRIPT_INDIC,
    af_indic_uniranges,

    sizeof(AF_LatinMetricsRec),

    (AF_Script_InitMetricsFunc) af_indic_metrics_init,
    (AF_Script_ScaleMetricsFunc)af_indic_metrics_scale,
    (AF_Script_DoneMetricsFunc) NULL,

    (AF_Script_InitHintsFunc)   af_indic_hints_init,
    (AF_Script_ApplyHintsFunc)  af_indic_hints_apply
};

#else /* !AF_CONFIG_OPTION_INDIC */

static const AF_Script_UniRangeRec  af_indic_uniranges[] =
{
    { 0, 0 }
};


FT_CALLBACK_TABLE_DEF const AF_ScriptClassRec
af_indic_script_class =
{
    AF_SCRIPT_INDIC,
    af_indic_uniranges,

    sizeof(AF_LatinMetricsRec),

    (AF_Script_InitMetricsFunc) NULL,
    (AF_Script_ScaleMetricsFunc)NULL,
    (AF_Script_DoneMetricsFunc) NULL,

    (AF_Script_InitHintsFunc)   NULL,
    (AF_Script_ApplyHintsFunc)  NULL
};

#endif /* !AF_CONFIG_OPTION_INDIC */


/* END */
