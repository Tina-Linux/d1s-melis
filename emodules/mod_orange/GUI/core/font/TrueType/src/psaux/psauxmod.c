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
/*  psauxmod.c                                                             */
/*                                                                         */
/*    FreeType auxiliary PostScript module implementation (body).          */
/*                                                                         */
/*  Copyright 2000-2001, 2002, 2003, 2006 by                               */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#include <ft2build.h>
#include "psauxmod.h"
#include "psobjs.h"
#include "t1decode.h"
#include "t1cmap.h"

#ifndef T1_CONFIG_OPTION_NO_AFM
#include "afmparse.h"
#endif


FT_CALLBACK_TABLE_DEF
const PS_Table_FuncsRec  ps_table_funcs =
{
    ps_table_new,
    ps_table_done,
    ps_table_add,
    ps_table_release
};


FT_CALLBACK_TABLE_DEF
const PS_Parser_FuncsRec  ps_parser_funcs =
{
    ps_parser_init,
    ps_parser_done,
    ps_parser_skip_spaces,
    ps_parser_skip_PS_token,
    ps_parser_to_int,
    ps_parser_to_fixed,
    ps_parser_to_bytes,
    ps_parser_to_coord_array,
    ps_parser_to_fixed_array,
    ps_parser_to_token,
    ps_parser_to_token_array,
    ps_parser_load_field,
    ps_parser_load_field_table
};


FT_CALLBACK_TABLE_DEF
const T1_Builder_FuncsRec  t1_builder_funcs =
{
    t1_builder_init,
    t1_builder_done,
    t1_builder_check_points,
    t1_builder_add_point,
    t1_builder_add_point1,
    t1_builder_add_contour,
    t1_builder_start_point,
    t1_builder_close_contour
};


FT_CALLBACK_TABLE_DEF
const T1_Decoder_FuncsRec  t1_decoder_funcs =
{
    t1_decoder_init,
    t1_decoder_done,
    t1_decoder_parse_charstrings
};


#ifndef T1_CONFIG_OPTION_NO_AFM
FT_CALLBACK_TABLE_DEF
const AFM_Parser_FuncsRec  afm_parser_funcs =
{
    afm_parser_init,
    afm_parser_done,
    afm_parser_parse
};
#endif


FT_CALLBACK_TABLE_DEF
const T1_CMap_ClassesRec  t1_cmap_classes =
{
    &t1_cmap_standard_class_rec,
    &t1_cmap_expert_class_rec,
    &t1_cmap_custom_class_rec,
    &t1_cmap_unicode_class_rec
};


static
const PSAux_Interface  psaux_interface =
{
    &ps_table_funcs,
    &ps_parser_funcs,
    &t1_builder_funcs,
    &t1_decoder_funcs,
    t1_decrypt,

    (const T1_CMap_ClassesRec *) &t1_cmap_classes,

#ifndef T1_CONFIG_OPTION_NO_AFM
    &afm_parser_funcs,
#else
    0,
#endif
};


FT_CALLBACK_TABLE_DEF
const FT_Module_Class  psaux_module_class =
{
    0,
    sizeof(FT_ModuleRec),
    "psaux",
    0x20000L,
    0x20000L,

    &psaux_interface,  /* module-specific interface */

    (FT_Module_Constructor)0,
    (FT_Module_Destructor) 0,
    (FT_Module_Requester)  0
};


/* END */
