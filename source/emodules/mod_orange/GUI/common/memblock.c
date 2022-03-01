/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º        memblock.c
*
*Author£º       William Wang
*
*Version :          1.1.0
*
*Date:              2009-8-4
*
*Description :  memblock function implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>                  <version>     <description>
*
* William Wang     2009-8-4         1.1.0          Create File
*
*******************************************************************************************************************
*/
#include "common_i.h"

/*
**********************************************************************************************************************
*                                               InitMemBlock
*
* author:
*
* date:             2009-7-21
*
* Description:      initmemblock
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void InitMemBlockLink(pmemblock heap, __u32 bd_size, __u32 heap_size)
{
    pthread_mutex_init(&heap->lock, (void *)1);

    if (heap->lock == NULL)
    {
        __wrn("create message queue mutex failed");
        return ;
    }

    heap->heap      = NULL;
    heap->bd_size   = bd_size + sizeof(__u32);
    heap->heap_size = heap_size;
    return ;
}

/*
**********************************************************************************************************************
*                                               MemBlockAlloc
*
* author:
*
* date:             2009-7-21
*
* Description:      memblockalloc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void *MemBlockAlloc(pmemblock heap)
{
    __u32       i;
    char       *block_data = NULL;
    pthread_mutex_lock(heap->lock);

    if (heap->heap == NULL)
    {
        heap->heap = orange_bmalloc(heap->heap_size * heap->bd_size);

        if (!heap->heap)
        {
            goto ret;
        }

        heap->free = 0;
    }

    block_data = (char *) heap->heap + heap->bd_size * heap->free;

    for (i = heap->free; i < heap->heap_size; i++)
    {
        if (*((__u32 *)block_data) == MBS_FREE)
        {
            heap->free = i + 1;
            *((__u32 *)block_data) = MBS_USED;
            goto ret;
        }

        block_data += heap->bd_size;
    }

    block_data = orange_malloc(heap->bd_size);

    if (!block_data)
    {
        goto ret;
    }

    *((__u32 *)block_data) = MBS_SPECIAL;
ret:
    pthread_mutex_unlock(heap->lock);

    if (block_data)
    {
        return block_data + sizeof(__u32);
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               MemBlockFree
*
* author:
*
* date:             2009-7-21
*
* Description:      memblockfree
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void MemBlockFree(pmemblock heap, void *data)
{
    __u32   i;
    char   *block_data;
    pthread_mutex_lock(heap->lock);
    block_data = (char *) data - sizeof(__u32);

    if (*((__u32 *)block_data) == MBS_SPECIAL)
    {
        orange_mfree(block_data);
    }
    else if (*((__u32 *)block_data) == MBS_USED)
    {
        *((__u32 *)block_data) = MBS_FREE;
        i = (block_data - (char *)heap->heap) / heap->bd_size;

        if (heap->free > i)
        {
            heap->free = i;
        }
    }

    pthread_mutex_unlock(heap->lock);
}

/*
**********************************************************************************************************************
*                                               DestroyMemBlockLink
*
* author:
*
* date:             2009-7-21
*
* Description:      destroymemblocklink
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void DestroyMemBlockLink(pmemblock heap)
{
    /*delete memlink sem*/
    pthread_mutex_destroy(heap->lock);
    orange_bfree(heap->heap, heap->heap_size * heap->bd_size);
}
