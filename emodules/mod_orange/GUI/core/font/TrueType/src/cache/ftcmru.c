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
/*  ftcmru.c                                                               */
/*                                                                         */
/*    FreeType MRU support (body).                                         */
/*                                                                         */
/*  Copyright 2003, 2004, 2006 by                                          */
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
#include FT_CACHE_H
#include "ftcmru.h"
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_DEBUG_H

#include "ftcerror.h"


FT_LOCAL_DEF(void)
FTC_MruNode_Prepend(FTC_MruNode  *plist,
                    FTC_MruNode   node)
{
    FTC_MruNode  first = *plist;

    if (first)
    {
        FTC_MruNode  last = first->prev;
#ifdef FT_DEBUG_ERROR
        {
            FTC_MruNode  cnode = first;

            do
            {
                if (cnode == node)
                {
                    fprintf(stderr, "FTC_MruNode_Prepend: invalid action!\n");
                    exit(2);
                }

                cnode = cnode->next;
            } while (cnode != first);
        }
#endif
        first->prev = node;
        last->next  = node;
        node->next  = first;
        node->prev  = last;
    }
    else
    {
        node->next = node;
        node->prev = node;
    }

    *plist = node;
}


FT_LOCAL_DEF(void)
FTC_MruNode_Up(FTC_MruNode  *plist,
               FTC_MruNode   node)
{
    FTC_MruNode  first = *plist;
    FT_ASSERT(first != NULL);

    if (first != node)
    {
        FTC_MruNode  prev, next, last;
#ifdef FT_DEBUG_ERROR
        {
            FTC_MruNode  cnode = first;

            do
            {
                if (cnode == node)
                {
                    goto Ok;
                }

                cnode = cnode->next;
            } while (cnode != first);

            fprintf(stderr, "FTC_MruNode_Up: invalid action!\n");
            exit(2);
Ok:
        }
#endif
        prev = node->prev;
        next = node->next;
        prev->next = next;
        next->prev = prev;
        last = first->prev;
        last->next  = node;
        first->prev = node;
        node->next = first;
        node->prev = last;
        *plist = node;
    }
}


FT_LOCAL_DEF(void)
FTC_MruNode_Remove(FTC_MruNode  *plist,
                   FTC_MruNode   node)
{
    FTC_MruNode  first = *plist;
    FTC_MruNode  prev, next;
    FT_ASSERT(first != NULL);
#ifdef FT_DEBUG_ERROR
    {
        FTC_MruNode  cnode = first;

        do
        {
            if (cnode == node)
            {
                goto Ok;
            }

            cnode = cnode->next;
        } while (cnode != first);

        fprintf(stderr, "FTC_MruNode_Remove: invalid action!\n");
        exit(2);
Ok:
    }
#endif
    prev = node->prev;
    next = node->next;
    prev->next = next;
    next->prev = prev;

    if (node == next)
    {
        FT_ASSERT(first == node);
        FT_ASSERT(prev  == node);
        *plist = NULL;
    }
    else if (node == first)
    {
        *plist = next;
    }
}


FT_LOCAL_DEF(void)
FTC_MruList_Init(FTC_MruList       list,
                 FTC_MruListClass  clazz,
                 FT_UInt           max_nodes,
                 FT_Pointer        data,
                 FT_Memory         memory)
{
    list->num_nodes = 0;
    list->max_nodes = max_nodes;
    list->nodes     = NULL;
    list->clazz     = *clazz;
    list->data      = data;
    list->memory    = memory;
}


FT_LOCAL_DEF(void)
FTC_MruList_Reset(FTC_MruList  list)
{
    while (list->nodes)
    {
        FTC_MruList_Remove(list, list->nodes);
    }

    FT_ASSERT(list->num_nodes == 0);
}


FT_LOCAL_DEF(void)
FTC_MruList_Done(FTC_MruList  list)
{
    FTC_MruList_Reset(list);
}


#ifndef FTC_INLINE
FT_LOCAL_DEF(FTC_MruNode)
FTC_MruList_Find(FTC_MruList  list,
                 FT_Pointer   key)
{
    FTC_MruNode_CompareFunc  compare = list->clazz.node_compare;
    FTC_MruNode              first, node;
    first = list->nodes;
    node  = NULL;

    if (first)
    {
        node = first;

        do
        {
            if (compare(node, key))
            {
                if (node != first)
                {
                    FTC_MruNode_Up(&list->nodes, node);
                }

                return node;
            }

            node = node->next;
        } while (node != first);
    }

    return NULL;
}
#endif

FT_LOCAL_DEF(FT_Error)
FTC_MruList_New(FTC_MruList   list,
                FT_Pointer    key,
                FTC_MruNode  *anode)
{
    FT_Error     error;
    FTC_MruNode  node;
    FT_Memory    memory = list->memory;

    if (list->num_nodes >= list->max_nodes && list->max_nodes > 0)
    {
        node = list->nodes->prev;
        FT_ASSERT(node);

        if (list->clazz.node_reset)
        {
            FTC_MruNode_Up(&list->nodes, node);
            error = list->clazz.node_reset(node, key, list->data);

            if (!error)
            {
                goto Exit;
            }
        }

        FTC_MruNode_Remove(&list->nodes, node);
        list->num_nodes--;

        if (list->clazz.node_done)
        {
            list->clazz.node_done(node, list->data);
        }
    }
    else if (FT_ALLOC(node, list->clazz.node_size))
    {
        goto Exit;
    }

    error = list->clazz.node_init(node, key, list->data);

    if (error)
    {
        goto Fail;
    }

    FTC_MruNode_Prepend(&list->nodes, node);
    list->num_nodes++;
Exit:
    *anode = node;
    return error;
Fail:

    if (list->clazz.node_done)
    {
        list->clazz.node_done(node, list->data);
    }

    FT_FREE(node);
    goto Exit;
}


#ifndef FTC_INLINE
FT_LOCAL_DEF(FT_Error)
FTC_MruList_Lookup(FTC_MruList   list,
                   FT_Pointer    key,
                   FTC_MruNode  *anode)
{
    FTC_MruNode  node;
    node = FTC_MruList_Find(list, key);

    if (node == NULL)
    {
        return FTC_MruList_New(list, key, anode);
    }

    *anode = node;
    return 0;
}
#endif /* FTC_INLINE */

FT_LOCAL_DEF(void)
FTC_MruList_Remove(FTC_MruList  list,
                   FTC_MruNode  node)
{
    FTC_MruNode_Remove(&list->nodes, node);
    list->num_nodes--;
    {
        FT_Memory  memory = list->memory;

        if (list->clazz.node_done)
        {
            list->clazz.node_done(node, list->data);
        }

        FT_FREE(node);
    }
}


FT_LOCAL_DEF(void)
FTC_MruList_RemoveSelection(FTC_MruList              list,
                            FTC_MruNode_CompareFunc  selection,
                            FT_Pointer               key)
{
    FTC_MruNode  first, node, next;
    first = list->nodes;

    while (first && (selection == NULL || selection(first, key)))
    {
        FTC_MruList_Remove(list, first);
        first = list->nodes;
    }

    if (first)
    {
        node = first->next;

        while (node != first)
        {
            next = node->next;

            if (selection(node, key))
            {
                FTC_MruList_Remove(list, node);
            }

            node = next;
        }
    }
}


/* END */
