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
/*  ftsizes.h                                                              */
/*                                                                         */
/*    FreeType size objects management (specification).                    */
/*                                                                         */
/*  Copyright 1996-2001, 2003, 2004, 2006 by                               */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
/*************************************************************************/
/*                                                                       */
/* Typical application would normally not need to use these functions.   */
/* However, they have been placed in a public API for the rare cases     */
/* where they are needed.                                                */
/*                                                                       */
/*************************************************************************/


#ifndef __FTSIZES_H__
#define __FTSIZES_H__


#include "ft2build.h"
#include FT_FREETYPE_H

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif


FT_BEGIN_HEADER


/*************************************************************************/
/*                                                                       */
/* <Section>                                                             */
/*    sizes_management                                                   */
/*                                                                       */
/* <Title>                                                               */
/*    Size Management                                                    */
/*                                                                       */
/* <Abstract>                                                            */
/*    Managing multiple sizes per face.                                  */
/*                                                                       */
/* <Description>                                                         */
/*    When creating a new face object (e.g., with @FT_New_Face), an      */
/*    @FT_Size object is automatically created and used to store all     */
/*    pixel-size dependent information, available in the `face->size'    */
/*    field.                                                             */
/*                                                                       */
/*    It is however possible to create more sizes for a given face,      */
/*    mostly in order to manage several character pixel sizes of the     */
/*    same font family and style.  See @FT_New_Size and @FT_Done_Size.   */
/*                                                                       */
/*    Note that @FT_Set_Pixel_Sizes and @FT_Set_Char_Size only           */
/*    modify the contents of the current `active' size; you thus need    */
/*    to use @FT_Activate_Size to change it.                             */
/*                                                                       */
/*    99% of applications won't need the functions provided here,        */
/*    especially if they use the caching sub-system, so be cautious      */
/*    when using these.                                                  */
/*                                                                       */
/*************************************************************************/


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_New_Size                                                        */
/*                                                                       */
/* <Description>                                                         */
/*    Create a new size object from a given face object.                 */
/*                                                                       */
/* <Input>                                                               */
/*    face :: A handle to a parent face object.                          */
/*                                                                       */
/* <Output>                                                              */
/*    asize :: A handle to a new size object.                            */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  0 means success.                             */
/*                                                                       */
/* <Note>                                                                */
/*    You need to call @FT_Activate_Size in order to select the new size */
/*    for upcoming calls to @FT_Set_Pixel_Sizes, @FT_Set_Char_Size,      */
/*    @FT_Load_Glyph, @FT_Load_Char, etc.                                */
/*                                                                       */
FT_EXPORT(FT_Error)
FT_New_Size(FT_Face   face,
            FT_Size  *size);


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Done_Size                                                       */
/*                                                                       */
/* <Description>                                                         */
/*    Discard a given size object.  Note that @FT_Done_Face              */
/*    automatically discards all size objects allocated with             */
/*    @FT_New_Size.                                                      */
/*                                                                       */
/* <Input>                                                               */
/*    size :: A handle to a target size object.                          */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  0 means success.                             */
/*                                                                       */
FT_EXPORT(FT_Error)
FT_Done_Size(FT_Size  size);


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Activate_Size                                                   */
/*                                                                       */
/* <Description>                                                         */
/*    Even though it is possible to create several size objects for a    */
/*    given face (see @FT_New_Size for details), functions like          */
/*    @FT_Load_Glyph or @FT_Load_Char only use the last-created one to   */
/*    determine the `current character pixel size'.                      */
/*                                                                       */
/*    This function can be used to `activate' a previously created size  */
/*    object.                                                            */
/*                                                                       */
/* <Input>                                                               */
/*    size :: A handle to a target size object.                          */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  0 means success.                             */
/*                                                                       */
/* <Note>                                                                */
/*    If `face' is the size's parent face object, this function changes  */
/*    the value of `face->size' to the input size handle.                */
/*                                                                       */
FT_EXPORT(FT_Error)
FT_Activate_Size(FT_Size  size);

/* */


FT_END_HEADER

#endif /* __FTSIZES_H__ */


/* END */
