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
/*  ftotval.h                                                              */
/*                                                                         */
/*    FreeType API for validating OpenType tables (specification).         */
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

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/* Warning: This module might be moved to a different library in the       */
/*          future to avoid a tight dependency between FreeType and the    */
/*          OpenType specification.                                        */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
#ifndef __FTOTVAL_H__
#define __FTOTVAL_H__

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
/*    ot_validation                                                      */
/*                                                                       */
/* <Title>                                                               */
/*    OpenType Validation                                                */
/*                                                                       */
/* <Abstract>                                                            */
/*    An API to validate OpenType tables.                                */
/*                                                                       */
/* <Description>                                                         */
/*    This section contains the declaration of functions to validate     */
/*    some OpenType tables (BASE, GDEF, GPOS, GSUB, JSTF).               */
/*                                                                       */
/*************************************************************************/


/**********************************************************************
 *
 * @enum:
 *    FT_VALIDATE_OTXXX
 *
 * @description:
 *    A list of bit-field constants used with @FT_OpenType_Validate to
 *    indicate which OpenType tables should be validated.
 *
 * @values:
 *    FT_VALIDATE_BASE ::
 *      Validate BASE table.
 *
 *    FT_VALIDATE_GDEF ::
 *      Validate GDEF table.
 *
 *    FT_VALIDATE_GPOS ::
 *      Validate GPOS table.
 *
 *    FT_VALIDATE_GSUB ::
 *      Validate GSUB table.
 *
 *    FT_VALIDATE_JSTF ::
 *      Validate JSTF table.
 *
 *    FT_VALIDATE_OT ::
 *      Validate all OpenType tables (BASE, GDEF, GPOS, GSUB, JSTF).
 *
 */
#define FT_VALIDATE_BASE  0x0100
#define FT_VALIDATE_GDEF  0x0200
#define FT_VALIDATE_GPOS  0x0400
#define FT_VALIDATE_GSUB  0x0800
#define FT_VALIDATE_JSTF  0x1000

#define FT_VALIDATE_OT  FT_VALIDATE_BASE | \
    FT_VALIDATE_GDEF | \
    FT_VALIDATE_GPOS | \
    FT_VALIDATE_GSUB | \
    FT_VALIDATE_JSTF

/* */

/**********************************************************************
 *
 * @function:
 *    FT_OpenType_Validate
 *
 * @description:
 *    Validate various OpenType tables to assure that all offsets and
 *    indices are valid.  The idea is that a higher-level library which
 *    actually does the text layout can access those tables without
 *    error checking (which can be quite time consuming).
 *
 * @input:
 *    face ::
 *       A handle to the input face.
 *
 *    validation_flags ::
 *       A bit field which specifies the tables to be validated.  See
 *       @FT_VALIDATE_OTXXX for possible values.
 *
 * @output:
 *    BASE_table ::
 *       A pointer to the BASE table.
 *
 *    GDEF_table ::
 *       A pointer to the GDEF table.
 *
 *    GPOS_table ::
 *       A pointer to the GPOS table.
 *
 *    GSUB_table ::
 *       A pointer to the GSUB table.
 *
 *    JSTF_table ::
 *       A pointer to the JSTF table.
 *
 * @return:
 *   FreeType error code.  0 means success.
 *
 * @note:
 *   This function only works with OpenType fonts, returning an error
 *   otherwise.
 *
 *   After use, the application should deallocate the five tables with
 *   @FT_OpenType_Free.  A NULL value indicates that the table either
 *   doesn't exist in the font, or the application hasn't asked for
 *   validation.
 */
FT_EXPORT(FT_Error)
FT_OpenType_Validate(FT_Face    face,
                     FT_UInt    validation_flags,
                     FT_Bytes  *BASE_table,
                     FT_Bytes  *GDEF_table,
                     FT_Bytes  *GPOS_table,
                     FT_Bytes  *GSUB_table,
                     FT_Bytes  *JSTF_table);

/* */

/**********************************************************************
 *
 * @function:
 *    FT_OpenType_Free
 *
 * @description:
 *    Free the buffer allocated by OpenType validator.
 *
 * @input:
 *    face ::
 *       A handle to the input face.
 *
 *    table ::
 *       The pointer to the buffer that is allocated by
 *       @FT_OpenType_Validate.
 *
 * @note:
 *   This function must be used to free the buffer allocated by
 *   @FT_OpenType_Validate only.
 */
FT_EXPORT(void)
FT_OpenType_Free(FT_Face   face,
                 FT_Bytes  table);


/* */


FT_END_HEADER

#endif /* __FTOTVAL_H__ */


/* END */
