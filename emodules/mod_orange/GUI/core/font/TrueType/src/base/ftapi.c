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
/*  ftapi.c                                                                */
/*                                                                         */
/*    The FreeType compatibility functions (body).                         */
/*                                                                         */
/*  Copyright 2002 by                                                      */
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
#include FT_LIST_H
#include FT_OUTLINE_H
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TABLES_H
#include FT_OUTLINE_H


/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/****                                                                 ****/
/****                                                                 ****/
/****                 C O M P A T I B I L I T Y                       ****/
/****                                                                 ****/
/****                                                                 ****/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/

/* backwards compatibility API */

FT_BASE_DEF(void)
FT_New_Memory_Stream(FT_Library  library,
                     FT_Byte    *base,
                     FT_ULong    size,
                     FT_Stream   stream)
{
    FT_UNUSED(library);
    FT_Stream_OpenMemory(stream, base, size);
}


FT_BASE_DEF(FT_Error)
FT_Seek_Stream(FT_Stream  stream,
               FT_ULong   pos)
{
    return FT_Stream_Seek(stream, pos);
}


FT_BASE_DEF(FT_Error)
FT_Skip_Stream(FT_Stream  stream,
               FT_Long    distance)
{
    return FT_Stream_Skip(stream, distance);
}


FT_BASE_DEF(FT_Error)
FT_Read_Stream(FT_Stream  stream,
               FT_Byte   *buffer,
               FT_ULong   count)
{
    return FT_Stream_Read(stream, buffer, count);
}


FT_BASE_DEF(FT_Error)
FT_Read_Stream_At(FT_Stream  stream,
                  FT_ULong   pos,
                  FT_Byte   *buffer,
                  FT_ULong   count)
{
    return FT_Stream_ReadAt(stream, pos, buffer, count);
}


FT_BASE_DEF(FT_Error)
FT_Extract_Frame(FT_Stream  stream,
                 FT_ULong   count,
                 FT_Byte  **pbytes)
{
    return FT_Stream_ExtractFrame(stream, count, pbytes);
}


FT_BASE_DEF(void)
FT_Release_Frame(FT_Stream  stream,
                 FT_Byte  **pbytes)
{
    FT_Stream_ReleaseFrame(stream, pbytes);
}

FT_BASE_DEF(FT_Error)
FT_Access_Frame(FT_Stream  stream,
                FT_ULong   count)
{
    return FT_Stream_EnterFrame(stream, count);
}


FT_BASE_DEF(void)
FT_Forget_Frame(FT_Stream  stream)
{
    FT_Stream_ExitFrame(stream);
}


/* END */
