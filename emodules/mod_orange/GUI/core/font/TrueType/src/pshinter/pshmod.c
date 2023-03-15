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
/*  pshmod.c                                                               */
/*                                                                         */
/*    FreeType PostScript hinter module implementation (body).             */
/*                                                                         */
/*  Copyright 2001, 2002, 2007 by                                          */
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
#include FT_INTERNAL_OBJECTS_H
#include "pshrec.h"
#include "pshalgo.h"


/* the Postscript Hinter module structure */
typedef struct  PS_Hinter_Module_Rec_
{
    FT_ModuleRec          root;
    PS_HintsRec           ps_hints;

    PSH_Globals_FuncsRec  globals_funcs;
    T1_Hints_FuncsRec     t1_funcs;
    T2_Hints_FuncsRec     t2_funcs;

} PS_Hinter_ModuleRec, *PS_Hinter_Module;


/* finalize module */
FT_CALLBACK_DEF(void)
ps_hinter_done(PS_Hinter_Module  module)
{
    module->t1_funcs.hints = NULL;
    module->t2_funcs.hints = NULL;
    ps_hints_done(&module->ps_hints);
}


/* initialize module, create hints recorder and the interface */
FT_CALLBACK_DEF(FT_Error)
ps_hinter_init(PS_Hinter_Module  module)
{
    FT_Memory  memory = module->root.memory;
    void      *ph     = &module->ps_hints;
    ps_hints_init(&module->ps_hints, memory);
    psh_globals_funcs_init(&module->globals_funcs);
    t1_hints_funcs_init(&module->t1_funcs);
    module->t1_funcs.hints = (T1_Hints)ph;
    t2_hints_funcs_init(&module->t2_funcs);
    module->t2_funcs.hints = (T2_Hints)ph;
    return 0;
}


/* returns global hints interface */
FT_CALLBACK_DEF(PSH_Globals_Funcs)
pshinter_get_globals_funcs(FT_Module  module)
{
    return &((PS_Hinter_Module)module)->globals_funcs;
}


/* return Type 1 hints interface */
FT_CALLBACK_DEF(T1_Hints_Funcs)
pshinter_get_t1_funcs(FT_Module  module)
{
    return &((PS_Hinter_Module)module)->t1_funcs;
}


/* return Type 2 hints interface */
FT_CALLBACK_DEF(T2_Hints_Funcs)
pshinter_get_t2_funcs(FT_Module  module)
{
    return &((PS_Hinter_Module)module)->t2_funcs;
}


static
const PSHinter_Interface  pshinter_interface =
{
    pshinter_get_globals_funcs,
    pshinter_get_t1_funcs,
    pshinter_get_t2_funcs
};


FT_CALLBACK_TABLE_DEF
const FT_Module_Class  pshinter_module_class =
{
    0,
    sizeof(PS_Hinter_ModuleRec),
    "pshinter",
    0x10000L,
    0x20000L,

    &pshinter_interface,            /* module-specific interface */

    (FT_Module_Constructor)ps_hinter_init,
    (FT_Module_Destructor) ps_hinter_done,
    (FT_Module_Requester)  0        /* no additional interface for now */
};


/* END */
