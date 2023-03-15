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
/*  ftcmru.h                                                               */
/*                                                                         */
/*    Simple MRU list-cache (specification).                               */
/*                                                                         */
/*  Copyright 2000-2001, 2003, 2004, 2005, 2006 by                         */
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
/* An MRU is a list that cannot hold more than a certain number of       */
/* elements (`max_elements').  All elements in the list are sorted in    */
/* least-recently-used order, i.e., the `oldest' element is at the tail  */
/* of the list.                                                          */
/*                                                                       */
/* When doing a lookup (either through `Lookup()' or `Lookup_Node()'),   */
/* the list is searched for an element with the corresponding key.  If   */
/* it is found, the element is moved to the head of the list and is      */
/* returned.                                                             */
/*                                                                       */
/* If no corresponding element is found, the lookup routine will try to  */
/* obtain a new element with the relevant key.  If the list is already   */
/* full, the oldest element from the list is discarded and replaced by a */
/* new one; a new element is added to the list otherwise.                */
/*                                                                       */
/* Note that it is possible to pre-allocate the element list nodes.      */
/* This is handy if `max_elements' is sufficiently small, as it saves    */
/* allocations/releases during the lookup process.                       */
/*                                                                       */
/*************************************************************************/


#ifndef __FTCMRU_H__
#define __FTCMRU_H__


#include "ft2build.h"
#include FT_FREETYPE_H

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

#define  xxFT_DEBUG_ERROR
#define  FTC_INLINE

FT_BEGIN_HEADER

typedef struct FTC_MruNodeRec_  *FTC_MruNode;

typedef struct  FTC_MruNodeRec_
{
    FTC_MruNode  next;
    FTC_MruNode  prev;

} FTC_MruNodeRec;


FT_LOCAL(void)
FTC_MruNode_Prepend(FTC_MruNode  *plist,
                    FTC_MruNode   node);

FT_LOCAL(void)
FTC_MruNode_Up(FTC_MruNode  *plist,
               FTC_MruNode   node);

FT_LOCAL(void)
FTC_MruNode_Remove(FTC_MruNode  *plist,
                   FTC_MruNode   node);


typedef struct FTC_MruListRec_              *FTC_MruList;

typedef struct FTC_MruListClassRec_ const   *FTC_MruListClass;


typedef FT_Bool
(*FTC_MruNode_CompareFunc)(FTC_MruNode  node,
                           FT_Pointer   key);

typedef FT_Error
(*FTC_MruNode_InitFunc)(FTC_MruNode  node,
                        FT_Pointer   key,
                        FT_Pointer   data);

typedef FT_Error
(*FTC_MruNode_ResetFunc)(FTC_MruNode  node,
                         FT_Pointer   key,
                         FT_Pointer   data);

typedef void
(*FTC_MruNode_DoneFunc)(FTC_MruNode  node,
                        FT_Pointer   data);


typedef struct  FTC_MruListClassRec_
{
    FT_UInt                  node_size;
    FTC_MruNode_CompareFunc  node_compare;
    FTC_MruNode_InitFunc     node_init;
    FTC_MruNode_ResetFunc    node_reset;
    FTC_MruNode_DoneFunc     node_done;

} FTC_MruListClassRec;

typedef struct  FTC_MruListRec_
{
    FT_UInt              num_nodes;
    FT_UInt              max_nodes;
    FTC_MruNode          nodes;
    FT_Pointer           data;
    FTC_MruListClassRec  clazz;
    FT_Memory            memory;

} FTC_MruListRec;


FT_LOCAL(void)
FTC_MruList_Init(FTC_MruList       list,
                 FTC_MruListClass  clazz,
                 FT_UInt           max_nodes,
                 FT_Pointer        data,
                 FT_Memory         memory);

FT_LOCAL(void)
FTC_MruList_Reset(FTC_MruList  list);


FT_LOCAL(void)
FTC_MruList_Done(FTC_MruList  list);


FT_LOCAL(FT_Error)
FTC_MruList_New(FTC_MruList   list,
                FT_Pointer    key,
                FTC_MruNode  *anode);

FT_LOCAL(void)
FTC_MruList_Remove(FTC_MruList  list,
                   FTC_MruNode  node);

FT_LOCAL(void)
FTC_MruList_RemoveSelection(FTC_MruList              list,
                            FTC_MruNode_CompareFunc  selection,
                            FT_Pointer               key);


#ifdef FTC_INLINE

#define FTC_MRULIST_LOOKUP_CMP( list, key, compare, node, error )           \
    FT_BEGIN_STMNT                                                            \
    FTC_MruNode*             _pfirst  = &(list)->nodes;                     \
    FTC_MruNode_CompareFunc  _compare = (FTC_MruNode_CompareFunc)(compare); \
    FTC_MruNode              _first, _node, *_pnode;                        \
    \
    \
    error  = 0;                                                             \
    _first = *(_pfirst);                                                    \
    _node  = NULL;                                                          \
    \
    if ( _first )                                                           \
    {                                                                       \
        _node = _first;                                                       \
        do                                                                    \
        {                                                                     \
            if ( _compare( _node, (key) ) )                                     \
            {                                                                   \
                if ( _node != _first )                                            \
                    FTC_MruNode_Up( _pfirst, _node );                               \
                \
                _pnode = (FTC_MruNode*)(void*)&(node);                            \
                *_pnode = _node;                                                  \
                goto _MruOk;                                                      \
            }                                                                   \
            _node = _node->next;                                                \
            \
        } while ( _node != _first) ;                                          \
    }                                                                       \
    \
    error = FTC_MruList_New( (list), (key), (FTC_MruNode*)(void*)&(node) ); \
    _MruOk:                                                                   \
    ;                                                                       \
    FT_END_STMNT

#define FTC_MRULIST_LOOKUP( list, key, node, error ) \
    FTC_MRULIST_LOOKUP_CMP( list, key, (list)->clazz.node_compare, node, error )

#else  /* !FTC_INLINE */

FT_LOCAL(FTC_MruNode)
FTC_MruList_Find(FTC_MruList  list,
                 FT_Pointer   key);

FT_LOCAL(FT_Error)
FTC_MruList_Lookup(FTC_MruList   list,
                   FT_Pointer    key,
                   FTC_MruNode  *pnode);

#define FTC_MRULIST_LOOKUP( list, key, node, error ) \
    error = FTC_MruList_Lookup( (list), (key), (FTC_MruNode*)&(node) )

#endif /* !FTC_INLINE */


#define FTC_MRULIST_LOOP( list, node )        \
    FT_BEGIN_STMNT                              \
    FTC_MruNode  _first = (list)->nodes;      \
    \
    \
    if ( _first )                             \
    {                                         \
        FTC_MruNode  _node = _first;            \
        \
        \
        do                                      \
        {                                       \
            *(FTC_MruNode*)&(node) = _node;


#define FTC_MRULIST_LOOP_END()               \
    _node = _node->next;                 \
    \
    } while ( _node != _first );           \
    }                                        \
    FT_END_STMNT

/* */

FT_END_HEADER


#endif /* __FTCMRU_H__ */


/* END */
