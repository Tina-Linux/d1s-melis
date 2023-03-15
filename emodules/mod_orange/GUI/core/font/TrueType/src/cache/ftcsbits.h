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
/*  ftcsbits.h                                                             */
/*                                                                         */
/*    A small-bitmap cache (specification).                                */
/*                                                                         */
/*  Copyright 2000-2001, 2002, 2003, 2006 by                               */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __FTCSBITS_H__
#define __FTCSBITS_H__


#include "ft2build.h"
#include FT_CACHE_H
#include "ftcglyph.h"


FT_BEGIN_HEADER

#define FTC_SBIT_ITEMS_PER_NODE  16

typedef struct  FTC_SNodeRec_
{
    FTC_GNodeRec  gnode;
    FT_UInt       count;
    FTC_SBitRec   sbits[FTC_SBIT_ITEMS_PER_NODE];

} FTC_SNodeRec, *FTC_SNode;


#define FTC_SNODE( x )         ( (FTC_SNode)( x ) )
#define FTC_SNODE_GINDEX( x )  FTC_GNODE( x )->gindex
#define FTC_SNODE_FAMILY( x )  FTC_GNODE( x )->family

typedef FT_UInt
(*FTC_SFamily_GetCountFunc)(FTC_Family   family,
                            FTC_Manager  manager);

typedef FT_Error
(*FTC_SFamily_LoadGlyphFunc)(FTC_Family   family,
                             FT_UInt      gindex,
                             FTC_Manager  manager,
                             FT_Face     *aface);

typedef struct  FTC_SFamilyClassRec_
{
    FTC_MruListClassRec        clazz;
    FTC_SFamily_GetCountFunc   family_get_count;
    FTC_SFamily_LoadGlyphFunc  family_load_glyph;

} FTC_SFamilyClassRec;

typedef const FTC_SFamilyClassRec  *FTC_SFamilyClass;

#define FTC_SFAMILY_CLASS( x )  ((FTC_SFamilyClass)(x))

#define FTC_CACHE__SFAMILY_CLASS( x )  \
    FTC_SFAMILY_CLASS( FTC_CACHE__GCACHE_CLASS( x )->family_class )


FT_LOCAL(void)
FTC_SNode_Free(FTC_SNode  snode,
               FTC_Cache  cache);

FT_LOCAL(FT_Error)
FTC_SNode_New(FTC_SNode   *psnode,
              FTC_GQuery   gquery,
              FTC_Cache    cache);

#if 0
FT_LOCAL(FT_ULong)
FTC_SNode_Weight(FTC_SNode  inode);
#endif


FT_LOCAL(FT_Bool)
FTC_SNode_Compare(FTC_SNode   snode,
                  FTC_GQuery  gquery,
                  FTC_Cache   cache);

/* */

FT_END_HEADER

#endif /* __FTCSBITS_H__ */


/* END */
