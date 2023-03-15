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
/*  ftmm.c                                                                 */
/*                                                                         */
/*    Multiple Master font support (body).                                 */
/*                                                                         */
/*  Copyright 1996-2001, 2003, 2004 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#include "ft2build.h"
#include FT_MULTIPLE_MASTERS_H
#include FT_INTERNAL_OBJECTS_H
#include FT_SERVICE_MULTIPLE_MASTERS_H


/*************************************************************************/
/*                                                                       */
/* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
/* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
/* messages during execution.                                            */
/*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_mm


static FT_Error ft_face_get_mm_service(FT_Face                   face,
                                       FT_Service_MultiMasters  *aservice)
{
    FT_Error  error;
    *aservice = NULL;

    if (!face)
    {
        return FT_Err_Invalid_Face_Handle;
    }

    error = FT_Err_Invalid_Argument;

    if (FT_HAS_MULTIPLE_MASTERS(face))
    {
        FT_FACE_LOOKUP_SERVICE(face,
                               *aservice,
                               MULTI_MASTERS);

        if (aservice)
        {
            error = FT_Err_Ok;
        }
    }

    return error;
}


/* documentation is in ftmm.h */

FT_EXPORT_DEF(FT_Error)
FT_Get_Multi_Master(FT_Face           face,
                    FT_Multi_Master  *amaster)
{
    FT_Error                 error;
    FT_Service_MultiMasters  service;
    error = ft_face_get_mm_service(face, &service);

    if (!error)
    {
        error = FT_Err_Invalid_Argument;

        if (service->get_mm)
        {
            error = service->get_mm(face, amaster);
        }
    }

    return error;
}


/* documentation is in ftmm.h */

FT_EXPORT_DEF(FT_Error)
FT_Get_MM_Var(FT_Face      face,
              FT_MM_Var **amaster)
{
    FT_Error                 error;
    FT_Service_MultiMasters  service;
    error = ft_face_get_mm_service(face, &service);

    if (!error)
    {
        error = FT_Err_Invalid_Argument;

        if (service->get_mm_var)
        {
            error = service->get_mm_var(face, amaster);
        }
    }

    return error;
}


/* documentation is in ftmm.h */

FT_EXPORT_DEF(FT_Error)
FT_Set_MM_Design_Coordinates(FT_Face   face,
                             FT_UInt   num_coords,
                             FT_Long  *coords)
{
    FT_Error                 error;
    FT_Service_MultiMasters  service;
    error = ft_face_get_mm_service(face, &service);

    if (!error)
    {
        error = FT_Err_Invalid_Argument;

        if (service->set_mm_design)
        {
            error = service->set_mm_design(face, num_coords, coords);
        }
    }

    return error;
}


/* documentation is in ftmm.h */

FT_EXPORT_DEF(FT_Error)
FT_Set_Var_Design_Coordinates(FT_Face    face,
                              FT_UInt    num_coords,
                              FT_Fixed  *coords)
{
    FT_Error                 error;
    FT_Service_MultiMasters  service;
    error = ft_face_get_mm_service(face, &service);

    if (!error)
    {
        error = FT_Err_Invalid_Argument;

        if (service->set_var_design)
        {
            error = service->set_var_design(face, num_coords, coords);
        }
    }

    return error;
}


/* documentation is in ftmm.h */

FT_EXPORT_DEF(FT_Error)
FT_Set_MM_Blend_Coordinates(FT_Face    face,
                            FT_UInt    num_coords,
                            FT_Fixed  *coords)
{
    FT_Error                 error;
    FT_Service_MultiMasters  service;
    error = ft_face_get_mm_service(face, &service);

    if (!error)
    {
        error = FT_Err_Invalid_Argument;

        if (service->set_mm_blend)
        {
            error = service->set_mm_blend(face, num_coords, coords);
        }
    }

    return error;
}


/* documentation is in ftmm.h */

/* This is exactly the same as the previous function.  It exists for */
/* orthogonality.                                                    */

FT_EXPORT_DEF(FT_Error)
FT_Set_Var_Blend_Coordinates(FT_Face    face,
                             FT_UInt    num_coords,
                             FT_Fixed  *coords)
{
    FT_Error                 error;
    FT_Service_MultiMasters  service;
    error = ft_face_get_mm_service(face, &service);

    if (!error)
    {
        error = FT_Err_Invalid_Argument;

        if (service->set_mm_blend)
        {
            error = service->set_mm_blend(face, num_coords, coords);
        }
    }

    return error;
}


/* END */
