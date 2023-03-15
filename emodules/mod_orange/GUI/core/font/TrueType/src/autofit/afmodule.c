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
/*  afmodule.c                                                             */
/*                                                                         */
/*    Auto-fitter module implementation (body).                            */
/*                                                                         */
/*  Copyright 2003, 2004, 2005, 2006 by                                    */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#include "afmodule.h"
#include "afloader.h"

#ifdef AF_DEBUG
int    _af_debug;
int    _af_debug_disable_horz_hints;
int    _af_debug_disable_vert_hints;
int    _af_debug_disable_blue_hints;
void  *_af_debug_hints;
#endif

#include FT_INTERNAL_OBJECTS_H


typedef struct  FT_AutofitterRec_
{
    FT_ModuleRec  root;
    AF_LoaderRec  loader[1];

} FT_AutofitterRec, *FT_Autofitter;


FT_CALLBACK_DEF(FT_Error)
af_autofitter_init(FT_Autofitter  module)
{
    return af_loader_init(module->loader, module->root.library->memory);
}


FT_CALLBACK_DEF(void)
af_autofitter_done(FT_Autofitter  module)
{
    af_loader_done(module->loader);
}


FT_CALLBACK_DEF(FT_Error)
af_autofitter_load_glyph(FT_Autofitter  module,
                         FT_GlyphSlot   slot,
                         FT_Size        size,
                         FT_UInt        glyph_index,
                         FT_Int32       load_flags)
{
    FT_UNUSED(size);
    return af_loader_load_glyph(module->loader, slot->face,
                                glyph_index, load_flags);
}


FT_CALLBACK_TABLE_DEF
const FT_AutoHinter_ServiceRec  af_autofitter_service =
{
    NULL,
    NULL,
    NULL,
    (FT_AutoHinter_GlyphLoadFunc)af_autofitter_load_glyph
};


FT_CALLBACK_TABLE_DEF
const FT_Module_Class  autofit_module_class =
{
    FT_MODULE_HINTER,
    sizeof(FT_AutofitterRec),

    "autofitter",
    0x10000L,   /* version 1.0 of the autofitter  */
    0x20000L,   /* requires FreeType 2.0 or above */

    (const void *) &af_autofitter_service,

    (FT_Module_Constructor)af_autofitter_init,
    (FT_Module_Destructor) af_autofitter_done,
    (FT_Module_Requester)  NULL
};


/* END */
