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
/*  ftcimage.h                                                             */
/*                                                                         */
/*    FreeType Generic Image cache (specification)                         */
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
/*
 *  FTC_ICache is an _abstract_ cache used to store a single FT_Glyph
 *  image per cache node.
 *
 *  FTC_ICache extends FTC_GCache.  For an implementation example,
 *  see FTC_ImageCache in `src/cache/ftbasic.c'.
 */


/*************************************************************************/
/*                                                                       */
/* Each image cache really manages FT_Glyph objects.                     */
/*                                                                       */
/*************************************************************************/


#ifndef __FTCIMAGE_H__
#define __FTCIMAGE_H__


#include "ft2build.h"
#include FT_CACHE_H
#include "ftcglyph.h"

FT_BEGIN_HEADER


/* the FT_Glyph image node type - we store only 1 glyph per node */
typedef struct  FTC_INodeRec_
{
    FTC_GNodeRec  gnode;
    FT_Glyph      glyph;

} FTC_INodeRec, *FTC_INode;

#define FTC_INODE( x )         ( (FTC_INode)( x ) )
#define FTC_INODE_GINDEX( x )  FTC_GNODE(x)->gindex
#define FTC_INODE_FAMILY( x )  FTC_GNODE(x)->family

typedef FT_Error
(*FTC_IFamily_LoadGlyphFunc)(FTC_Family  family,
                             FT_UInt     gindex,
                             FTC_Cache   cache,
                             FT_Glyph   *aglyph);

typedef struct  FTC_IFamilyClassRec_
{
    FTC_MruListClassRec        clazz;
    FTC_IFamily_LoadGlyphFunc  family_load_glyph;

} FTC_IFamilyClassRec;

typedef const FTC_IFamilyClassRec  *FTC_IFamilyClass;

#define FTC_IFAMILY_CLASS( x )  ((FTC_IFamilyClass)(x))

#define FTC_CACHE__IFAMILY_CLASS( x ) \
    FTC_IFAMILY_CLASS( FTC_CACHE__GCACHE_CLASS(x)->family_class )


/* can be used as a @FTC_Node_FreeFunc */
FT_LOCAL(void)
FTC_INode_Free(FTC_INode  inode,
               FTC_Cache  cache);

/* Can be used as @FTC_Node_NewFunc.  `gquery.index' and `gquery.family'
 * must be set correctly.  This function will call the `family_load_glyph'
 * method to load the FT_Glyph into the cache node.
 */
FT_LOCAL(FT_Error)
FTC_INode_New(FTC_INode   *pinode,
              FTC_GQuery   gquery,
              FTC_Cache    cache);

#if 0
/* can be used as @FTC_Node_WeightFunc */
FT_LOCAL(FT_ULong)
FTC_INode_Weight(FTC_INode  inode);
#endif


/* */

FT_END_HEADER

#endif /* __FTCIMAGE_H__ */


/* END */
