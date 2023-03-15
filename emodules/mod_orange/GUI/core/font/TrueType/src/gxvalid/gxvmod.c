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
/*  gxvmod.c                                                               */
/*                                                                         */
/*    FreeType's TrueTypeGX/AAT validation module implementation (body).   */
/*                                                                         */
/*  Copyright 2004, 2005, 2006                                             */
/*  by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,                      */
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
#include <ft2build.h>
#include FT_TRUETYPE_TABLES_H
#include FT_TRUETYPE_TAGS_H
#include FT_GX_VALIDATE_H
#include FT_INTERNAL_OBJECTS_H
#include FT_SERVICE_GX_VALIDATE_H

#include "gxvmod.h"
#include "gxvalid.h"
#include "gxvcommn.h"


/*************************************************************************/
/*                                                                       */
/* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
/* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
/* messages during execution.                                            */
/*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_gxvmodule


static FT_Error gxv_load_table(FT_Face             face,
                               FT_Tag              tag,
                               FT_Byte *volatile  *table,
                               FT_ULong           *table_len)
{
    FT_Error   error;
    FT_Memory  memory = FT_FACE_MEMORY(face);
    error = FT_Load_Sfnt_Table(face, tag, 0, NULL, table_len);

    if (error == GXV_Err_Table_Missing)
    {
        return GXV_Err_Ok;
    }

    if (error)
    {
        goto Exit;
    }

    if (FT_ALLOC(*table, *table_len))
    {
        goto Exit;
    }

    error = FT_Load_Sfnt_Table(face, tag, 0, *table, table_len);
Exit:
    return error;
}


#define GXV_TABLE_DECL( _sfnt )                     \
    FT_Byte* volatile  _sfnt          = NULL; \
    FT_ULong            len_ ## _sfnt = 0

#define GXV_TABLE_LOAD( _sfnt )                                     \
    if ( ( FT_VALIDATE_ ## _sfnt ## _INDEX < table_count ) && \
         ( gx_flags & FT_VALIDATE_ ## _sfnt )            )    \
    {                                                         \
        error = gxv_load_table( face, TTAG_ ## _sfnt,           \
                                &_sfnt, &len_ ## _sfnt );       \
        if ( error )                                            \
            goto Exit;                                            \
    }

#define GXV_TABLE_VALIDATE( _sfnt )                                  \
    if ( _sfnt )                                               \
    {                                                          \
        ft_validator_init( &valid, _sfnt, _sfnt + len_ ## _sfnt, \
                           FT_VALIDATE_DEFAULT );                \
        if ( ft_setjmp( valid.jump_buffer ) == 0 )               \
            gxv_ ## _sfnt ## _validate( _sfnt, face, &valid );     \
        error = valid.error;                                     \
        if ( error )                                             \
            goto Exit;                                             \
    }

#define GXV_TABLE_SET( _sfnt )                                        \
    if ( FT_VALIDATE_ ## _sfnt ## _INDEX < table_count )        \
        tables[FT_VALIDATE_ ## _sfnt ## _INDEX] = (FT_Bytes)_sfnt


static FT_Error gxv_validate(FT_Face   face,
                             FT_UInt   gx_flags,
                             FT_Bytes  tables[FT_VALIDATE_GX_LENGTH],
                             FT_UInt   table_count)
{
    FT_Memory volatile        memory = FT_FACE_MEMORY(face);
    FT_Error                  error = GXV_Err_Ok;
    FT_ValidatorRec volatile  valid;
    FT_UInt  i;
    GXV_TABLE_DECL(feat);
    GXV_TABLE_DECL(bsln);
    GXV_TABLE_DECL(trak);
    GXV_TABLE_DECL(just);
    GXV_TABLE_DECL(mort);
    GXV_TABLE_DECL(morx);
    GXV_TABLE_DECL(kern);
    GXV_TABLE_DECL(opbd);
    GXV_TABLE_DECL(prop);
    GXV_TABLE_DECL(lcar);

    for (i = 0; i < table_count; i++)
    {
        tables[i] = 0;
    }

    /* load tables */
    GXV_TABLE_LOAD(feat);
    GXV_TABLE_LOAD(bsln);
    GXV_TABLE_LOAD(trak);
    GXV_TABLE_LOAD(just);
    GXV_TABLE_LOAD(mort);
    GXV_TABLE_LOAD(morx);
    GXV_TABLE_LOAD(kern);
    GXV_TABLE_LOAD(opbd);
    GXV_TABLE_LOAD(prop);
    GXV_TABLE_LOAD(lcar);
    /* validate tables */
    GXV_TABLE_VALIDATE(feat);
    GXV_TABLE_VALIDATE(bsln);
    GXV_TABLE_VALIDATE(trak);
    GXV_TABLE_VALIDATE(just);
    GXV_TABLE_VALIDATE(mort);
    GXV_TABLE_VALIDATE(morx);
    GXV_TABLE_VALIDATE(kern);
    GXV_TABLE_VALIDATE(opbd);
    GXV_TABLE_VALIDATE(prop);
    GXV_TABLE_VALIDATE(lcar);
    /* Set results */
    GXV_TABLE_SET(feat);
    GXV_TABLE_SET(mort);
    GXV_TABLE_SET(morx);
    GXV_TABLE_SET(bsln);
    GXV_TABLE_SET(just);
    GXV_TABLE_SET(kern);
    GXV_TABLE_SET(opbd);
    GXV_TABLE_SET(trak);
    GXV_TABLE_SET(prop);
    GXV_TABLE_SET(lcar);
Exit:

    if (error)
    {
        FT_FREE(feat);
        FT_FREE(bsln);
        FT_FREE(trak);
        FT_FREE(just);
        FT_FREE(mort);
        FT_FREE(morx);
        FT_FREE(kern);
        FT_FREE(opbd);
        FT_FREE(prop);
        FT_FREE(lcar);
    }

    return error;
}


static FT_Error classic_kern_validate(FT_Face    face,
                                      FT_UInt    ckern_flags,
                                      FT_Bytes  *ckern_table)
{
    FT_Memory volatile        memory = FT_FACE_MEMORY(face);
    FT_Byte *volatile         ckern     = NULL;
    FT_ULong                  len_ckern = 0;
    /* without volatile on `error' GCC 4.1.1. emits:                         */
    /*  warning: variable 'error' might be clobbered by 'longjmp' or 'vfork' */
    /* this warning seems spurious but ---                                   */
    FT_Error volatile         error = GXV_Err_Ok;
    FT_ValidatorRec volatile  valid;
    *ckern_table = NULL;
    error = gxv_load_table(face, TTAG_kern, &ckern, &len_ckern);

    if (error)
    {
        goto Exit;
    }

    if (ckern)
    {
        ft_validator_init(&valid, ckern, ckern + len_ckern,
                          FT_VALIDATE_DEFAULT);

        if (ft_setjmp(valid.jump_buffer) == 0)
            gxv_kern_validate_classic(ckern, face,
                                      ckern_flags & FT_VALIDATE_CKERN, &valid);

        error = valid.error;

        if (error)
        {
            goto Exit;
        }
    }

    *ckern_table = ckern;
Exit:

    if (error)
    {
        FT_FREE(ckern);
    }

    return error;
}


static
const FT_Service_GXvalidateRec  gxvalid_interface =
{
    gxv_validate
};


static
const FT_Service_CKERNvalidateRec  ckernvalid_interface =
{
    classic_kern_validate
};


static
const FT_ServiceDescRec  gxvalid_services[] =
{
    { FT_SERVICE_ID_GX_VALIDATE,          &gxvalid_interface },
    { FT_SERVICE_ID_CLASSICKERN_VALIDATE, &ckernvalid_interface },
    { NULL, NULL }
};


static FT_Pointer gxvalid_get_service(FT_Module    module,
                                      const char  *service_id)
{
    FT_UNUSED(module);
    return ft_service_list_lookup(gxvalid_services, service_id);
}


FT_CALLBACK_TABLE_DEF
const FT_Module_Class  gxv_module_class =
{
    0,
    sizeof(FT_ModuleRec),
    "gxvalid",
    0x10000L,
    0x20000L,

    0,              /* module-specific interface */

    (FT_Module_Constructor)0,
    (FT_Module_Destructor) 0,
    (FT_Module_Requester)  gxvalid_get_service
};


/* END */
