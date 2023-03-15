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
/*  ftgxval.c                                                              */
/*                                                                         */
/*    FreeType API for validating TrueTyepGX/AAT tables (body).            */
/*                                                                         */
/*  Copyright 2004, 2005, 2006 by                                          */
/*  Masatake YAMATO, Redhat K.K,                                           */
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
#include "ft2build.h"
#include FT_INTERNAL_OBJECTS_H
#include FT_SERVICE_GX_VALIDATE_H


/* documentation is in ftgxval.h */

FT_EXPORT_DEF(FT_Error)
FT_TrueTypeGX_Validate(FT_Face   face,
                       FT_UInt   validation_flags,
                       FT_Bytes  tables[FT_VALIDATE_GX_LENGTH],
                       FT_UInt   table_length)
{
    FT_Service_GXvalidate  service;
    FT_Error               error;

    if (!face)
    {
        error = FT_Err_Invalid_Face_Handle;
        goto Exit;
    }

    if (tables == NULL)
    {
        error = FT_Err_Invalid_Argument;
        goto Exit;
    }

    FT_FACE_FIND_GLOBAL_SERVICE(face, service, GX_VALIDATE);

    if (service)
        error = service->validate(face,
                                  validation_flags,
                                  tables,
                                  table_length);
    else
    {
        error = FT_Err_Unimplemented_Feature;
    }

Exit:
    return error;
}


FT_EXPORT_DEF(void)
FT_TrueTypeGX_Free(FT_Face   face,
                   FT_Bytes  table)
{
    FT_Memory  memory = FT_FACE_MEMORY(face);
    FT_FREE(table);
}


FT_EXPORT_DEF(FT_Error)
FT_ClassicKern_Validate(FT_Face    face,
                        FT_UInt    validation_flags,
                        FT_Bytes  *ckern_table)
{
    FT_Service_CKERNvalidate  service;
    FT_Error                  error;

    if (!face)
    {
        error = FT_Err_Invalid_Face_Handle;
        goto Exit;
    }

    if (ckern_table == NULL)
    {
        error = FT_Err_Invalid_Argument;
        goto Exit;
    }

    FT_FACE_FIND_GLOBAL_SERVICE(face, service, CLASSICKERN_VALIDATE);

    if (service)
        error = service->validate(face,
                                  validation_flags,
                                  ckern_table);
    else
    {
        error = FT_Err_Unimplemented_Feature;
    }

Exit:
    return error;
}


FT_EXPORT_DEF(void)
FT_ClassicKern_Free(FT_Face   face,
                    FT_Bytes  table)
{
    FT_Memory  memory = FT_FACE_MEMORY(face);
    FT_FREE(table);
}


/* END */
