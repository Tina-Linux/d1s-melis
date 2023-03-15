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
/*  ftccback.h                                                             */
/*                                                                         */
/*    Callback functions of the caching sub-system (specification only).   */
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
#ifndef __FTCCBACK_H__
#define __FTCCBACK_H__

#include "ft2build.h"
#include FT_CACHE_H
#include "ftcmru.h"
#include "ftcimage.h"
#include "ftcmanag.h"
#include "ftcglyph.h"
#include "ftcsbits.h"


FT_LOCAL(void)
ftc_inode_free(FTC_Node   inode,
               FTC_Cache  cache);

FT_LOCAL(FT_Error)
ftc_inode_new(FTC_Node   *pinode,
              FT_Pointer  gquery,
              FTC_Cache   cache);

FT_LOCAL(FT_ULong)
ftc_inode_weight(FTC_Node   inode,
                 FTC_Cache  cache);


FT_LOCAL(void)
ftc_snode_free(FTC_Node   snode,
               FTC_Cache  cache);

FT_LOCAL(FT_Error)
ftc_snode_new(FTC_Node   *psnode,
              FT_Pointer  gquery,
              FTC_Cache   cache);

FT_LOCAL(FT_ULong)
ftc_snode_weight(FTC_Node   snode,
                 FTC_Cache  cache);

FT_LOCAL(FT_Bool)
ftc_snode_compare(FTC_Node    snode,
                  FT_Pointer  gquery,
                  FTC_Cache   cache);


FT_LOCAL(FT_Bool)
ftc_gnode_compare(FTC_Node    gnode,
                  FT_Pointer  gquery,
                  FTC_Cache   cache);


FT_LOCAL(FT_Error)
ftc_gcache_init(FTC_Cache  cache);

FT_LOCAL(void)
ftc_gcache_done(FTC_Cache  cache);


FT_LOCAL(FT_Error)
ftc_cache_init(FTC_Cache  cache);

FT_LOCAL(void)
ftc_cache_done(FTC_Cache  cache);

#ifndef FT_CONFIG_OPTION_OLD_INTERNALS
FT_LOCAL(void)
ftc_node_destroy(FTC_Node     node,
                 FTC_Manager  manager);
#endif

#endif /* __FTCCBACK_H__ */

/* END */
