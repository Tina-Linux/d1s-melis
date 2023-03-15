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
/*  afmparse.h                                                             */
/*                                                                         */
/*    AFM parser (specification).                                          */
/*                                                                         */
/*  Copyright 2006 by                                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __AFMPARSE_H__
#define __AFMPARSE_H__


#include <ft2build.h>
#include FT_INTERNAL_POSTSCRIPT_AUX_H


FT_BEGIN_HEADER


FT_LOCAL(FT_Error)
afm_parser_init(AFM_Parser  parser,
                FT_Memory   memory,
                FT_Byte    *base,
                FT_Byte    *limit);


FT_LOCAL(void)
afm_parser_done(AFM_Parser  parser);


FT_LOCAL(FT_Error)
afm_parser_parse(AFM_Parser  parser);


enum  AFM_ValueType_
{
    AFM_VALUE_TYPE_STRING,
    AFM_VALUE_TYPE_NAME,
    AFM_VALUE_TYPE_FIXED,   /* real number */
    AFM_VALUE_TYPE_INTEGER,
    AFM_VALUE_TYPE_BOOL,
    AFM_VALUE_TYPE_INDEX    /* glyph index */
};


typedef struct  AFM_ValueRec_
{
    enum AFM_ValueType_  type;
    union
    {
        char     *s;
        FT_Fixed  f;
        FT_Int    i;
        FT_Bool   b;

    } u;

} AFM_ValueRec, *AFM_Value;

#define  AFM_MAX_ARGUMENTS  5

FT_LOCAL(FT_Int)
afm_parser_read_vals(AFM_Parser  parser,
                     AFM_Value   vals,
                     FT_Int      n);

/* read the next key from the next line or column */
FT_LOCAL(char *)
afm_parser_next_key(AFM_Parser  parser,
                    FT_Bool     line,
                    FT_UInt    *len);

FT_END_HEADER

#endif /* __AFMPARSE_H__ */


/* END */
