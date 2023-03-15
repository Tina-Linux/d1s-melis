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
/*  ftbitmap.h                                                             */
/*                                                                         */
/*    FreeType utility functions for converting 1bpp, 2bpp, 4bpp, and 8bpp */
/*    bitmaps into 8bpp format (specification).                            */
/*                                                                         */
/*  Copyright 2004, 2005, 2006 by                                          */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __FTBITMAP_H__
#define __FTBITMAP_H__


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
/*    bitmap_handling                                                    */
/*                                                                       */
/* <Title>                                                               */
/*    Bitmap Handling                                                    */
/*                                                                       */
/* <Abstract>                                                            */
/*    Handling FT_Bitmap objects.                                        */
/*                                                                       */
/* <Description>                                                         */
/*    This section contains functions for converting FT_Bitmap objects.  */
/*                                                                       */
/*************************************************************************/


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Bitmap_New                                                      */
/*                                                                       */
/* <Description>                                                         */
/*    Initialize a pointer to an @FT_Bitmap structure.                   */
/*                                                                       */
/* <InOut>                                                               */
/*    abitmap :: A pointer to the bitmap structure.                      */
/*                                                                       */
FT_EXPORT(void)
FT_Bitmap_New(FT_Bitmap  *abitmap);


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Bitmap_Copy                                                     */
/*                                                                       */
/* <Description>                                                         */
/*    Copies an bitmap into another one.                                 */
/*                                                                       */
/* <Input>                                                               */
/*    library :: A handle to a library object.                           */
/*                                                                       */
/*    source  :: A handle to the source bitmap.                          */
/*                                                                       */
/* <Output>                                                              */
/*    target  :: A handle to the target bitmap.                          */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  0 means success.                             */
/*                                                                       */
FT_EXPORT(FT_Error)
FT_Bitmap_Copy(FT_Library        library,
               const FT_Bitmap  *source,
               FT_Bitmap        *target);


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Bitmap_Embolden                                                 */
/*                                                                       */
/* <Description>                                                         */
/*    Embolden a bitmap.  The new bitmap will be about `xStrength'       */
/*    pixels wider and `yStrength' pixels higher.  The left and bottom   */
/*    borders are kept unchanged.                                        */
/*                                                                       */
/* <Input>                                                               */
/*    library   :: A handle to a library object.                         */
/*                                                                       */
/*    xStrength :: How strong the glyph is emboldened horizontally.      */
/*                 Expressed in 26.6 pixel format.                       */
/*                                                                       */
/*    yStrength :: How strong the glyph is emboldened vertically.        */
/*                 Expressed in 26.6 pixel format.                       */
/*                                                                       */
/* <InOut>                                                               */
/*    bitmap    :: A handle to the target bitmap.                        */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  0 means success.                             */
/*                                                                       */
/* <Note>                                                                */
/*    The current implementation restricts `xStrength' to be less than   */
/*    or equal to 8 if bitmap is of pixel_mode @FT_PIXEL_MODE_MONO.      */
/*                                                                       */
/*    If you want to embolden the bitmap owned by a @FT_GlyphSlotRec,    */
/*    you should call `FT_GlyphSlot_Own_Bitmap' on the slot first.       */
/*                                                                       */
FT_EXPORT(FT_Error)
FT_Bitmap_Embolden(FT_Library  library,
                   FT_Bitmap  *bitmap,
                   FT_Pos      xStrength,
                   FT_Pos      yStrength);


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Bitmap_Convert                                                  */
/*                                                                       */
/* <Description>                                                         */
/*    Convert a bitmap object with depth 1bpp, 2bpp, 4bpp, or 8bpp to a  */
/*    bitmap object with depth 8bpp, making the number of used bytes per */
/*    line (a.k.a. the `pitch') a multiple of `alignment'.               */
/*                                                                       */
/* <Input>                                                               */
/*    library   :: A handle to a library object.                         */
/*                                                                       */
/*    source    :: The source bitmap.                                    */
/*                                                                       */
/*    alignment :: The pitch of the bitmap is a multiple of this         */
/*                 parameter.  Common values are 1, 2, or 4.             */
/*                                                                       */
/* <Output>                                                              */
/*    target    :: The target bitmap.                                    */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  0 means success.                             */
/*                                                                       */
/* <Note>                                                                */
/*    It is possible to call @FT_Bitmap_Convert multiple times without   */
/*    calling @FT_Bitmap_Done (the memory is simply reallocated).        */
/*                                                                       */
/*    Use @FT_Bitmap_Done to finally remove the bitmap object.           */
/*                                                                       */
/*    The `library' argument is taken to have access to FreeType's       */
/*    memory handling functions.                                         */
/*                                                                       */
FT_EXPORT(FT_Error)
FT_Bitmap_Convert(FT_Library        library,
                  const FT_Bitmap  *source,
                  FT_Bitmap        *target,
                  FT_Int            alignment);


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Bitmap_Done                                                     */
/*                                                                       */
/* <Description>                                                         */
/*    Destroy a bitmap object created with @FT_Bitmap_New.               */
/*                                                                       */
/* <Input>                                                               */
/*    library :: A handle to a library object.                           */
/*                                                                       */
/*    bitmap  :: The bitmap object to be freed.                          */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  0 means success.                             */
/*                                                                       */
/* <Note>                                                                */
/*    The `library' argument is taken to have access to FreeType's       */
/*    memory handling functions.                                         */
/*                                                                       */
FT_EXPORT(FT_Error)
FT_Bitmap_Done(FT_Library  library,
               FT_Bitmap  *bitmap);


/* */


FT_END_HEADER

#endif /* __FTBITMAP_H__ */


/* END */
