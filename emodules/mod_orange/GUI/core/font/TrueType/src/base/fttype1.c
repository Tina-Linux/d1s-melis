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
/*  fttype1.c                                                              */
/*                                                                         */
/*    FreeType utility file for PS names support (body).                   */
/*                                                                         */
/*  Copyright 2002, 2003, 2004 by                                          */
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
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_SERVICE_H
#include FT_SERVICE_POSTSCRIPT_INFO_H


/* documentation is in t1tables.h */

FT_EXPORT_DEF(FT_Error)
FT_Get_PS_Font_Info(FT_Face          face,
                    PS_FontInfoRec  *afont_info)
{
    FT_Error  error = FT_Err_Invalid_Argument;

    if (face)
    {
        FT_Service_PsInfo  service = NULL;
        FT_FACE_FIND_SERVICE(face, service, POSTSCRIPT_INFO);

        if (service && service->ps_get_font_info)
        {
            error = service->ps_get_font_info(face, afont_info);
        }
    }

    return error;
}


/* documentation is in t1tables.h */

FT_EXPORT_DEF(FT_Int)
FT_Has_PS_Glyph_Names(FT_Face  face)
{
    FT_Int             result  = 0;
    FT_Service_PsInfo  service = NULL;

    if (face)
    {
        FT_FACE_FIND_SERVICE(face, service, POSTSCRIPT_INFO);

        if (service && service->ps_has_glyph_names)
        {
            result = service->ps_has_glyph_names(face);
        }
    }

    return result;
}


/* documentation is in t1tables.h */

FT_EXPORT_DEF(FT_Error)
FT_Get_PS_Font_Private(FT_Face         face,
                       PS_PrivateRec  *afont_private)
{
    FT_Error  error = FT_Err_Invalid_Argument;

    if (face)
    {
        FT_Service_PsInfo  service = NULL;
        FT_FACE_FIND_SERVICE(face, service, POSTSCRIPT_INFO);

        if (service && service->ps_get_font_private)
        {
            error = service->ps_get_font_private(face, afont_private);
        }
    }

    return error;
}


/* END */
