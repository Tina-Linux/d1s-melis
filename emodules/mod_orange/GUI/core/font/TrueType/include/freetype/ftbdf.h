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
/*  ftbdf.h                                                                */
/*                                                                         */
/*    FreeType API for accessing BDF-specific strings (specification).     */
/*                                                                         */
/*  Copyright 2002, 2003, 2004, 2006 by                                    */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __FTBDF_H__
#define __FTBDF_H__

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
/*    bdf_fonts                                                          */
/*                                                                       */
/* <Title>                                                               */
/*    BDF Files                                                          */
/*                                                                       */
/* <Abstract>                                                            */
/*    BDF specific API.                                                  */
/*                                                                       */
/* <Description>                                                         */
/*    This section contains the declaration of BDF specific functions.   */
/*                                                                       */
/*************************************************************************/


/**********************************************************************
 *
 * @enum:
 *    FT_PropertyType
 *
 * @description:
 *    A list of BDF property types.
 *
 * @values:
 *    BDF_PROPERTY_TYPE_NONE ::
 *      Value 0 is used to indicate a missing property.
 *
 *    BDF_PROPERTY_TYPE_ATOM ::
 *      Property is a string atom.
 *
 *    BDF_PROPERTY_TYPE_INTEGER ::
 *      Property is a 32-bit signed integer.
 *
 *    BDF_PROPERTY_TYPE_CARDINAL ::
 *      Property is a 32-bit unsigned integer.
 */
typedef enum  BDF_PropertyType_
{
    BDF_PROPERTY_TYPE_NONE     = 0,
    BDF_PROPERTY_TYPE_ATOM     = 1,
    BDF_PROPERTY_TYPE_INTEGER  = 2,
    BDF_PROPERTY_TYPE_CARDINAL = 3

} BDF_PropertyType;


/**********************************************************************
 *
 * @type:
 *    BDF_Property
 *
 * @description:
 *    A handle to a @BDF_PropertyRec structure to model a given
 *    BDF/PCF property.
 */
typedef struct BDF_PropertyRec_  *BDF_Property;


/**********************************************************************
 *
 * @struct:
 *    BDF_PropertyRec
 *
 * @description:
 *    This structure models a given BDF/PCF property.
 *
 * @fields:
 *    type ::
 *      The property type.
 *
 *    u.atom ::
 *      The atom string, if type is @BDF_PROPERTY_TYPE_ATOM.
 *
 *    u.integer ::
 *      A signed integer, if type is @BDF_PROPERTY_TYPE_INTEGER.
 *
 *    u.cardinal ::
 *      An unsigned integer, if type is @BDF_PROPERTY_TYPE_CARDINAL.
 */
typedef struct  BDF_PropertyRec_
{
    BDF_PropertyType  type;
    union
    {
        const char     *atom;
        FT_Int32        integer;
        FT_UInt32       cardinal;

    } u;

} BDF_PropertyRec;


/**********************************************************************
 *
 * @function:
 *    FT_Get_BDF_Charset_ID
 *
 * @description:
 *    Retrieves a BDF font character set identity, according to
 *    the BDF specification.
 *
 * @input:
 *    face ::
 *       A handle to the input face.
 *
 * @output:
 *    acharset_encoding ::
 *       Charset encoding, as a C string, owned by the face.
 *
 *    acharset_registry ::
 *       Charset registry, as a C string, owned by the face.
 *
 * @return:
 *   FreeType error code.  0 means success.
 *
 * @note:
 *   This function only works with BDF faces, returning an error otherwise.
 */
FT_EXPORT(FT_Error)
FT_Get_BDF_Charset_ID(FT_Face       face,
                      const char **acharset_encoding,
                      const char **acharset_registry);


/**********************************************************************
 *
 * @function:
 *    FT_Get_BDF_Property
 *
 * @description:
 *    Retrieves a BDF property from a BDF or PCF font file.
 *
 * @input:
 *    face :: A handle to the input face.
 *
 *    name :: The property name.
 *
 * @output:
 *    aproperty :: The property.
 *
 * @return:
 *   FreeType error code.  0 means success.
 *
 * @note:
 *   This function works with BDF _and_ PCF fonts.  It returns an error
 *   otherwise.  It also returns an error if the property is not in the
 *   font.
 *
 *   In case of error, `aproperty->type' is always set to
 *   @BDF_PROPERTY_TYPE_NONE.
 */
FT_EXPORT(FT_Error)
FT_Get_BDF_Property(FT_Face           face,
                    const char       *prop_name,
                    BDF_PropertyRec  *aproperty);

/* */

FT_END_HEADER

#endif /* __FTBDF_H__ */


/* END */
